<?php

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title'         => t('T-pane'),
  'description'   => t('Modify the output of a field using a few predefined processors'),
  'category'      => array(t('Mediacurrent Examples'), -9),
  'content type'  => 'majorpanes_t_pane_content_type_content_type',
  'defaults'      => array(
    'label'           => 'title', 
    'formatter'       => '', 
    'delta_limit'     => 0, 
    'delta_offset'    => '0', 
    'delta_reversed'  => FALSE,
    'filter'          => 'rewrite',
  )
);

/**
 * Generate the content type (pane) definitions
 */
function majorpanes_t_pane_content_type_content_type($subtype) {
  $types = majorpanes_t_pane_content_type_content_types();
  if (isset($types[$subtype])) {
    return $types[$subtype];
  }
}

/**
 * Utility function to generate the pane configs
 */
function majorpanes_t_pane_content_type_content_types() {
  $types = &drupal_static(__FUNCTION__, array());
  if (!empty($types)) {
    return $types;
  }

  // This will hold all the individual field content types.
  $context_types = array();
  $entities = entity_get_info();

  foreach ($entities as $entity_type => $entity) {
    foreach ($entity['bundles'] as $type => $bundle) {
      foreach (field_info_instances($entity_type, $type) as $field_name => $field) {
        if (!isset($types[$entity_type . ':' . $field_name])) {
          $types[$entity_type . ':' . $field_name] = array(
            'category'      => 'T-Pane '.t(ucfirst($entity_type)),
            'icon'          => 'icon_field.png',
            'title'         => t('Field: @widget_label (@field_name)', array(
              '@widget_label' => t($field['label']),
              '@field_name'   => $field_name,
            )),
            'description'   => t('Tamperable field on the referenced entity.'),
          );
        }
        $context_types[$entity_type . ':' . $field_name]['types'][$type] = $bundle['label'];
      }
    }
  }

  // Create the required context for each field related to the bundle types.
  foreach ($types as $key => $field_content_type) {
    list($entity_type, $field_name) = explode(':', $key, 2);
    $types[$key]['required context'] = new ctools_context_required(t(ucfirst($entity_type)), $entity_type, array(
      'type' => array_keys($context_types[$key]['types']),
    ));
    unset($context_types[$key]['types']);
  }

  return $types;
}

/**
 * Implements hook_content_type_admin_title.
 * This is what will appear in the pane preview
 */
function majorpanes_t_pane_content_type_admin_title($subtype, $conf, $context) {
  return t(
    'T-pane wrapper for @FIELD using filter @FILTER',
    array(
      'FILTER'  => $conf['filter']
    )
  );
}

/**
 * Implements hook_content_type_edit_form.
 * This is the local config form for the pane
 */
function majorpanes_t_pane_content_type_edit_form($form, &$form_state) {
  // Instance configuration for the form is in form_state['conf'];
  $conf = $form_state['conf'];

  $form['filter'] = array(
    '#type'           => 'select',
    '#title'          => t('Choose filter'),
    '#description'    => t('Choose the filter that you would like to apply to this field'),
    '#options'        => _t_pane_populate_filter_select($entity),
    '#default_value'  => $conf['filter'],
  );

  $plugin = feeds_tamper_get_plugin($conf['filter']);
  $ff = $plugin['form'];

  $tamper_form = call_user_func($ff, array('a','b', $conf));

  $form['#validate'][] = '_t_pane_settings_validate';
  $form = array_merge($form, $tamper_form);

  return $form;
}

function _t_pane_settings_validate($form, &$form_state) {
  foreach (element_children($form) as $key) {
    if (!empty($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
  $conf = $form_state['conf'];

  $plugin = feeds_tamper_get_plugin($conf['filter']);
  $f = $plugin['validate']($conf);

  $form_state['conf'] = $conf;
}

/**
 * Implements hook_content_type_edit_form_submit.
 */
function majorpanes_t_pane_content_type_edit_form_submit(&$form, &$form_state) {
  // For each part of the form defined in the 'defaults' array set when you
  // defined the content type, copy the value from the form into the array
  // of items to be saved. 

  foreach (element_children($form) as $key) {
    if (!empty($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

/**
 * Implements hook_content_type_render.
 */
function majorpanes_t_pane_content_type_render($subtype, $conf, $args, $context) {
  $block = new stdClass();

  $plugin = feeds_tamper_get_plugin($conf['filter']);

  $field_name = substr($subtype,5);

  $vals = field_get_items('node', $context->data, $field_name);

  // Get the raw value
  $field = $vals[0]['value'];


  $result = '';
  $source = '';
  $item_key = '';
  $element_key = '';
  // Tamper callbacks use the following parameters
  // $result, $item_key, $element_key, &$field, $settings, $source
  $result = call_user_func_array(
    (string)$plugin['callback'], 
    array(
      $result,
      $item_key,
      $element_key,
      &$field,
      $conf,
      $source
    )
  );
  $out = $field;

  $block->title   = 'Override this title'; 
  $block->content = 'test:'.$out;

  return $block;
}

/**
 * Utility function to populate the filter selector
 */
function _t_pane_populate_filter_select($entity) {
  $plugins = feeds_tamper_get_plugins();

  foreach ($plugins as $k => $v) {
    if ($v['category'] !== 'Text') continue;
    $opts[$k] = $v['name'];
  }

  return $opts;
}

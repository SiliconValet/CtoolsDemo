<?php

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title'       => t('Growing panes'),
  'description' => t('Show off a few more advanced features of panes'),
  'single'      => TRUE,
  'category'    => array(t('Mediacurrent Examples'), -9),
  'required context'  => new ctools_context_required(
    t('Cog node'),
    'node'
  ),
  'defaults'    => array(
    'threshold'     => 250,
    'notice'        => t('That is a lot of teeth!'),
  )
  // this example does not provide 'admin info', which would populate the
  // panels builder page preview.
);

/**
 * Implements hook_content_type_admin_title.
 * This is what will appear in the pane preview
 */
function majorpanes_growing_panes_content_type_admin_title($subtype, $conf, $context) {
  return t('Now we are starting to get somewhere');
}

/**
 * Implements hook_content_type_edit_form.
 * This is the local config form for the pane
 */
function majorpanes_growing_panes_content_type_edit_form($form, &$form_state) {
  // Instance configuration for the form is in form_state['conf'];
  $conf = $form_state['conf'];

  $form['threshold'] = array(
    '#type'           => 'textfield',
    '#title'          => t('Threshold'),
    '#description'    => t('Enter the threshold beyond which this notice should appear'),
    '#default_value'  => $conf['threshold'],
  );

  $form['notice'] = array(
    '#type'           => 'textarea',
    '#title'          => t('Notice'),
    '#description'    => t('Enter some custom content that you would like to appear in this "block"'),
    '#default_value'  => $conf['notice'],
  );

  return $form;
}

/**
 * Implements hook_content_type_edit_form_submit.
 */
function majorpanes_growing_panes_content_type_edit_form_submit(&$form, &$form_state) {
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
function majorpanes_growing_panes_content_type_render($subtype, $conf, $args, $context) {
  $block = new stdClass();

  if (
    ($context->data->type == 'cog') &&
    ($context->data->field_cog_tpi['und'][0]['value'] >= $conf['threshold'])
  ) {
    $block->title   = 'Override this title'; 
    $block->content = $conf['notice'];
  }

  return $block;
}


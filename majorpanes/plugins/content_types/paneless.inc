<?php

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  // Title as it will appear in the list of content panes
  'title'       => t('Relatively paneless'),

  // Appears in the hover text in the pane selection view
  'description' => t('No context content type - requires and uses no context.'),

  // 'single' => TRUE means that only one type is declared here.
  'single'      => TRUE,

  // The default context.
  'defaults'    => array(),

  // This is the group that the pane will appear in
  'category'    => array(t('Mediacurrent Examples'), -9),
);

/**
 * Run-time rendering of the body of the block.
 * Note that render function follows the naming structure below
 * MODULE_CTYPE_content_type_render
 *
 * @param $subtype
 * @param $conf
 *   Configuration as done at admin time.
 * @param $args
 * @param $context
 *   Context - in this case we don't have any.
 *
 * @return
 *   An object with at least title and content members.
 */
function majorpanes_paneless_content_type_render($subtype, $conf, $args, $context) {
  $block = new stdClass();

  // The title actually used in rendering
  $block->title   = check_plain("No-pane no gain");
  $block->content = t("We should try to make something a little more complex now.");

  return $block;
}


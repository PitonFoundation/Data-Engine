<?php
$context = new stdClass();
$context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
$context->api_version = 3;
$context->name = 'visualizations';
$context->description = 'Visualize page context';
$context->tag = 'path';
$context->conditions = array(
  'path' => array(
    'values' => array(
      'visualize' => 'visualize',
    ),
  ),
);
$context->reactions = array(
  'block' => array(
    'blocks' => array(
      'data_engine_browse-data_engine_browse_vis_filters' => array(
        'module' => 'data_engine_browse',
        'delta' => 'data_engine_browse_vis_filters',
        'region' => 'sidebar_first',
        'weight' => '-10',
      ),
    ),
  ),
);
$context->condition_mode = 0;

// Translatables
// Included for use with string extractors like potx.
t('Visualize page context');
t('path');

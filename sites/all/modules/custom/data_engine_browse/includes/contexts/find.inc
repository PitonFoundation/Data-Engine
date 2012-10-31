<?php
$context = new stdClass();
$context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
$context->api_version = 3;
$context->name = 'find';
$context->description = 'Context for the find datasets page';
$context->tag = 'path';
$context->conditions = array(
  'path' => array(
    'values' => array(
      'find' => 'find',
    ),
  ),
);
$context->reactions = array(
  'block' => array(
    'blocks' => array(
      'data_engine_browse-data_engine_browse_find_filters' => array(
        'module' => 'data_engine_browse',
        'delta' => 'data_engine_browse_find_filters',
        'region' => 'sidebar_first',
        'weight' => '-10',
      ),
    ),
  ),
);
$context->condition_mode = 0;

// Translatables
// Included for use with string extractors like potx.
t('Context for the find datasets page');
t('path');

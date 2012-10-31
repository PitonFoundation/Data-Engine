<?php
/**
 * @file Add visualization form
 */

/**
 * Visualization add form
 *
 * URL: visualize/add
 *
 * The form is broken down into sub-functions for each step. The step
 * functions are data_engine_visualize_add_form_N and with "_submit" on
 * the end of it.
 *
 */
function data_engine_visualize_add_form($form, &$form_state) {

  // add a backdoor letting admins clear their session
  global $user;

  if (
    $user->uid == 1 && 
    isset($_GET['reset'])
  ) {
    _data_engine_visualize_clear_session();
    drupal_goto('visualize/add');
    exit;
  } // if

  // Prompt anonymous users to login to create a visualization
  //-----------------------------------------------------------------

  if ($user->uid == 0) {
    drupal_goto('node/188', array('query' => drupal_get_destination()));
  } //if

  // figure out or initialize the step number. the form state and session are used
  // for storage, so we'll check both
  $step = 1;
  if (
    isset($form_state['storage']) && 
    isset($form_state['storage']['step'])
  ) {
    $step = intval($form_state['storage']['step']);
  } // if
  elseif (isset($_SESSION['visualization_step'])) {
    $step = intval($_SESSION['visualization_step']);
  } // elseif

  // initalize the storage array if it does not exist
  if (!isset($form_state['storage'])) {
    $form_state['storage'] = array();
  } // if

  // save our step in the form and in the session
  $form_state['storage']['step'] = $_SESSION['visualization_step'] = $step;

  // add the module JS
  drupal_add_js(drupal_get_path('module', 'data_engine_visualize') .'/js/add_visualization.js');

  // call the step-specific form handler
  $callback = 'data_engine_visualize_add_form_' . $step;
  return call_user_func($callback, $form, $form_state);

} // data_engine_visualize_add_form

/**
 * Submit handle for the add visualization form
 *
 * This function also breaks down submit handlers into step-specific
 * functions. Each has a name like data_engine_visualize_add_form_N_submit()
 */
function data_engine_visualize_add_form_submit($form, &$form_state) {

  $form_state['rebuild'] = TRUE;

  // handle someone starting over
  if (
    isset($form_state['values']['clear']) && 
    $form_state['values']['op'] == $form_state['values']['clear']
  ) {
    _data_engine_visualize_clear_session();
    $form_state['rebuild'] = FALSE;
    return;
  } // if

  // they hit the back button
  elseif (
    isset($form_state['values']['back']) && 
    $form_state['values']['op'] == $form_state['values']['back']
  ) {
    $form_state['storage']['step'] = $_SESSION['visualization_step'] = $form_state['storage']['step'] - 1;
    return;
  } // elseif

  // trigger multistep and call the step-specific submit handler
  $callback = 'data_engine_visualize_add_form_' . $form_state['storage']['step'] .'_submit';
  $form_state = call_user_func($callback, $form, $form_state);

} // data_engine_visualize_add_form_submit

/**
 * Validate handler for our form
 */
function data_engine_visualize_add_form_validate($form, &$form_state) {

  // skip validation if they are going back
  if (
    (
      isset($form_state['values']['back']) && 
      $form_state['values']['op'] == $form_state['values']['back']
    ) ||
    (
      isset($form_state['values']['clear']) && 
      $form_state['values']['op'] == $form_state['values']['clear']
    )
  ) {
    return;
  } // if

  // validate title
  if (isset($form_state['input']['title'])) {
    // check if the title is empty
    if (trim($form_state['input']['title']) == '') {
      form_set_error('title', t('The visualization title is required'));
    }
  }

  // validate categories
  if (
    isset($form_state['input']['field_data_categories']) &&
    isset($form_state['input']['field_data_categories'][LANGUAGE_NONE]) &&
    isset($form_state['input']['field_data_categories'][LANGUAGE_NONE][0]) &&
    count($form_state['input']['field_data_categories'][LANGUAGE_NONE][0]) == 0
  ) {
    form_set_error('field_data_categories', t('Please provide one or more categories for this visualization'));
  } // if

} // data_engine_visualize_add_form_validate

/**
 * STEP 1 of add visualization form
 */
function data_engine_visualize_add_form_1($form, &$form_state) {

  $form = array();

  // get the user's selected datasets
  $datasets = _data_engine_visualize_get_datasets($form_state);

  // Add the list of steps
  $form['steps'] = _data_engine_visualize_form_steps($form_state);

  // add the new datasets, if we can
  if (isset($form_state['values']['add_more_datasets'])) {
    $datasets[] = intval($form_state['values']['add_more_datasets']);
    $form_state['storage']['datasets'] = $_SESSION['visualization_datasets'] = $datasets;
  } // if

  drupal_add_js(drupal_get_path('module', 'data_engine_visualize') .'/js/add_visualization.js');

  // if the user did not select any datasets, we need to prompt them to do so
  if (!$datasets) {
    $form['select_data'] = array(
      '#markup' => '<p>' .
        t('You haven\'t added any data to visualize. Start by !link.', array(
          '!link' => l(t('finding some data'), 'find')
        )
      )
    );
  }
  // the user has selected some datasets
  elseif ($datasets) {

    $form['datasets_fieldset'] = array(
      '#type' => 'fieldset'
    );

    // create dataset checkboxes
    $datasetOptions = array();
    $datasetTypes = array();
    foreach($datasets as $dataset) {
      $node = node_load($dataset);
      if ($node) {
        $datasetOptions[$dataset] = $node->title;
      }
    }

    $form['datasets_fieldset']['datasets'] = array(
      '#title' => t('Selected Datasets'),
      '#type' => 'checkboxes',
      '#options' => $datasetOptions,
      '#default_value' => array_keys($datasetOptions),
      // '#value' => array_keys($datasetOptions)
    );

    if (count($datasets) < VISUALIZE_MAX_DATASETS) {
      // add dataset link
      $form['datasets_fieldset']['add_dataset'] = array(
        '#markup' => l(
          t('Add another dataset'),
          'find',
          array(
            'attributes' => array(
              'class' => array(
                'button--add-another',
                'ss-plus',
              ),
            ),
          )
        ),
        '#prefix' => '<div class="add-dataset">',
        '#suffix' => '</div>',
      );

      // get dataset options
      // currently we limit these options by those not in the current
      // datasets
      $options = array();
      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'dataset');
      $query->propertyCondition('status', 1)
        ->propertyCondition('nid', $datasets, 'NOT IN');
      $query->propertyOrderBy('title');
      $results = $query->execute();
      node_load_multiple(array_keys($results['node']));
      foreach ($results['node'] as $datasetNID => $dataset) {
        $node = node_load($datasetNID);
        if ($node) {
          $options[$node->nid] = $node->title;
        }
      }

      $form['datasets_fieldset']['add_more_datasets'] = array(
        '#title' => t('Select a new dataset to add'),
        '#type' => 'select',
        '#options' => $options,
      );
      $form['datasets_fieldset']['add_more_button'] = array(
        '#type' => 'button',
        '#value' => t('Add'),
        '#ajax' => array(
          'callback' => 'data_engine_visualize_add_form_datasets',
          'wrapper' => 'edit-datasets-fieldset',
          'method' => 'replace',
        )
      );
    }

    // buttons
    $form['create'] = array(
      '#type' => 'submit',
      '#value' => t('Next'),
    );
    $form['clear'] = array(
      '#type' => 'submit',
      '#value' => t('Start over'),
      '#attributes' => array(
        'class' => array('button--abort'),
      ),
    );
  }

  return $form;

} // data_engine_visualize_add_form_1

/**
 * AJAX handler for add form step 1
 */
function data_engine_visualize_add_form_datasets($form, &$form_state) {
  return $form['datasets_fieldset'];
} // data_engine_visualize_add_form_datasets

/**
 * Submit handle for step 1
 */
function data_engine_visualize_add_form_1_submit($form, &$form_state) {

  // update the step
  $form_state['storage']['step'] = $_SESSION['visualization_step'] = 2;

  // store the datasets in the form_state
  $datasets = array();
  if (isset($form_state['values']['datasets'])) {
    foreach($form_state['values']['datasets'] as $datasetNID => $selected) {
      if ($datasetNID == $selected) {
        $datasets[] = $datasetNID;
      }
    }
  }

  if (isset($form_state['values']['add_more_datasets']) && $form_state['values']['op'] != $form_state['values']['create']) {
    $dataset = intval($form_state['values']['add_more_datasets']);
    if (!in_array($dataset, $datasets)) {
      $datasets[] = $dataset;
    }
  }

  // limit to 2 datasets
  if (count($datasets) > VISUALIZE_MAX_DATASETS) {
    $datasets = array_slice($datasets, 0, VISUALIZE_MAX_DATASETS);
  }

  $form_state['storage']['datasets'] = $_SESSION['visualization_datasets'] = $datasets;

  return $form_state;

} // data_engine_visualize_add_form_1_submit

/**
 * Step 2 add visualization form
 *
 * User has selected their datasets, now they select what type of
 * visualization to create.
 */
function data_engine_visualize_add_form_2($form, &$form_state) {

  $form_state = _data_engine_visualize_sync_form_state($form_state);
  $datasets = $form_state['storage']['datasets'];

  // Add the list of steps
  $form['steps'] = _data_engine_visualize_form_steps($form_state);

  // add datasets

  $form['datasets'] = array(
    '#type' => 'fieldset', 
    '#title' => t('Choose the attributes you would like to visualize:'),
  );

  $settings = array('datasets' => array());

  foreach ($datasets as $dataset) {

    $datasetNode = node_load($dataset);

    $view = node_view($datasetNode, 'visualize_dataset');
    $form['datasets']['dataset_' . $dataset] = $view;
    $form['datasets']['dataset_' . $dataset .'_attributes'] = array(
      '#type' => 'fieldset',
    );

    // get dataset attributes
    $options = _data_engine_visualize_get_dataset_field_options($datasetNode);
    
    $selected = array();
    
    if (
      isset($form_state['storage']['datasetAttributes']) &&
      isset($form_state['storage']['datasetAttributes'][$dataset])
    ) {
      $selected = $form_state['storage']['datasetAttributes'][$dataset];
    } // if

    $index = data_engine_db_index_field_from_dataset($datasetNode);

    foreach ($options as $option => $label) {
      
      $option_form_api = str_replace('.', DATA_ENGINE_DB_PLACEHOLDER, $option);
      
      $form['datasets']['dataset_' . $dataset .'_attributes'][$option_form_api] = array(
        '#type' => 'checkbox',
        '#title' => $label,
      );
      
      if (in_array($option, $selected)) {
        $form['datasets']['dataset_' . $dataset .'_attributes'][$option_form_api]['#default_value'] = 1;   
      }  // if
      
      if ($option == $index) {
        $form['datasets']['dataset_' . $dataset .'_attributes'][$option_form_api]['#default_value'] = 1;
        $form['datasets']['dataset_' . $dataset .'_attributes'][$option_form_api]['#disabled'] = TRUE; 
      } // if
     
    } // foreach
    
    $value = NULL;
  
    // add some JS settings
    $setting = array('nid' => $dataset);

    if (
      isset($datasetNode->field_data_type) && 
      isset($datasetNode->field_data_type[$datasetNode->language])
    ) {
      $setting['type'] = $datasetNode->field_data_type[$datasetNode->language][0]['value'];
    } // if

    $settings['datasets'][$dataset] = $setting;

  } // foreach

  // add dataset information as JS variables for the add_visuzalition.js functionality
  $settings['number_of_datasets'] = count($datasets);
  drupal_add_js(array('data_engine_visualize' => $settings), 'setting');

  // add visualization types
  $visualizationTypes = data_engine_visualize_get_visualization_types();
  $form['visualizations'] = array(
    '#type' => 'fieldset', 
    '#title' => t('Choose the visualization type:'),
  );
  $visualizationOptions = array();

  foreach ($visualizationTypes as $visualizationType) {

    $term = taxonomy_term_load($visualizationType['tid']);
    $termView = taxonomy_term_view($term);
    $visualizationOptions[$term->tid] = drupal_render($termView);

  } // foreach

  $form['visualizations']['type'] = array(
    '#type' => 'radios',
    '#options' => $visualizationOptions,
    '#default_value' => $visualizationTypes[0]['tid']
  );

  // buttons
  $form['back'] = array(
    '#type' => 'submit',
    '#value' => t('Back'),
    '#attributes' => array(
      'class' => array('button--back'),
    ),
  );
  $form['adjust'] = array(
    '#type' => 'submit',
    '#value' => t('Next'),
    '#attributes' => array(
      'class' => array('button-next')
    )
  );

  _data_engine_visualize_form_start_over_button($form);

  return $form;

} // data_engine_visualize_add_form_2

/**
 * Add visualization step 2 submit handler
 */
function data_engine_visualize_add_form_2_submit($form, &$form_state) {
  
  // update the step
  $form_state['storage']['step'] = 3;
  
  $selectedFields = array();
  $datasets = $form_state['storage']['datasets'];

  foreach ($datasets as $dataset) {

    $datasetNode = node_load($dataset);
    $options = _data_engine_visualize_get_dataset_field_options($datasetNode);
    $selectedFields[$dataset] = array();

    foreach ($options as $option => $label) {
      
      $option_form_api = str_replace('.', DATA_ENGINE_DB_PLACEHOLDER, $option);

      if ($form_state['values'][$option_form_api]) {
        $selectedFields[$dataset][] =  $option;
      } // if
     
    } // foreach

  } // foreach

  // save the submitted setup to the form state/session
  $form_state['storage']['datasetAttributes'] = $selectedFields;
  $form_state['storage']['datasets'] = array_keys($selectedFields);

  // save type
  $form_state['storage']['type'] = $form_state['values']['type'];
  
  // save type name
  $visualizationType = taxonomy_term_load($form_state['values']['type']);
  $visualizationPath = drupal_lookup_path('alias', 'taxonomy/term/' . $visualizationType->tid);
  $path = explode('/', $visualizationPath);
  $form_state['storage']['typeName'] = array_pop($path);

  _data_engine_visualize_sync_session($form_state['storage']);

  return $form_state;

} // data_engine_visualize_add_form_2_submit

/**
 * Add visualization step 3
 */
function data_engine_visualize_add_form_3($form, &$form_state) {

  $form_state = _data_engine_visualize_sync_form_state($form_state);

  // Add the list of steps
  $form['steps'] = _data_engine_visualize_form_steps($form_state);

  // add the visualize space
  // -------------------------------------------
  // add a section tag for the this part of the form
  $form['visualize'] = array('#prefix' => '<section>', '#suffix' => '</section>');

  // get the visualization configuration
  $config = _data_engine_visualize_get_config($form_state);

  // get our visualization
  $visualization = module_invoke('data_engine_visualize', 'visualization', $config, array('pagination' => 0));


  if (!$visualization || !isset($visualization['html'])) {
    $visualization = array('html' => '<h3>Visualization Here</h3>');
  }
  $form['visualize']['visualization'] = array(
    '#markup' => $visualization['html'],
    '#prefix' => '<div id="visualization-preview">',
    '#suffix' => '</div>',
  );
  drupal_add_js(array('data_engine_visualize' => array('config' => $config, 'pagination' => 0, 'placeholder' => DATA_ENGINE_DB_PLACEHOLDER)), 'setting');

  // dataset attributes output
  // -------------------------------------------
  $datasets = _data_engine_visualize_get_datasets($form_state);
  $form_state['storage']['datasets'] = $_SESSION['visualization_datasets'] = $datasets;

  // wrapper/fieldset for our attributes
  $form['visualize']['attributes'] = array(
    '#prefix' => '<aside class="dataset-attributes">',
    '#suffix' => '</aside>',
    '#type' => 'fieldset',
  );

  foreach ($datasets as $dataset) {

    $datasetNode = node_load($dataset);

    $form['visualize']['attributes'][$dataset] = array(
      '#type' => 'fieldset',
    );

    // get dataset attributes
    $options = _data_engine_visualize_get_dataset_field_options($datasetNode);
    
    $selected = array();
    
    if (
      isset($form_state['storage']['datasetAttributes']) &&
      isset($form_state['storage']['datasetAttributes'][$dataset])
    ) {
      $selected = $form_state['storage']['datasetAttributes'][$dataset];
    } // if

    $index = data_engine_db_index_field_from_dataset($datasetNode);

    foreach ($options as $option => $label) {
      
      $option_form_api = str_replace('.', DATA_ENGINE_DB_PLACEHOLDER, $option);
      
      $form['visualize']['attributes'][$dataset][$option_form_api] = array(
        '#type' => 'checkbox',
        '#title' => $label,
      );
      
      if (in_array($option, $selected)) {
        $form['visualize']['attributes'][$dataset][$option_form_api]['#default_value'] = 1;   
      }  // if
      
      if ($option == $index) {
        $form['visualize']['attributes'][$dataset][$option_form_api]['#default_value'] = 1;
        $form['visualize']['attributes'][$dataset][$option_form_api]['#disabled'] = TRUE; 
      } // if
     
    } // foreach

  } // foreach

  $form['visualize']['attributes']['update_attributes'] = array(
    '#type' => 'button',
    '#value' => t('Update'),
  );

  // get the visualization form
  // -------------------------------------------
  module_load_include('inc', 'node', 'node.pages');
  $visualize = node_add('visualization');
  // was trying to remove the format section, but this fixed the other problem ... that it was above
  // the body field
  unset($visualize['body'][LANGUAGE_NONE][0]['format']);
  $form['visualization'] = array('#type' => 'fieldset');

  // title
  $form['visualization']['title'] = $visualize['title'];
  if (isset($form_state['storage']['title'])) {
    $form['visualization']['title']['#default_value'] = $form_state['storage']['title'];
    $form['visualization']['title']['#value'] = $form_state['storage']['title'];
  }

  // body
  $form['visualization']['body'] = $visualize['body'];
  if (isset($form_state['storage']['body'])) {
    $form['visualization']['body'][LANGUAGE_NONE][0]['#default_value'] = $form_state['storage']['body'];
  }

  // categories
  $form['visualization']['categories'] = $visualize['field_data_categories'];
  $form['visualization']['categories'][LANGUAGE_NONE]['#element_validate'] = array(); // remove validation
  if (isset($form_state['storage']['categories'])) {
    $form['visualization']['categories'][LANGUAGE_NONE]['#value'] = $form_state['storage']['categories'];
  }

  $form['visualization']['status'] = array(
    '#type' => 'radios',
    '#options' => array(1 => t('Published'), 0 => t('Unpublished')),
    '#weight' => 10,
    '#default_value' => 0
  );

  // buttons
  $form['back'] = array(
    '#type' => 'submit',
    '#value' => t('Back'),
    '#attributes' => array(
      'class' => array('button--back'),
    ),
  );
  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save')
  );
  _data_engine_visualize_form_start_over_button($form);

  $form['#submit'] = array('data_engine_visualize_add_form_submit');

  return $form;

} // data_engine_visualize_add_form_3

/**
 * Add visualization step 3 submit
 *
 * Create our visualization and clear the session
 */
function data_engine_visualize_add_form_3_submit($form, &$form_state) {

  global $user;

  $form_state = _data_engine_visualize_sync_form_state($form_state);

  // create the visualization
  $node = new stdClass();
  $node->type = 'visualization';
  $node->title = $form_state['storage']['title'] = $form_state['input']['title'];
  node_object_prepare($node);
  $node->language = LANGUAGE_NONE;
  $node->uid = $user->uid;
  $node->comments = 1;
  $node->promote = 0;
  $form_state['storage']['body'] = $form_state['input']['body'][LANGUAGE_NONE][0]['value'];
  $node->body = array(LANGUAGE_NONE => array($form_state['input']['body'][LANGUAGE_NONE][0]));
  $form_state['storage']['categories'] = $form_state['input']['field_data_categories'][LANGUAGE_NONE];
  $node->status = $form_state['storage']['status'] = $form_state['input']['status'];

  // save our configuration data
  $config = _data_engine_visualize_get_config($form_state);
  $node->field_configuration_data = array(LANGUAGE_NONE => array(array('value' => drupal_json_encode($config))));

  // save the visualization type for this node
  $node->field_visualization_type = array(LANGUAGE_NONE => array(array('tid' => $form_state['storage']['type'])));

  // save the datasets as utilized datasets
  $node->field_utilized_datasets = array(LANGUAGE_NONE => array());
  foreach ($form_state['storage']['datasets'] as $nid) {
    $node->field_utilized_datasets[LANGUAGE_NONE][] = array('target_id' => $nid);
  }

  $categories = drupal_explode_tags($form_state['storage']['categories']);
  $node->field_data_categories = array(LANGUAGE_NONE => array());
  foreach ($categories as $category) {
    $term = taxonomy_get_term_by_name($category, 'data_categories');
    if ($term) {
      $term = array_shift($term);
      $node->field_data_categories[LANGUAGE_NONE][] = array('tid' => $term->tid);
    }
  }

  // _data_engine_visualize_sync_session($form_state['storage']);
  _data_engine_visualize_clear_session();
  $node = node_submit($node);
  node_save($node);
  drupal_goto('node/' . $node->nid);

} // data_engine_visualize_add_form_3_submit

/**
 * Implements hook_permission().
 */
function data_engine_visualize_permission() {
  return array(
    'create unpublished visualization content' => array(
      'title' => t('%type: create unpublished content', array('%type' => 'Visualization')),
    ),
  );
} // data_engine_visualize_permission

/*=====================================================================
  INTERNAL FUNCTIONS
=====================================================================*/

/**
 * Check the input, sanitizes the dataset NIDs
 */
function _data_engine_visualize_check_datasets($datasets, $sessionDatasets = NULL) {
  if ($sessionDatasets) {
    $datasets = array_merge($datasets, $sessionDatasets);
  }

  // check the dataset NIDs to ensure they are datasets
  node_load_multiple($datasets);
  $dset = array();
  foreach ($datasets as $dataset) {
    $node = node_load($dataset);
    if (!$node || $node->type != 'dataset') {
      continue;
    }
    if (!in_array($dataset, $dset)) {
      $dset[] = $dataset;
    }
  }
  return $dset;
}

/**
 * Clear session data that relates to the visualize module
 */
function _data_engine_visualize_clear_session() {
  foreach ($_SESSION as $key => $value) {
    if (preg_match('/^visualization\_/', $key)) {
      unset($_SESSION[$key]);
    }
  }
}

/**
 * Convert the string that is passed from the GET parameters to dataset NIDs
 * @param  string $datasets The datasets from the GET parameter
 */
function _data_engine_visualize_convert_get_data($datasets) {
  if (!is_string($datasets)) {
    return array();
  }
  $datasets = explode(' ', $datasets);
  $datasets = array_map('intval', $datasets);
  if (count($datasets) > VISUALIZE_MAX_DATASETS) {
    $datasets = array_slice($datasets, 0, VISUALIZE_MAX_DATASETS);
  }
  return $datasets;
}

/**
 * Get the config object used to pass to data_engine_visualize_visualization
 * @return array
 */
function _data_engine_visualize_get_config($form_state) {

  $config = array(
    'type' => $form_state['storage']['type'],
    'typeName' => $form_state['storage']['typeName'],
    'datasets' => $form_state['storage']['datasets'],
    'attributes' => array(),
  );
    
  foreach ($form_state['storage']['datasetAttributes'] as $nid => $fields) {

    $config['attributes'][$nid] = array();

    foreach ($fields as $field) {
      $config['attributes'][$nid][$field] = $field;
    } // foreach

  } // foreach

  return $config;

} // _data_engine_visualize_get_config

/**
 * Wrapper for data_engine_db_fields_from_dataset
 * @param  object $node
 * @return array
 */
function _data_engine_visualize_get_dataset_field_options($node, $visualizationConfig = NULL) {
  global $databases;
  if (!isset($databases['data'])) {
    $attributes = array('field1' => 'Field 1', 'field2' => 'Field 2');
  }
  else {
    // get the field options from the actual data
    // if this is a chart, we'll limit this to just integers
    $types = FALSE;
    if ($visualizationConfig && $visualizationConfig['typeName'] == 'chart') {
      $types = array('Integer');
    }
    $attributes = data_engine_db_fields_from_dataset($node, $types);
  }
  return $attributes;
}

/**
 * Handle getting the visualization datasets from multiple sources
 * @param array $form_state The form_state array
 * @param array $session    (optional) The session array, mostly used for testing
 * @param array $get        (optional) The get array, mostly used for testing
 * @return array
 */
function _data_engine_visualize_get_datasets($form_state, $session = NULL, $get = NULL) {

  // if the session is not passed in, use the $_SESSION
  if (!$session && isset($_SESSION)) {
    $session = $_SESSION;
  }
  elseif (!$session && !isset($_SESSION)) {
    $session = array();
  }

  // get the session datasets
  $sessionDatasets = array();
  if (isset($session['visualization_datasets'])) {
    $sessionDatasets = $session['visualization_datasets'];
  }

  // get the form state datasets
  $datasets = array();
  if (isset($form_state['storage']['datasets'])) {
    $datasets = $form_state['storage']['datasets'];
  }

  // merge the form data and session datasets
  $datasets = array_merge($datasets, $sessionDatasets);

  // check for query string parameters
  if (!$get && isset($_GET)) {
    $get = $_GET;
  }
  elseif (!$get && !isset($_GET)) {
    $get = array();
  }
  if (isset($get['data'])) {
    $datasets = array_merge($datasets, _data_engine_visualize_convert_get_data($get['data']));
  }

  // validate this
  $datasets = _data_engine_visualize_check_datasets($datasets);

  // limit datasets
  if (count($datasets > VISUALIZE_MAX_DATASETS)) {
    $datasets = array_slice($datasets, 0, VISUALIZE_MAX_DATASETS);
  }

  // save the session to datasets
  if (isset($_SESSION)) {
    $_SESSION['visualization_datasets'] = $datasets;
  }

  return $datasets;

} // _data_engine_visualize_get_datasets

/**
 * Sync the form state storage with the session
 * @param  array $storage The form state storage
 */
function _data_engine_visualize_sync_session($storage) {
  foreach ($storage as $key => $value) {
    $_SESSION["visualization_$key"] = $value;
  }
}

/**
 * Sync the session to the form state
 * @param  array $form_state  The form state
 * @return array
 */
function _data_engine_visualize_sync_form_state($form_state) {
  if (isset($_SESSION)) {
    foreach ($_SESSION as $key => $value) {
      if (preg_match('/^visualization\_/', $key)) {
        $form_state_key = str_replace('visualization_', '', $key);
        if (!isset($form_state['storage'][$form_state_key])) {
          $form_state['storage'][$form_state_key] = $value;
        }
      }
    }
  }
  return $form_state;
}

/**
 * Create Visualize form steps
 * @param  array $form_state  The form state
 * @return array Render array for steps list
 */

function _data_engine_visualize_form_steps($form_state) {
  // Get the current step.
  $current_step = isset($form_state['storage']['step'])
    ? $form_state['storage']['step']
    : 0;

  // Create a render array or an ordered list.
  $step_list = array(
    '#theme' => 'item_list',
    '#type' => 'ol',
    '#items' => array(
      t('Choose datasets'),
      t('Choose comparisons &amp; visualization type'),
      t('Adjust visualization'),
    ),
    '#attributes' => array(
      'class' => array('step-list'),
    )
  );

  // Convert each step to a render array and add attributes.
  $index = 1;
  foreach ($step_list['#items'] as $key => $item) {
    $step_list['#items'][$key] = array(
      'data' => '<span class="step-title">' . $item. '</span>',
      'class' => array('step-item'),
    );
    // Set the active step.
    if ($index < $current_step) {
      $step_list['#items'][$key]['class'][] = 'is-complete';
    } elseif ($index == $current_step) {
      $step_list['#items'][$key]['class'][] = 'is-active';
    }
    $index++;
  }
  return $step_list;
}

/**
 * Buttons for step 2/3
 * @return array
 */
function _data_engine_visualize_form_start_over_button(&$form) {
  $form['clear'] = array(
    '#type' => 'submit',
    '#value' => t('Start over'),
    '#attributes' => array(
      'class' => array('button--abort'),
    ),
  );
}

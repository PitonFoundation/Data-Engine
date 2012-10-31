<?php

/**
* @file
* Views area node embed add handler.
*/

class views_handler_node_embed_add_area extends views_handler_area {

  function option_definition() {

    $options = parent::option_definition();
    $options['types'] = array('default' => array());
    return $options;

  } // option_definition

  function options_form(&$form, &$form_state) {

    parent::options_form($form, $form_state);

    $types = node_type_get_types();

    $form['types'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Content types'),
      '#default_value' => $this->options['types'],
      '#options' => array(
      ),
    );

    foreach ($types as $type) {
      $form['types']['#options'][$type->type] = $type->name;
    } // foreach

  } // options_form

  function render($empty = FALSE) {

    if (!$empty || !empty($this->options['empty'])) {
      
      $types = node_type_get_types();
      $add_links = array();

      foreach ($types as $type) {

        if ($this->options['types'][$type->type] !== 0) {
          $add_links[] = '<li>' . l('Add ' . $type->name, 'node/add/' . $type->type, array('query' => drupal_get_destination())) . '</li>';
        } // if

      } // foreach

      if (count($add_links) > 0) {

        return '<ul>' . implode('', $add_links) . '</ul>';

      } // if
      
    } // if

    return '';

  } // render
  
} // views_handler_node_embed_add_area

<?php
/**
 * @file
 */
require_once 'DataEngineVisualization.inc';
require_once 'DataEngineChartVisualization.inc';
require_once 'DataEngineDataTableVisualization.inc';
require_once 'DataEngineMapVisualization.inc';

/**
 * Factory for creating the concrete visualization objects
 */
class DataEngineVisualizationFactory {

  /**
   * Create the visualization object for this visualization object
   * @param  object $visualization The visualization node
   * @return object                The appropriate visualization object
   */
  public static function create($visualization) {
    if (is_object($visualization)) {
      // get the configuration for this visualization
      if (isset($visualization->field_configuration_data[$visualization->language])
        && isset($visualization->field_configuration_data[$visualization->language][0])) {
        try {
          $config = drupal_json_decode($visualization->field_configuration_data[$visualization->language][0]['value']);
        }
        catch (Exception $e) {
          throw new Exception('Could not parse configuration for this node: ' . $e->getMessage());
        }
      }
    }
    else {
      $config = $visualization;
    }

    $type = str_replace(' ', '', ucwords(str_replace('-', ' ', $config['typeName'])));
    $className = 'DataEngine' . $type . 'Visualization';
    if (class_exists($className)) {
      $vis = new $className;
      if (is_object($visualization)) {
        $vis->setVisualization($visualization);
      }
      else {
        $vis->config = $config;
      }
      $vis->init();
      return $vis;
    }

    throw new Exception('Invalid visualization class: ' . $className);
  }
}

<?php
class DataEngineVisualization {
  public $config;
  public $datasets = array();
  public $visualization;
  public $fields = array();

  public function __construct($visualization = NULL) {
    if ($visualization) {
      $this->setVisualization($visualization);
    }
  }

  public function convertHeader($header) {
    return strtolower(str_replace(array('.', '_'), array('', '-'), $header));
  }

  /**
   * Format the data to the internal structure we need to later retrieve the data
   * @param  array $data The data array
   * @return array
   */
  public function formatData($dataset, $data) {
    $reformattedData = array();
    if ($data) {
      foreach ($data as $row) {
        $reformattedRow = array();
        foreach ($row as $key => $value) {
          $reformattedRow[$dataset->nid . '.' . $key] = $value;
        }
        $reformattedData[] = $reformattedRow;
      }
    }
    $data = $reformattedData;
    return $data;
  }

  /**
   * Get the data from the database
   * @param  object  $dataset Dataset node
   * @param  mixed $start     Where to offset the results to
   * @param  mixed $end       Where to limit the results at
   * @return array
   */
  public function getData($dataset, $start = FALSE, $end = FALSE) {
    $data = data_engine_db_records_from_node($dataset, $start, $end);

    // now we need for reformat the keys to be nid.key
    return $this->formatData($dataset, $data);
  }

  /**
   * Return the value of the key in the data row
   *
   * This is a bit of a strange bit of functionality. In different cases (especially AJAX),
   * the visualizations may get a row key in different formats. Sometimes it will be:
   *
   *    nid.table.key
   *    nid.tablekey
   *    nid.table_with_underscores.key
   *    nid.table-with-underscores.key
   *
   * @param  array $dataRow  The data row
   * @param  string $rowKey  The row key that is requested
   * @return value or FALSE
   */
  public function getDataRowValue($dataRow, $rowKey) {
    if (isset($dataRow[$rowKey])) {
      return $dataRow[$rowKey];
    }

    $normalizedKey = str_replace('-', '_', $rowKey);
    $keyMap = array();
    foreach (array_keys($dataRow) as $key) {
      $keyMap[str_replace('-', '_', $key)] = $key;
      $keyMap[$key] = $key;
      $keyParts = explode('.', $key);
      if (count($keyParts) == 3) {
        $newKey = "{$keyParts[0]}.{$keyParts[1]}{$keyParts[2]}";
        $keyMap[$newKey] = $key;
      }
    }

    if (isset($keyMap[$normalizedKey])) {
      return $dataRow[$keyMap[$normalizedKey]];
    }

    return FALSE;
  }

  /**
   * Get the label for a field
   * @param  string $field The field name with the table ID first
   * @return string        The field label
   */
  public function getFieldLabel($field) {
    // get field name that will be in the fields list
    $fieldParts = explode('.', $field);
    array_shift($fieldParts);

    // normalize key
    $label = $this->normalize(implode('.', $fieldParts));

    // get the label
    if (isset($this->fields[$label])) {
      return $this->fields[$label];
    }

    $alerts = &drupal_static(__FUNCTION__);
    if (!isset($alerts)) {
      $alerts = array();
    }
    if (!in_array($label, $alerts)) {
      $alerts[] = $label;
    }

    // the label was not found
    return $field;
  }

  /**
   * Get the field names for this dataset
   * @param  object $dataset The dataset node
   * @return array
   */
  public function getFieldNames($dataset) {
    $cacheKey = 'data_engine_visualize_fields_' . $dataset->nid;

    // if the cache function exists, we can cache this value
    $useCache = function_exists('cache_get');

    // check if we have the cached value
    if ($useCache) {
      if ($cache = cache_get($cacheKey)) {
        $fields = $cache->data;
        return $fields;
      }
    }

    $fields = data_engine_db_fields_from_dataset($dataset);
    // make sure to apply converted header names
    foreach ($fields as $fieldName => $fieldInfo) {
      $fields[$this->normalize($fieldName)] = $fieldInfo;
    }

    // save the cached value
    if ($useCache) {
      cache_set($cacheKey, $fields, 'cache', CACHE_TEMPORARY);
    }
    return $fields;
  }

  /**
   * Determines if this install has a "data" database connection
   * @return bool
   */
  public function hasDataConnection() {
    global $databases;
    if (isset($databases['data'])) {
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Initialize steps
   */
  public function init() {
    $this->loadDatasets();
  }

  /**
   * Pre load the dataset nodes
   */
  public function loadDatasets() {
    node_load_multiple(array_keys($this->config['attributes']));
  }

  public function normalize($str) {
    return str_replace(
      array('.', '-'),
      array('', '_'),
      $str
    );
  }

  /**
   * Render the visualization
   * @return string The HTML output for the visualization
   */
  public function render() {
    return '';
  }

  /**
   * If this visualization has a visualization node, set it here
   * @param object $visualization Visualization node
   */
  public function setVisualization($visualization) {
    $this->visualization = $visualization;

    // load the configuration if it exists
    if (isset($visualization->field_configuration_data[$visualization->language])
      && isset($visualization->field_configuration_data[$visualization->language][0])) {
      $this->config = drupal_json_decode($visualization->field_configuration_data[$visualization->language][0]['value']);
    }
    else {
      $this->config = array();
    }
  }

  /**
   * Determine if we should use the field value
   *
   * Since we're using checkboxes, the fields array is a set of key/values that look
   * represent the field name and either 0 (don't use) or the field name (use).
   *
   * @param  string $field
   * @param  string $value
   * @return boolean
   */
  public function useField($field, $value) {
    return $field == $value;
  }
}

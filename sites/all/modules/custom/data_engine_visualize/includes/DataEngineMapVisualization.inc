<?php
class DataEngineMapVisualization extends DataEngineVisualization {
  /**
   * Default zoom for the map visualization
   * @var integer
   */
  public $zoom = 9;

  /**
   * Default centering for the map.
   *
   * Currently set to Denver, CO
   *
   * @var array
   */
  public $center = array('lat' => '39.739167', 'lon' => '-104.984722');

  public $range = array();
  public $color_attribute = FALSE;

  public $data_colors = array(
    '#CCFFFF',
    '#99CCCC',
    '#339999',
    '#00737D',
  );

  public function visualizationID($args = NULL) {
    return 'dataset-visualization-map';
  }

  /**
   * Get the features for this map
   */
  public function data($args = NULL) {
    $features = &drupal_static(__FUNCTION__);
    if (!isset($features)) {
      $dataset = array_slice($this->config['datasets'], 0, 1);
      $dataset = node_load($dataset[0]);
      $data = $this->getData($dataset);
      $this->fields = $this->getFieldNames($dataset);

      // Set the color attribute to first non-index attribute.

      $index = data_engine_db_index_field_from_dataset($dataset);

      foreach ($this->config['attributes'][$dataset->nid] as $attribute) {
        if ($attribute != $index && $attribute != 'null') {
          $this->color_attribute = $dataset->nid . '.' . $attribute;
          break;
        } // if
      } // foreach

      // create our list of features

      $features = array();

      if ($data) {
      
        $this->setColorRange($data, $dataset->nid);
      
        foreach ($data as $record) {

          $popup = $this->formatPopup($dataset, $record);

          if (isset($record[$dataset->nid . '.geojson'])) {
            // the geojson from the record is just the geometry portion of a GeoJSON Feature,
            // so we need to create the rest of the feature with that information
            $geoJSONFeature = array(
              'type' => 'Feature',
              'geometry' => drupal_json_decode($record[$dataset->nid . '.geojson']),
              'properties' => array(
                'popup' => $popup, 
                'style' => array(
                  'color' => $this->recordColor($record),
                  'fillOpacity' => 0.8,
                ),
              ),
            );
            $features[] = array(
              'type' => 'json',
              'json' => $geoJSONFeature,
            );
          }

        } // foreach

      } // if
      else {
        // error so track it and display something for the user
        watchdog('data_engine_visualize', 'No geodata for: ' . $dataset->title . ' [#' . $dataset->nid . ']', array(), WATCHDOG_ERROR);
        drupal_set_message(t('There is no geodata for this dataset or there is an error'), 'error');
      }
    }
    return $features;
  }

  /**
   * Create the text for the feature popup
   * @param  object $dataset The dataset node
   * @param  array  $record  The DB record
   * @param  bool   $debug
   * @return string
   */
  public function formatPopup($dataset, $record, $debug = FALSE) {
    $items = array();
    foreach ($this->config['attributes'][$dataset->nid] as $field => $useField){
      if ($this->useField($field, $useField)) {
        $rowKey = strtolower($dataset->nid . '.' . $field);
        $dataValue = $this->getDataRowValue($record, $rowKey);
        if ($debug) {
          dpm($dataValue);
        }
        if ($dataValue !== FALSE) {
          $items[$this->getFieldLabel($rowKey)] = $dataValue;
        }
      }
    }
    $list = '<dl>';
    foreach ($items as $label => $value) {
      $list .= "<dt>$label</dt><dd>$value</dd>";
    }
    $list .= '</dl>';
    return $list;
  }

  /**
   * Get the center point for the map.
   *
   * We try to use the center of the dataset, but will fall back to the class center
   * if we cannot get that information.
   * @param  object $dataset The dataset to get the center form
   * @return array
   */
  public function getCenter($dataset = NULL) {
    if ($dataset) {
      // $center = data_engine_db_geo_center_from_node($dataset);
      // @todo use when fixed
    }
    return $this->center;
  }

  /**
   * Override the DataEngineVisualization::getData() method
   * @param object $dataset The dataset node
   */
  public function getData($dataset, $start = FALSE, $end = FALSE) {
    // get the data from cache, if it available
    $cacheKey = 'data_engine_visualize_geodata_' . $dataset->nid;
    if ($cache = cache_get($cacheKey)) {
      $data = $cache->data;
    }
    else {
      $data = data_engine_db_geo_records_from_node($dataset, $start, $end);
      cache_set($cacheKey, $data, 'cache', CACHE_TEMPORARY);
    }
    return $this->formatData($dataset, $data);
  }
  
  /**
   * Gets color for a given record.
   */
  public function recordColor($record) {
    
    $segment = $this->range['difference'] / count($this->data_colors);
    
    if (!isset($record[$this->color_attribute])) {
      return $this->data_colors[0];
    } // if

    $index = 1;
    
    while ($index < count($this->data_colors)) {
      
      $high = $this->range['low'] + ($segment * $index);
      if ($record[$this->color_attribute] < $high) {
        return $this->data_colors[$index - 1];
      } // if
      
      $index++;
      
    } // while

    return end($this->data_colors);
    
  } // recordColor
  
  /**
   * Creates legend.
   */
  public function legendSettings() {
    
    $field_parts = explode('.', $this->color_attribute);
    array_shift($field_parts);
    $field = implode('.', $field_parts);

    $segment = $this->range['difference'] / count($this->data_colors);

    $parts = array();
    $index = 1;
    
    while ($index < count($this->data_colors)) {
      
      $low = $this->range['low'] + ($segment * ($index - 1));
      $high = $this->range['low'] + ($segment * $index);

      $parts[] = '<span style="background: ' . $this->data_colors[$index - 1] . '; display: inline-block; height: 0.8em; width: 0.8em;"></span> ' . $low . ' to ' . $high;
      $index++;
      
    } // while
    
    $parts[] = '<span style="background: ' . end($this->data_colors) . '; display: inline-block; height: 0.8em; width: 0.8em;"></span> ' . $high . ' to ' . $this->range['high'];

    return array('maplegend' => '<strong>' . $this->fields[$field] . '</strong><div>' . implode('</div><div>', $parts) . '</div>');
    
  } // legendSettings
  
  public function setColorRange($data) {
    
    foreach ($data as $record) {
      
      $value = $record[$this->color_attribute];
      
      if (!isset($this->range['low']) || $value < $this->range['low']) {
        $this->range['low'] = $value;
      } // if
      
      if (!isset($this->range['high']) || $value > $this->range['high']) {
        $this->range['high'] = $value;
      } // if
      
    } // foreach
    
    $this->range['difference'] = $this->range['high'] - $this->range['low'];
    
  } // setColorRange

  /**
   * Override the init method
   *
   * Adds a call to the data() method
   */
  public function init() {
    parent::init();
    $this->data();
  }

  /**
   * Render the map
   * @param  [type] $args [description]
   * @return [type]       [description]
   */
  public function render($args = NULL) {
    // if no arguments, use $_GET
    if ($args === NULL) {
      $args = $_GET;
    }

    drupal_add_js(drupal_get_path('module', 'data_engine_visualize') . '/js/data_engine_visualize.map.js');

    // load the first dataset
    $dataset = array_slice($this->config['datasets'], 0, 1);
    $dataset = node_load($dataset[0]);
    $this->fields = $this->getFieldNames($dataset);

    // get the record count and data for this dataset
    $count = data_engine_db_record_count_from_node($dataset);
    $features = $this->data($args);

    drupal_add_js(array('data_engine_visualize' => $this->legendSettings()), 'setting');


    // get the map and configure
    $map = leaflet_leaflet_map_info();
    $map['OSM Mapnik']['center'] = $this->getCenter($dataset);
    $map['OSM Mapnik']['settings']['zoom'] = $this->zoom;
    $map['OSM Mapnik']['settings']['scrollWheelZoom'] = FALSE;
    // $map['OSM Mapnik']['label'] = 'Mapbox';
    // $map['OSM Mapnik']['layers']['earth']['urlTemplate'] = "http://{s}.tiles.mapbox.com/v3/mapbox.world-glass/{z}/{x}/{y}.png";
    // $map['OSM Mapnik']['layers']['earth']['attributes']['attribution'] = 'Mapbox';

    return leaflet_render_map($map['OSM Mapnik'], $features);
  }
}

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
  public $centroid = 0;
  public $color_attribute = array();

  public $data_colors = array(
    '#CCFFFF',
    '#99CCCC',
    '#339999',
    '#00737D',
  );

  public $point_data_colors = array(
    '#FF6633',
    '#CC3333',
    '#993300',
    '#5E1A15',
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

      $datasets = array();
  
      foreach ($this->config['datasets'] as $dataset_nid) {
        $datasets[] = node_load($dataset_nid);
      } // foreach

      if (count($datasets) == 1) {
        $this->fields = $this->getFieldNames($datasets[0]);
      } // if
      else {
        $this->fields = $this->getFieldNames($datasets[0]) + $this->getFieldNames($datasets[1]);
      } // else      
            
      $features = array();
      
      // Swap datasets if first one is points.
      
      $data = $this->getData($datasets[0], 0, 1);
      
      if (strpos($data[0][$datasets[0]->nid . '.geojson'], 'Point') > 0) {
        $datasets = array_reverse($datasets);
        $this->config['datasets'] = array_reverse($this->config['datasets']);
      } // if
   
      foreach ($datasets as $dataset_index => $dataset) {

        $this->config['attributes'][$dataset->nid] = data_engine_db_set_display_name_attribute($this->config['attributes'][$dataset->nid], $dataset);

        $data = $this->getData($dataset, 0, 2000, ($this->centroid == ($dataset_index + 1)));
        
        if (!$this->centroid) {
          $this->centroid = $dataset_index + 2;
        } // if

        // Set the color attribute to first non-index attribute.

        $index = data_engine_db_index_field_from_dataset($dataset);

        if (!$this->color_attribute[$dataset->nid]) {

          foreach ($this->config['attributes'][$dataset->nid] as $attribute) {
            if ($attribute != $index && $attribute != 'null') {
              $this->color_attribute[$dataset->nid] = $dataset->nid . '.' . $attribute;
              break;
            } // if
          } // foreach

        } // if
  
        // create our list of features
    
        if ($data) {
        
          $this->setColorRange($data, $dataset->nid);
      
          foreach ($data as $record) {

            $popup = $this->formatPopup($dataset, $record);

            if (isset($record[$dataset->nid . '.geojson'])) {
            
              try {
            
                // the geojson from the record is just the geometry portion of a GeoJSON Feature,
                // so we need to create the rest of the feature with that information
                $geoJSONFeature = array(
                  'type' => 'Feature',
                  'geometry' => drupal_json_decode($record[$dataset->nid . '.geojson']),
                  'properties' => array(
                    'popup' => $popup, 
                    'style' => array(
                      'fillColor' => $this->recordColor($record, $dataset->nid, ($this->centroid == $dataset_index + 1)),
                      'color' => '#cccccc',
                      'fillOpacity' => 0.8,
                      'stroke' => TRUE,
                      'weight' => 3,
                    ),
                  ),
                );
                $features[] = array(
                  'type' => 'json',
                  'json' => $geoJSONFeature,
                );
              
              } // try
              catch (Exception $e) {}
            
            } // if
            
  
          } // foreach
  
        } // if
        else {
          // error so track it and display something for the user
          watchdog('data_engine_visualize', 'No geodata for: ' . $dataset->title . ' [#' . $dataset->nid . ']', array(), WATCHDOG_ERROR);
          drupal_set_message(t('There is no geodata for this dataset or there is an error'), 'error');
        } // else

      } // foreach

    } // if

    return $features;

  } // data

  /**
   * Create the text for the feature popup
   * @param  object $dataset The dataset node
   * @param  array  $record  The DB record
   * @param  bool   $debug
   * @return string
   */
  public function formatPopup(&$dataset, $record, $debug = FALSE) {
    
    $items = array();
    
    $pkeys = data_engine_db_pkeys_for_node($dataset);
    
    foreach ($pkeys as $pkey) {
      
      $rowKey = strtolower($dataset->nid . '.' . $pkey);
      $dataValue = $this->getDataRowValue($record, $rowKey);
      
      if ($dataValue !== FALSE) {
        $items[$this->getFieldLabel($rowKey)] = $dataValue;
      } // if

    } // forach

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
  public function getData($dataset, $start = FALSE, $end = FALSE, $centroid = FALSE) {

    $data = data_engine_db_geo_records_from_node($dataset, $start, $end, $centroid, $this->config['filters'][$dataset->nid]);
    return $this->formatData($dataset, $data);
  }
  
  /**
   * Gets color for a given record.
   */
  public function recordColor($record, $nid, $point = FALSE) {
    
    $colors = $this->data_colors;
    
    if ($point) {
      $colors = $this->point_data_colors;
    } // if
    
    $segment = $this->range[$nid]['difference'] / count($colors);
    
    if (is_null($record[$this->color_attribute[$nid]])) {
      return '#fff';
    } // if
    
    if (!isset($record[$this->color_attribute[$nid]])) {
      return $colors[0];
    } // if

    $index = 1;
    
    while ($index < count($colors)) {
      
      $high = $this->range[$nid]['low'] + ($segment * $index);
      if ($record[$this->color_attribute[$nid]] < $high) {
        return $colors[$index - 1];
      } // if
      
      $index++;
      
    } // while

    return end($colors);
    
  } // recordColor
  
  /**
   * Creates legend.
   */
  public function legendSettings() {
    
    $parts = array();
      
    foreach ($this->config['datasets'] as $dataset_index => $nid) {
      
      $colors = $this->data_colors;
      $style = 'display: inline-block; height: 0.8em; width: 0.8em; border: 1px #CCCCCC solid;';

      if ($this->centroid == ($dataset_index + 1)) {
        $style .= ' border-radius: 0.4em;';
        $colors = $this->point_data_colors;
      } // if
      
      $field_parts = explode('.', $this->color_attribute[$nid]);
      array_shift($field_parts);
      $field = implode('.', $field_parts);
  
      $parts[] = '<strong>' . $this->fields[$field] . '</strong>';
  
      $segment = $this->range[$nid]['difference'] / count($colors);
  
      $index = 1;
            
      while ($index < count($colors)) {
        
        $low = $this->range[$nid]['low'] + ($segment * ($index - 1));
        $high = $this->range[$nid]['low'] + ($segment * $index);
    
        $parts[] = '<div><span style="background: ' . $colors[$index - 1] . '; ' . $style . '"></span> ' . $low . ' to ' . $high . '</div>';
        $index++;
        
      } // while
      
      $parts[] = '<div><span style="background: ' . end($colors) . '; ' . $style . '"></span> ' . $high . ' to ' . $this->range[$nid]['high'] . '</div>';
      $parts[] = '<div><span style="background: #fff; ' . $style . '"></span> No value</div>';

    } // foreach

    return array('maplegend' => implode('', $parts));
    
  } // legendSettings
  
  public function setColorRange($data, $nid) {
    
    $this->range[$nid] = array();
    
    foreach ($data as $index => $record) {
      
      $value = $record[$this->color_attribute[$nid]];
      
      if (!isset($this->range[$nid]['low']) || ($value !== '' && !is_null($value) && floatval($value) < floatval($this->range[$nid]['low']))) {
        $this->range[$nid]['low'] = floatval($value);
      } // if
      
      if (!isset($this->range[$nid]['high']) || $value > $this->range[$nid]['high']) {
        $this->range[$nid]['high'] = $value;
      } // if
      
    } // foreach
    
    $this->range[$nid]['difference'] = $this->range[$nid]['high'] - $this->range[$nid]['low'];
    
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

    $datasets = array();
  
    foreach ($this->config['datasets'] as $dataset_nid) {
      $datasets[] = node_load($dataset_nid);
    } // foreach

    // load the first dataset
    $dataset = $datasets[0];

    // get the data for this dataset
    $features = $this->data($args);

    drupal_add_js(array('data_engine_visualize' => $this->legendSettings()), 'setting');

    // get the map and configure
    $map = leaflet_leaflet_map_info();
    $map['OSM Mapnik']['center'] = $this->getCenter($dataset);
    $map['OSM Mapnik']['settings']['zoom'] = $this->zoom;
    $map['OSM Mapnik']['settings']['scrollWheelZoom'] = FALSE;

    return leaflet_render_map($map['OSM Mapnik'], $features);

  }
}

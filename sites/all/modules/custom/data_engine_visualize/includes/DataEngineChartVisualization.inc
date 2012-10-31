<?php
class DataEngineChartVisualization extends DataEngineVisualization {
  public $data_colors = array(
    '#99CCCC',
    '#00737D',
    '#FF9933',
    '#CC3333',
    '#5E1A15',
    '#339999',
    '#009900',
    '#CCFFFF',
    '#993300',
    '#6699CC',
    '#FF6633',
    '#66CC66',
    '#003399',
    '#CCC',
    '#999',
    '#666',
    '#333',
  );

  public $column_labels = array();

  public function visualizationID($args = NULL) {
    return 'dataset-visualization-chart';
  }

  public function data($args = NULL) {
    // for now we only want the first dataset
    $dataset = array_shift($this->config['datasets']);
    $dataset = node_load($dataset);

    if ($dataset) {
      $nid = $dataset->nid;
      $this->fields = $this->getFieldNames($dataset);
      $column_key = strtolower($nid . '.' . data_engine_db_index_field_from_dataset($dataset));

      $count = data_engine_db_record_count_from_node($dataset);
      $data = $this->getData($dataset, 0, 3000);
      $graphData = array();
      $fieldData = array();
      $notFound = array();
      
      foreach ($data as $dataRow) {

        $this->column_labels[] = $this->getFieldLabel($column_key) . ': ' . $dataRow[$column_key];

        foreach ($this->config['attributes'][$dataset->nid] as $field => $useField){
          if (
            ($this->useField($field, $useField)) &&
            ($dataset->nid . '.' . $field != $column_key)
          ) {
            $rowKey = strtolower($nid . '.' . $field);
            if (!isset($fieldData[$rowKey])) {
              $fieldData[$rowKey] = array();
            }
            $dataValue = $this->getDataRowValue($dataRow, $rowKey);
            if ($dataValue !== FALSE) {
              $fieldData[$rowKey][] = array(count($fieldData[$rowKey]), intval($dataValue));
            }
            elseif (!isset($notFound[$rowKey])) {
              watchdog('data_engine_visualize', 'Could not find data for: ' . $rowKey .': '. implode(', ', array_keys($dataRow)));
              $notFound[$rowKey] = TRUE;
            }
          }
        }
      }

      $data_color_index = 0;

      foreach ($fieldData as $field => $data) {
        // create the label for this field
        $label = $this->getFieldLabel($field);
        // add the series data to the field
        $graphData[] = array(
          // Increment through $data_colors until you run out then start back at 0
          'color' => $this->data_colors[$data_color_index],
          'label' => $label,
          'data' => $data,
          'bars' => array('show' => TRUE),
          'hoverable' => TRUE,
        );

        $data_color_index++;

        if ($data_color_index == count($this->data_colors)) {
          $data_color_index = 0;
        } // if

      }
      return $graphData;
    }
    return NULL;
  }

  public function debugChart() {
    return theme('flot_graph',
      array(
        'element' => array('id' => 'dataset-visualization-chart'),
        'data' => array(
          array(
            'label' => 'label',
            'data' => array(array(0, 3), array(1, 4), array(2, 5)),
            'lines' => array('show' => TRUE),
          )
        ),
        'options' => array(
          // 'legend' => array('position' => 'nw'),
          'xaxis' => array('show' => FALSE),
        ),
      )
    );
  }

  public function render($args = NULL) {

    drupal_add_js(drupal_get_path('module', 'data_engine_visualize') . '/js/data_engine_visualize.chart.js');
    drupal_add_js(libraries_get_path('flot') . '/jquery.flot.stack.js', array('weight' => 1));

    $this->loadDatasets();

    if (!$this->hasDataConnection()) {
      // no data connection, so just debug a chart
      return $this->debugChart();
    }

    $graphData = $this->data($args);
    $middle = floor(count($graphData[0]['data']) / 2);

    return theme('flot_graph', array(
      'element' => array(
        'id' => 'dataset-visualization-chart',
        'style' => 'width:98%;height:340px',
      ),
      'data' => $graphData,
      'options' => array(
        'series' => array('stack' => 0, 'columnlabels' => $this->column_labels),
        'xaxis' => array('ticks' => array(array($middle, 'GEOID')), 'tickSize' => 0),
        'grid' => array('hoverable' => TRUE),
      ),
    ));
  }
}

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

    $datasets = array();

    foreach ($this->config['datasets'] as $dataset_nid) {
      $datasets[] = node_load($dataset_nid);
    } // foreach
  
    if (count($datasets) > 0) {

      if (count($datasets) == 1) {
  
        $count = data_engine_db_record_count_from_node($datasets[0]);
        $data = $this->getData($datasets[0], 0, 3000);
        $this->fields = $this->getFieldNames($datasets[0]);

      } // if
      else {
  
        $count = data_engine_db_record_count_from_nodes($datasets[0], $datasets[1]);
        $data = $this->getData($datasets, 0, 3000);
        $this->fields = $this->getFieldNames($datasets[0]) + $this->getFieldNames($datasets[1]);

      } // else
  
      $graphData = array();
      $fieldData = array();
      $notFound = array();
      
      $column_keys = array(
        strtolower($datasets[0]->nid . '.' . data_engine_db_index_field_from_dataset($datasets[0]))
      );

      $info = data_engine_db_info_for_node($datasets[0]);

      if (isset($info->metadata->display_name)) {

        $column_keys = array();
        
        $display_name = data_engine_db_table_id_to_name_in_field($info->metadata->display_name, $datasets[0]);
        $column_keys[] = $datasets[0]->nid . '.' . $display_name;

      } // if

      $column = 0;
      
      foreach ($data as $dataRow) {

        $column_label = array();

        foreach ($column_keys as $column_key) {
          $label = $this->getFieldLabel($column_key);
          if ($label == $column_key) {
            $column_label[] = $dataRow[$column_key];
          } // if      
          else {  
            $column_label[] = $label . ': ' . $dataRow[$column_key];
          } // else
        } // foreach
        
        $this->column_labels[] = implode(', ', $column_label);

        foreach ($datasets as $dataset) {

          $column_key = strtolower($dataset->nid . '.' . data_engine_db_index_field_from_dataset($dataset));

          foreach ($this->config['attributes'][$dataset->nid] as $field => $useField) {
  
            if (
              ($this->useField($field, $useField)) &&
              ($dataset->nid . '.' . $field != $column_key)
            ) {
    
              $rowKey = strtolower($datasets[0]->nid . '.' . $field);
              
              if (!isset($fieldData[$rowKey])) {
                $fieldData[$rowKey] = array();
              } // if
              
              $dataValue = $this->getDataRowValue($dataRow, $rowKey);
              
              if ($dataValue !== FALSE) {
                $fieldData[$rowKey][] = array($column, intval($dataValue));
                $column++;
              } // if
              elseif (!isset($notFound[$rowKey])) {
                watchdog('data_engine_visualize', 'Could not find data for: ' . $rowKey .': '. implode(', ', array_keys($dataRow)));
                $notFound[$rowKey] = TRUE;
              } // elseif
  
            } // if
  
          } // foreach

        } // forach

      } // foreach

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

      } // foreach

      return $graphData;

    } // if

    return NULL;

  } // data

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
    drupal_add_js(libraries_get_path('flot') . '/jquery.flot.tickrotor.js', array('weight' => 2));

    $this->loadDatasets();

    if (!$this->hasDataConnection()) {
      // no data connection, so just debug a chart
      return $this->debugChart();
    }

    $graphData = $this->data($args);
    $middle = floor(count($graphData[0]['data']) / 2);

    $first_dataset = node_load($this->config['datasets'][0]);
    $info = data_engine_db_info_for_node($first_dataset);

    $label = 'GEOID';

    if (isset($info->metadata->pkey)) {

      $fields = data_engine_db_fields_from_dataset($first_dataset);
      $id_to_name = data_engine_db_table_id_to_name($info);

      $label_parts = array();

      foreach ($info->metadata->pkey as $pkey) {

        list($table_id, $field_name) = explode('.', $pkey);
        $label_parts[] = $fields[$id_to_name[$table_id] . '.' . $field_name];

      } // foreach

      $label = implode(', ', $label_parts);

    } // if
    
    $ticks = array();
    
    foreach ($this->column_labels as $column_label) {

      $ticks[] = array((count($ticks) * count($graphData)) + 1, $column_label);
      
    } // foreach

    return theme('flot_graph', array(
      'element' => array(
        'id' => 'dataset-visualization-chart',
        'style' => 'width:98%;height:340px',
      ),
      'data' => $graphData,
      'options' => array(
        'series' => array('columnlabels' => $this->column_labels),
        'xaxis' => array('ticks' => $ticks, 'tickSize' => 0, 'rotateTicks' => 90),
        'grid' => array('hoverable' => TRUE),
      ),
    ));
  }
}

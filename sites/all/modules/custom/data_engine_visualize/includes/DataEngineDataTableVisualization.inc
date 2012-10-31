<?php
class DataEngineDataTableVisualization extends DataEngineVisualization {
  static public $numberPerPage = 50;
  public $fields = array();

  /**
   * When there is no data database connection, we just use this debug table
   * @return string
   */
  public function debugTable() {
    // build this table
    $datasets = array();
    $header = array();
    $rows = array();
    foreach ($this->config['attributes'] as $nid => $fields) {
      $datasets[$nid] = node_load($nid);
      foreach ($fields as $field => $useField) {
        if ($this->useField($field, $useField)) {
          $header[] = $this->formatHeader($field, $datasets[$nid]);
        }
      }
    }

    for ($i=0; $i < 3; $i++) {
      $row = array();
      foreach ($this->config['attributes'] as $nid => $fields) {
        foreach ($fields as $field => $useField) {
          if ($this->useField($field, $useField)) {
            $row[] = $this->formatCell($field);
          }
        }
      }
      $rows[] = $row;
    }

    $table = theme_table(array(
      'header' => $header,
      'rows' => $rows,
      'attributes' => array(),
      'colgroups' => array(),
      'sticky' => FALSE,
      'caption' => '',
      'empty' => ''
    ));
    return $table;
  }

  public function formatCell($cell) {
    return t($cell);
  }

  public function formatHeader($header, $dataset = NULL) {
    $header = $this->convertHeader($header);
    if (isset($this->fields[$header])) {
      return t($this->fields[$header]);
    }
    return t($header);
  }

  public function paginationNavigation($page, $start, $end, $count) {
    $numberOfPages = $this->paginationNumberOfPages($count);
    $links = array();
    if ($page > 1) {
      $links[] = l(t('First'), 'visualize/visualization', array('query' => array('config' => drupal_json_encode($this->config), 'page' => 1)));
      $links[] = l(t('Previous'), 'visualize/visualization', array('query' => array('config' => drupal_json_encode($this->config), 'page' => $page - 1)));
    }
    if ($page < $numberOfPages) {
      $links[] = l(t('Next'), 'visualize/visualization', array('query' => array('config' => drupal_json_encode($this->config), 'page' => $page + 1)));
      $links[] = l(t('Last'), 'visualize/visualization', array('query' => array('config' => drupal_json_encode($this->config), 'page' => $numberOfPages)));
    }
    if (count($links) == 0) {
      return '';
    }
    return '<div class="pagination pagination--embed">' . implode(' | ', $links) . '</div>';
  }

  public function paginationNumberOfPages($count) {
    return ceil($count / DataEngineDataTableVisualization::$numberPerPage);
  }

  public function render($args = NULL) {
    $this->loadDatasets();

    if (!$this->hasDataConnection()) {
      // no data connection, so just debug a table
      return $this->debugTable();
    }

    // if no arguments, use $_GET
    if ($args === NULL) {
      $args = $_GET;
    }

    $includePagination = TRUE;
    if (isset($args['pagination'])) {
      $includePagination = ($args['pagination'] == 1);
    }

    $header = array();
    $rows = array();
    $attributes = array('class' => explode(' ', 'visualization visualization-data-table'));
    $dataset = array_shift($this->config['datasets']);
    $datasets = array();
    $counts = array();

    $dataset = node_load($dataset);
    $nid = $dataset->nid;
    $count = data_engine_db_record_count_from_node($dataset);
    $this->fields = $this->getFieldNames($dataset);
    $attributes['class'][] = 'dataset-' . $dataset->nid;

    foreach ($this->config['attributes'][$dataset->nid] as $field => $useField) {
      if ($this->useField($field, $useField)) {
        $header[] = $this->formatHeader($field, $dataset);
      }
    }

    // foreach ($this->config['attributes'] as $nid => $fields) {
    //   // save the node to our array so we can use it later
    //   $datasets[$nid] = node_load($nid);

    //   // get the count for this dataset
    //   $counts[$nid] = data_engine_db_record_count_from_node($datasets[$nid]);

    //   // fields
    //   $this->fields[$nid] = data_engine_db_fields_from_dataset($datasets[$nid]);
    //   foreach ($this->fields[$nid] as $field => $label) {
    //     $this->fields[$nid][$this->convertHeader($field)] = $label;
    //   }

    //   // add the dataset as a class to the table
    //   $attributes['class'][] = 'dataset-' . $nid;

    //   // add the fields to the header
    //   foreach ($fields as $field => $useField) {
    //     if ($this->useField($field, $useField)) {
    //       $header[] = $this->formatHeader($field, $datasets[$nid]);
    //     }
    //   }
    // }
    // $count = intval(array_shift($counts));

    // pagination
    $numberOfPages = $this->paginationNumberOfPages($count);
    $page = 1;
    if (isset($args['page'])) {
      $page = intval($args['page']);
    }
    if ($page < 0) {
      $page = 1;
    }
    elseif ($page > $numberOfPages) {
      $page = $numberOfPages;
    }
    $start = (($page - 1) * DataEngineDataTableVisualization::$numberPerPage);
    $end = $start + DataEngineDataTableVisualization::$numberPerPage;

    // get the data for each dataset
    // $dataset = array_shift($datasets);
    $data = $this->getData($dataset, $start, $end);

    $fields = array_shift($this->config['attributes']);
    foreach ($data as $dataRow) {
      $row = array();
      foreach ($fields as $field => $useField) {
        if ($this->useField($field, $useField)) {
          $rowKey = strtolower($nid . '.' . $field);
          $dataValue = $this->getDataRowValue($dataRow, $rowKey);
          if ($dataValue !== FALSE) {
            $row[] = $this->formatCell($dataValue);
          }
          else {
            watchdog('data_engine_visualize', 'Could not find data for: ' . $rowKey);
          }
        }
      }
      $rows[] = $row;
    }

    $table = theme_table(array(
      'header' => $header,
      'rows' => $rows,
      'attributes' => $attributes,
      'colgroups' => array(),
      'caption' => '',
      'sticky' => FALSE,
      'empty' => t('No data to visualize')
    ));
    $output = $table;
    if ($includePagination) {
      $output .= $this->paginationNavigation($page, $start, $end, $count);
    }
    return $output;
  }
}

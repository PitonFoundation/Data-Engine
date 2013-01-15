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
    $header = $this->getFieldLabel($header);
    if (isset($this->fields[$header])) {
      return t($this->fields[$header]);
    }
    list($table, $header) = explode('.', $header);
    return t($header);
  }

  public function paginationNavigation($page, $start, $end, $count) {
    $numberOfPages = $this->paginationNumberOfPages($count);
    $links = array();
    if ($page > 1) {
      $links[] = l(t('First'), 'gallery/visualization', array('query' => array('config' => drupal_json_encode($this->config), 'page' => 1)));
      $links[] = l(t('Previous'), 'gallery/visualization', array('query' => array('config' => drupal_json_encode($this->config), 'page' => $page - 1)));
    }
    if ($page < $numberOfPages) {
      $links[] = l(t('Next'), 'gallery/visualization', array('query' => array('config' => drupal_json_encode($this->config), 'page' => $page + 1)));
      $links[] = l(t('Last'), 'gallery/visualization', array('query' => array('config' => drupal_json_encode($this->config), 'page' => $numberOfPages)));
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
    
    $datasets = array();

    foreach ($this->config['datasets'] as $dataset_nid) {
      $datasets[] = node_load($dataset_nid);
    } // foreach

    $attributes['class'][] = 'dataset-' . $datasets[0]->nid;
    
    if (count($datasets) == 1) {
  
      $count = data_engine_db_record_count_from_node($datasets[0]);
      $this->fields = $this->getFieldNames($datasets[0]);

    } // if
    else {

      $count = data_engine_db_record_count_from_nodes($datasets[0], $datasets[1]);
      $this->fields = $this->getFieldNames($datasets[0]) + $this->getFieldNames($datasets[1]);
      $attributes['class'][] = 'dataset-' . $datasets[1]->nid;

    } // else
        
    $fields = array();
    
    foreach ($datasets as $dataset) {

      $pkeys = data_engine_db_pkeys_for_node($dataset);

      foreach ($pkeys as $pkey) {
        $header[] = $this->formatHeader($pkey, $dataset);
        $fields[$pkey] = $pkey;
      } // forach

      foreach ($this->config['attributes'][$dataset->nid] as $field => $useField) {
        if ($this->useField($field, $useField)) {
          $header[] = $this->formatHeader($field, $dataset);
        }
        $fields[$field] = $useField;
      }
      
    } // foreach

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

    if (count($datasets) == 1) {
      $data = $this->getData($datasets[0], $start, $end);
    } // if
    else {
      $data = $this->getData($datasets, $start, $end);
    } // else

    foreach ($data as $dataRow) {
      $row = array();
      foreach ($fields as $field => $useField) {
        if ($this->useField($field, $useField)) {
          $rowKey = strtolower($datasets[0]->nid . '.' . $field);
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

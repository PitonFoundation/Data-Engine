<?php

/**
 * @file
 * Provides integration with the Views module.
 */

/**
 * Filter by view all unpublished permissions granted by view_unpublished
 * Takes over the Published or Admin filter query.
 */
class own_unpublished_handler_filter_node_status extends views_handler_filter_node_status {
  function query() {
    dpm('test');
    $table = $this->ensure_my_table();
    $where_per_type = array();
    foreach (node_type_get_types() as $type => $name) {
      $where_per_type[] = "($table.type = '$type' AND ***OWNUNPUBLISHED_VIEW_$type*** = 1)";
    }
    $where_per_type = implode(' OR ', $where_per_type);
    $this->query->add_where_expression($this->options['group'], "$table.status = 1 OR ($table.uid = ***CURRENT_USER*** AND ***CURRENT_USER*** <> 0 AND ***VIEW_OWN_UNPUBLISHED_NODES*** = 1) OR ***ADMINISTER_NODES*** = 1 OR $where_per_type");
  }
}

class own_unpublished_handler_filter_node_access extends views_handler_filter_node_access {
  function query() {
    dpm('own_unpublished_handler_filter_node_access');
    if (!user_access('administer nodes')) {
      $table = $this->ensure_my_table();
      $grants = db_or();
      dpm(node_access_grants());
      foreach (node_access_grants('view') as $realm => $gids) {
        foreach ($gids as $gid) {
          $grants->condition(db_and()
            ->condition($table . '.gid', $gid)
            ->condition($table . '.realm', $realm)
          );
        }
      }

      $this->query->add_where('AND', $grants);
      $this->query->add_where('AND', $table . '.grant_view', 1, '>=');
    }
  }
}
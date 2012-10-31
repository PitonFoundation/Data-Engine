<?php
require_once(dirname(dirname(__FILE__)) . '/data_engine_visualize.module');
require_once(dirname(dirname(__FILE__)) . '/data_engine_visualize.form.inc');

class TestSettingSyncing extends PHPUnit_Framework_TestCase {
  public function testSyncWithSession() {
    $_SESSION = array();
    _data_engine_visualize_sync_session(array('test' => 123));
    $this->assertTrue(isset($_SESSION['visualization_test']));
    $this->assertEquals($_SESSION['visualization_test'], 123);
  }
}

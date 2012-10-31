<?php
/**
 * These are not Drupal unittests
 */
require_once(dirname(dirname(__FILE__)) . '/includes/DataEngineVisualizationFactory.inc');

/**
 * Test DataEngineVisualizationTest
 */
class DataEngineVisualizationTest extends PHPUnit_Framework_TestCase {
  /**
   * Test the useField() method
   */
  public function testUseField() {
    $visualization = new DataEngineVisualization();
    $this->assertTrue($visualization->useField('field1', 'field1'));
    $this->assertFalse($visualization->useField('field1', NULL));
    $this->assertFalse($visualization->useField('field1', 'NULL'));
  }

  /**
   * Test the getDataRowValue() method
   */
  public function testGetDataRowValue() {
    $dataRow = array(
      '64.something_table_name.something' => 10,
      '64.something_table_name.something2' => 11,
      '64.something_table_name.something3' => 12,
    );

    $visualization = new DataEngineVisualization();
    $tests = array(
      '64.something_table_name.something',
      '64.something_table_namesomething',
      '64.something-table-namesomething',
      '64.something-table-name.something'
    );
    foreach ($tests as $test) {
      $this->assertEquals(
        $visualization->getDataRowValue($dataRow, $test), 10, "Could not find: $test");
    }
    // $this->assertEquals($visualization->getDataRowValue($dataRow, '64.something_table_name.something'), 10, "Could not find");
  }
}

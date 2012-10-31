<?php
/**
 * These are not Drupal unittests
 */
require_once(dirname(dirname(__FILE__)) . '/includes/DataEngineVisualizationFactory.inc');

// define some Drupal functions that are used in tested functions
if (!function_exists('drupal_json_encode')) {
  function drupal_json_encode($var) {
    return json_encode($var);
  }

  function l($text, $href, $options) {
    return $text;
  }

  function t($text) {
    return $text;
  }
}

class DataEngineDataTableTest extends PHPUnit_Framework_TestCase {
  public function setUp() {
    $this->visualization = DataEngineVisualizationFactory::create(array(
      'type' => 15,
      'typeName' => 'data-table',
      'datasets' => array(49),
      'attributes' => array(49 => array('field1' => 'field1'))
    ));
  }

  public function testCreate() {
    $this->assertEquals(get_class($this->visualization), 'DataEngineDataTableVisualization');
  }

  /**
   * Test our function for calculating number of pages
   */
  public function testPaginationNumberOfPages() {
    $this->assertEquals($this->visualization->paginationNumberOfPages(10), 1);
    $this->assertEquals($this->visualization->paginationNumberOfPages(50), 1);
    $this->assertEquals($this->visualization->paginationNumberOfPages(300), 6);
  }

  /**
   * Tests for pagination navigation
   */
  public function testPaginationNavigation() {
    // no navigation (less than one page of results)
    $this->assertEquals(
      $this->visualization->paginationNavigation(1, 0, 50, 10),
      '',
      'There should be no pagination navigation if there is less than one page of results'
    );

    // Add next/last
    $nav = $this->visualization->paginationNavigation(1, 0, 50, 60);
    $this->assertTrue(strpos($nav, 'Next') !== FALSE, 'The link to Next should be present');
    $this->assertTrue(strpos($nav, 'Last') !== FALSE, 'The link to Last should be present');
    $this->assertFalse(strpos($nav, 'First') !== FALSE, 'The link to First should NOT be present');
    $this->assertFalse(strpos($nav, 'Previous') !== FALSE, 'The link to Previous should NOT be present');

    // Add first/previous, next/last
    $nav = $this->visualization->paginationNavigation(2, 50, 100, 150);
    $this->assertTrue(strpos($nav, 'Next') !== FALSE, 'The link to Next should be present');
    $this->assertTrue(strpos($nav, 'Last') !== FALSE, 'The link to Last should be present');
    $this->assertTrue(strpos($nav, 'First') !== FALSE, 'The link to First should be present');
    $this->assertTrue(strpos($nav, 'Previous') !== FALSE, 'The link to Previous should be present');

    // Add first/previous, no next/last
    $nav = $this->visualization->paginationNavigation(3, 100, 150, 150);
    $this->assertFalse(strpos($nav, 'Next') !== FALSE, 'The link to Next should NOT be present');
    $this->assertFalse(strpos($nav, 'Last') !== FALSE, 'The link to Last should NOT be present');
    $this->assertTrue(strpos($nav, 'First') !== FALSE, 'The link to First should be present');
    $this->assertTrue(strpos($nav, 'Previous') !== FALSE, 'The link to Previous should be present');
  }
}

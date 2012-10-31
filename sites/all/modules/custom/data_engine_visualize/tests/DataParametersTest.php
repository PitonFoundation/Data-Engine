<?php
/**
 * These are not Drupal unittests
 */
require_once(dirname(dirname(__FILE__)) . '/data_engine_visualize.module');
require_once(dirname(dirname(__FILE__)) . '/data_engine_visualize.form.inc');

if (!function_exists('noad_load_multiple')) {
  function node_load_multiple($ids) {
  }

  function node_load($id) {
    $node = new stdClass();
    $node->nid = $id;
    $node->type = 'dataset';
    return $node;
  }
}

class TestDataParameters extends PHPUnit_Framework_TestCase {
  public function getFormState($datasets = array()) {
    $form_state = array('storage' => array('datasets' => $datasets));
    return $form_state;
  }

  public function testSingleGETParameter() {
    $formState = $this->getFormState();

    $checked = _data_engine_visualize_get_datasets(
      $formState,
      NULL,
      array('data' => '1')
    );

    $this->assertEquals(count($checked), 1);
    $this->assertEquals($checked[0], 1);
  }

  public function testMultipleGETParameters() {
    $formState = $this->getFormState();

    $checked = _data_engine_visualize_get_datasets(
      $formState,
      NULL,
      array('data' => '1 2')
    );

    $this->assertEquals(count($checked), 2);
    $this->assertEquals($checked[0], 1);
    $this->assertEquals($checked[1], 2);
  }

  public function testTooManyGETParameters() {
    $formState = $this->getFormState();

    $checked = _data_engine_visualize_get_datasets(
      $formState,
      NULL,
      array('data' => '1 2 3')
    );

    $this->assertEquals(count($checked), 2);
    $this->assertEquals($checked[0], 1);
    $this->assertEquals($checked[1], 2);
  }

  public function testSameSingleGETAndFormStateParameters() {
    $formState = $this->getFormState(array(1));

    $checked = _data_engine_visualize_get_datasets(
      $formState,
      NULL,
      array('data' => '1')
    );

    $this->assertEquals(count($checked), 1, 'There should only be one item. Found: ' . count($checked));
    $this->assertEquals($checked[0], 1, 'The first dataset should equal 1');
  }

  /**
   * Append a new parameter via get
   */
  public function testDifferentSingleGETAndFormStateParameters() {
    $formState = $this->getFormState(array(1));

    $checked = _data_engine_visualize_get_datasets(
      $formState,
      NULL,
      array('data' => '2')
    );

    $this->assertEquals(count($checked), 2, 'There should only be 2 items. Found: ' . count($checked));
    $this->assertTrue(in_array(1, $checked));
    $this->assertTrue(in_array(2, $checked));
  }

  public function testTooManyFormStateGETParameters() {
    $formState = $this->getFormState(array(4));

    $checked = _data_engine_visualize_get_datasets(
      $formState,
      NULL,
      array('data' => '1 2 3')
    );

    $this->assertEquals(count($checked), 2);
    $this->assertEquals($checked[0], 4);
    $this->assertEquals($checked[1], 1);
  }

  public function testFormStateAndSessionTheSame() {
    $formState = $this->getFormState(array(1));

    $checked = _data_engine_visualize_get_datasets(
      $formState,
      array('visualization_datasets' => array(1)),
      NULL
    );

    $this->assertEquals(count($checked), 1, 'There should only be 1 item. Found: ' . count($checked));
    $this->assertTrue(in_array(1, $checked));
  }

  public function testFormStateAndSessionTheSameAndAppendGETParam() {
    $formState = $this->getFormState(array(1));

    $checked = _data_engine_visualize_get_datasets(
      $formState,
      array('visualization_datasets' => array(1)),
      array('data' => '2')
    );

    $this->assertEquals(count($checked), 2, 'There should only be 2 items. Found: ' . count($checked));
    $this->assertTrue(in_array(1, $checked));
    $this->assertTrue(in_array(2, $checked));
  }
}
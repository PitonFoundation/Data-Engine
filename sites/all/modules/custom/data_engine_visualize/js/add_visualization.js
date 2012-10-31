/**
 * Add visualization JS
 */
(function($) {

  /**
   * Bind the leaflet popup to our layer
   */
  function bindPopup(feature, layer) {
    // skip if there are no popups defined
    if (typeof feature.properties == 'undefined' || (typeof feature.properties != 'undefined' && typeof feature.properties.popup == 'undefined')) {
      return;
    }

    // bind the popup
    layer.bindPopup(feature.properties.popup);
  } // bindPopup

  //===================================================================
  // DOCUMENT.AJAXCOMPLETE
  //===================================================================
  $(document).ajaxComplete(function(e, xhr, settings) {
    // check all the checkboxes after an AJAX request
    $('.form-item-datasets :checkbox').attr('checked', true);
  });

  //===================================================================
  // DOCUMENT.READY
  //===================================================================
  $(document).ready(function() {

    //-------------------------------------
    // STEP 1
    //-------------------------------------
    // attach JS behavior the add datasets link
    $('.add-dataset a').click(function(ev) {
      ev.preventDefault();
      // show the hidden field to add more data
      $('.form-item-add-more-datasets, #edit-add-more-button').show();
    });

    $('.form-item-add-more-datasets, #edit-add-more-button').hide();

    //-------------------------------------
    // STEP 2
    //-------------------------------------

    // add classes for visualization types
    var $visualizationTypes = $('#edit-visualizations :radio');
    $visualizationTypes.each(function() {
      var $this = $(this),
        $field = $this.closest('.form-item');

      var typeHref = $('h2 a', $field).attr('href');
      var hrefPieces = typeHref.split('/');

      $field.addClass('visualization-type-' + hrefPieces[(hrefPieces.length - 1)])
        .addClass('visualization-type');
    });

    // if there are multiple datasets, disable maps
    if (typeof Drupal.settings != 'undefined' && typeof Drupal.settings.data_engine_visualize != 'undefined') {
/*
      if (Drupal.settings.data_engine_visualize.number_of_datasets > 1) {
        $('.visualization-type-map').addClass('disabled')
          .find(':radio').attr('disabled', true);
      }
*/
    }

    //-------------------------------------
    // STEP 3
    //-------------------------------------

    // add handler for the update visualization button
    $('#edit-update-attributes').click(function(ev) {

      ev.preventDefault();

      // update the configuration
      var config = Drupal.settings.data_engine_visualize.config;
      config.attributes = {};

      $('.dataset-attributes fieldset fieldset').each(function() {

        var $datasetFieldset = $(this);
        var dataset = /^edit-(\d+)$/.exec($datasetFieldset.attr('id'))[1];
        
        if (typeof config.attributes[dataset] == 'undefined') {
          config.attributes[dataset] = {};
        } // if

        $datasetFieldset.find(':checkbox').each(function(){

          var $attributeCheckbox = $(this);
          var attribute = $attributeCheckbox.attr('name').split(Drupal.settings.data_engine_visualize.placeholder).join('.');

          if ($attributeCheckbox.attr('checked')) {
            config.attributes[dataset][attribute] = attribute;
          } // if
          else {
            config.attributes[dataset][attribute] = null;
          } // else

        });

      });

      var can_log = false; // typeof console != 'undefined' && typeof console.log != 'undefined';
      if (can_log) {
        console.log('get new visualization');
      }
      Drupal.settings.data_engine_visualize.config = config;
      $.getJSON('/visualize/visualization', {'config': config, 'pagination': 0}, function(data) {
        if (can_log) { console.log('got new visualization'); }
        if (typeof data.html != 'undefined') {
          $('#visualization-preview').empty().append(data.html);
        }
        else {
          if (can_log) { console.log('data: ', data.data); }
          if (data.type == 'chart') {
            Drupal.flot['dataset_visualization_chart']['flot'].setData(data.data);
            Drupal.flot['dataset_visualization_chart']['flot'].setupGrid();
            Drupal.flot['dataset_visualization_chart']['flot'].draw();
          }
          else {
            // remove the existing layer
            var map = Drupal.settings.leaflet[0].lMap;
            for (var layer in map._layers) {
              if (layer != 'earth' && typeof map._layers[layer].feature != 'undefined') {
                map.removeLayer(map._layers[layer]);
              }
            }

            for (var feature in data.data) {
              var newLayer = new L.GeoJSON(data.data[feature].json, {
                style: function(feature) {
                  return feature.properties.style;
                },
                onEachFeature: bindPopup
              });
              map.addLayer(newLayer);
            }
          }
        }
      });
    });
  });

})(jQuery);

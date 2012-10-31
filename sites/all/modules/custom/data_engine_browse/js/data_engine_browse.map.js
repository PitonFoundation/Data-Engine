(function ($) {

  Drupal.behaviors.dataEngineBrowse = {
  
    attach:function (context, settings) {

      $('html').bind('leafletmapload', function(event, map){

        dataEngineUpdateBounds(map);
        map.on('zoomend', function(event) { dataEngineUpdateBounds(event.target); });
        map.on('moveend', function(event) { dataEngineUpdateBounds(event.target); });
        $('#map-active').click(function(){ dataEngineUpdateBounds(map); });

      });

    } // attach

  }; // Drupal.behaviors.dataEngineBrowse

  function dataEngineUpdateBounds(map) {

    var bounds = map.getBounds();    

    if ($('#map-active').is(':checked')) {
      $('input[name=map-bounds]').val(bounds.toBBoxString());
      $('.data-engine-browse-filters').trigger('browseFilters-update', ['map']);
    } // if
    else {
      $('input[name=map-bounds]').val('');
    } // else

  } // dataEngineUpdateBounds

})(jQuery);

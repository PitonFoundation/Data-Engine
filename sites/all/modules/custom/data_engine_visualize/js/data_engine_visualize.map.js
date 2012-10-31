(function ($) {

  Drupal.behaviors.dataEngineBrowse = {
  
    attach:function (context, settings) {

      $('html').bind('leafletmapload', function(event, map){

        if (Drupal.settings.data_engine_visualize.maplegend) {
          
          var legend = L.control({position: 'bottomright'});
          
          legend.onAdd = function(map) {
            
            var div = $('<div style="background: #fff; padding: 1em;">' + Drupal.settings.data_engine_visualize.maplegend + '</div>');
      			return div[0];
            
          } // onAdd

          legend.addTo(map);
          
        } // if

      });

    } // attach

  }; // Drupal.behaviors.dataEngineBrowse


})(jQuery);

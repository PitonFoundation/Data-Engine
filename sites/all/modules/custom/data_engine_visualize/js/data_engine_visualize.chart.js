(function ($) {

  Drupal.behaviors.dataEngineVisualizeChart = {
  
    attach:function (context, settings) {

      $('#dataset-visualization-chart').bind("plothover", function (event, pos, item) {

        $("#dataset-visualization-chart-tooltip").remove();

        if (item) {

          $('<div id="dataset-visualization-chart-tooltip"><p>' + item.series.columnlabels[item.dataIndex] + '</p><p>' + item.series.label + ': ' + (item.datapoint[1] - item.datapoint[2])+  '</p></div>').css({
              position: 'absolute',
              // display: 'none',
              top: pos.pageY + 5,
              left: pos.pageX + 5,
              'z-index': 10,
              border: '1px solid #fdd',
              padding: '2px',
              'background-color': '#fee',
              opacity: 0.80
          }).appendTo('body').fadeIn(200);

        } // if

      });

    } // attach

  }; // Drupal.behaviors.dataEngineVisualizeChart

})(jQuery);

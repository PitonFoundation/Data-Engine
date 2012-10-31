/* ------------------------------------------------------------------
  document.ready
---------------------------------------------------------------------*/
(function ($) {
  $(document).ready(function(){

    // run browseFilters on the find page filters
    $('.data-engine-browse-filters').browseFilters();

    // call symbolset after filters run
    $('.data-engine-browse-filters').bind('browseFilters-updated', function() {
      if (typeof ss_legacy === 'function') {
        if (document.getElementsByClassName) {
          ss_legacy(document.getElementsByClassName('ss-icon'));
        } else {
          ss_legacy(ss_getElementsByClassName(document.body, 'ss-icon'));
        }
      }
    });

    // Contact form - select the correct reason for contacting
    if ($('#webform-client-form-20').length > 0) {

      var hash = window.location.hash;
      hash = hash.replace(/^.*#/, '');

      // Show previously requested datasets if option index is 3
      if (hash == 3) {
        $('#block-views-requested-datasets-block').show();
      }
      else {
        // Hide Requested Datasets in sidebar
      $('#block-views-requested-datasets-block').hide();
      }

      $('#edit-submitted-reason-for-contacting option').eq(hash).attr('selected', 'selected');

      $('#edit-submitted-reason-for-contacting').change(function(){

        // Show previously requested datasets if option index is 3
        if ($(this).find('option:selected').index() == 3) {
          $('#block-views-requested-datasets-block').show();
        }
        else {
          $('#block-views-requested-datasets-block').hide();
        }
      });

    } //if

  }); // END document.ready
})(jQuery); //$

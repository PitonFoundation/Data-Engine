// Adds fancybox functionality to preview buttons.
(function ($) {
  $(document).ready(function(){
    $('.button--preview-data, .button--preview-map')
      .attr('data-fancybox-type', 'iframe')
      .each(function() {
        var jsHref = this.href + '?js=1';
        $(this).attr('href', jsHref);
      });
    $('.button--preview-data').fancybox({
      width: '100%'
    });
    $('.button--preview-map').fancybox({
      maxWidth: '1024px',
      maxHeight: '420px'
    });
  });
})(jQuery);

/**
 * browseFilters Plugin
 *
 * Show/hide/check the checkboxes for the browse filters.
 *
 * Events:
 *
 *    update
 */
(function($) {

  // IE9 can't do console.log.apply, but this fixes it.

  if (Function.prototype.bind && console && typeof console.log == "object") {
    [
      "log","info","warn","error","assert","dir","clear","profile","profileEnd"
    ].forEach(function (method) {
        console[method] = this.bind(console[method], console);
    }, Function.prototype.call);
  }

  var filtersSelector = '.data-engine-browse-filters';

  // these are events that are used by this plugin. they are mapped
  // from potentially common names (e.g., update) to plugin specific
  // event names
  var events = {
    update: 'browseFilters-update',
    updateUI: 'browseFilters-updateUI',
    updating: 'browseFilters-updating',
    updated: 'browseFilters-updated'
  };

  /*===================================================================
    METHODS
   ==================================================================*/
  var methods = {
    click: function(ev) {
      ev.stopPropagation();

      var $this = $(this),
        $parent = $this.closest('li');

      if ($this.attr('checked')) {
        // show the children
        $parent.find('.children').slideDown('fast');
        $this.closest(filtersSelector).trigger(events.update);
        $this.closest(filtersSelector).trigger(events.updateUI);
      }
      else {
        $parent.find('.children').slideUp('fast');
        $this.closest(filtersSelector).trigger(events.update);
      }
    },

    /**
     * Initialize the browse filter behaviors
     *
     * Options: none
     */
    init: function(options) {
      // default settings
      var settings = $.extend({
        debug: false,
        updateDelay: 500,
        mapUpdateDelay: 1000
      }, options);

      return this.each(function() {
        var $this = $(this);

        // set a data timer holder
        $this.data('browseFiltersTimer', null);
        $this.data('browseFiltersOptions', settings);

        // hide all the childrens
        $('ul.children', this).hide();

        //-------------------------------------------------------------
        // EVENT HANDLERS
        //-------------------------------------------------------------
        // update handler
        $this.bind(events.update, function(ev, updateType) {
          $(this).browseFilters('update', updateType);
        });

        // update UI handler
        $this.bind(events.updateUI, function() {
          // if (typeof $.uniform != 'undefined') {
          //   $.uniform.update(':checkbox');
          // }
        });

        // click handler for the checkboxes
        $('.term :checkbox', this).click(methods.click);

        // search box handler
        $('input[name="search"]').keypress(function() {
          $(this).closest(filtersSelector).trigger(events.update);
        });

      });
    },

    log: function () {
      var $this = $(filtersSelector),
        options = $this.data('browseFiltersOptions');

      if (options.debug && typeof console != 'undefined' && typeof console.log != 'undefined') {
        console.log.apply(console, arguments);
      }
    },

    /**
     * The user has typed or checked a filter
     *
     * Note that this does not actually fire off an AJAX request, see updateView.
     */
    update: function(updateType) {
      var $this = $(this),
        options = $this.data('browseFiltersOptions');

      methods.log('update');

      if (!updateType) {
        updateType = 'default';
      }

      // hide children of unchecked items
      $(':checkbox:not(:checked)').each(function() {
        var $parent = $(this).closest('li');
        $('.children', $parent).slideUp('fast');
        $(':checkbox', $parent).attr('checked', false);
      });

      // set a timer
      if ($this.data('browseFiltersTimer')) {
        window.clearTimeout($this.data('browseFiltersTimer'));
      }

      // we set a timer so that every change does not potentially send a request
      var delay = options.updateDelay;
      if (updateType == 'map') {
        delay = options.mapUpdateDelay;
      }
      methods.log(delay);
      $this.data('browseFiltersTimer', window.setTimeout(function() {
        jQuery(this).browseFilters("updateView");
      }, delay));

      $this.closest(filtersSelector).trigger(events.updateUI);
    },

    /**
     * Actually send the AJAX request
     */
    updateView: function() {
      methods.log('updateView');
      var data = $(filtersSelector + ' :input').serializeArray();
      $(filtersSelector).trigger(events.updating);
      
      $('<div class="loading"><img src="/sites/all/modules/contrib/views/images/loading-small.gif" alt="Loading..." /></div>').insertBefore('.view .view-content .article-list, .view .view-empty');
      $.ajax({
        url: window.location.href,
        dataType: 'json',
        data: data,
        success: function(data) {
          $('.view').replaceWith(data.html);
          $(filtersSelector).trigger(events.updated);
          Drupal.behaviors.ViewsAjaxView.attach();
        },
        error: function(jqXHR, textStatus, errorThrown) {
/*
          if ($('#messages').length === 0) {
            $('<div id="messages"></div>').insertBefore('#main-content');
          }
          $('#messages').append('<div class="messages error"><h2 class="element-invisible">Error message</h2>' +
            Drupal.t('There was an error trying to load these results') +
            '</div>');
          $(filtersSelector).trigger(events.updated);
*/
        }
      });
    }
  };

  /*===================================================================
    PLUGIN
   ==================================================================*/
  $.fn.browseFilters = function( method ) {

    // Method calling logic
    if ( methods[method] ) {
      return methods[ method ].apply( this, Array.prototype.slice.call( arguments, 1 ));
    }
    else if ( typeof method === 'object' || ! method ) {
      return methods.init.apply( this, arguments );
    }
    else {
      $.error( 'Method ' +  method + ' does not exist on jQuery.browseFilters' );
    }

  };

})(jQuery); // end browseFilters

(function($) {
  Drupal.behaviors.modallogin = {
    attach: function(context) {
      var hash = window.location.hash
        , settings = Drupal.settings && Drupal.settings.modallogin
        , auto_open = settings && settings.open || []
        , $reveal;

      if (!$.fn.foundation || this.processed) {
        return;
      }
      this.processed = true;

      for (var id in auto_open) if (auto_open.hasOwnProperty(id)) {
        var options = auto_open[id];

        console.debug('modallogin: auto open modal set in code: %s', id);
        $('#' + id).foundation('reveal', 'open');
      }

      // Automatically open a "linked" modal.
      if (hash.indexOf('reveal_') === 1) {
        // transform #reveal_modallogin-login to #modallogin-login.reveal-modal
        $reveal = $('#' + hash.split('reveal_')[1] + '.reveal-modal');
        if ($reveal.length) {
          console.debug('modallogin: auto open linked modal: %s', hash);
          $reveal.foundation('reveal', 'open');
        }
      }

      // Update the hash when a modal is opened so we can link to them.
      $(document).on('opened.fndtn.reveal', '[data-reveal]', function () {
        console.debug('modallogin: set hash to %s', this.id);
        window.location.hash = 'reveal_' + this.id;
      });
      // Update the hash when a modal is closed so it doesnt reappear on refresh.
      $(document).on('closed.fndtn.reveal', '[data-reveal]', function () {
        console.debug('modallogin: remove hash of %s', this.id);
        window.location.hash = '';
      });

      // Refresh the form id.
      $(document).one('opened.fndtn.reveal', '[data-reveal]', Drupal.modallogin.refreshForms);
    }
  };

  Drupal.modallogin = Drupal.modallogin || {};

  Drupal.modallogin.refreshForms = function () {
    var refreshDefer = $.Deferred();
    // Trigger once only.
    $(document).off('opened.fndtn.reveal', '[data-reveal]', Drupal.modallogin.refreshForms);
    Drupal.modallogin.storeSourceUrlCookie();

    var forms = []
      , form_ids = [];

    $('.modallogin-modal form')
      // Refresh all the modallogin forms.
      .each(function() {
        var $this = $(this)
          , build_id = $this.find('[name="form_build_id"]').val()
          , form_id = $this.find('[name="form_id"]').val();

        forms.push({ form_id: form_id, build_id: build_id });
        form_ids.push(form_id);
      })
      .on('submit', function(e) {
        var form = this;
        e.preventDefault();

        if (refreshDefer.state() === 'pending') {
          console.debug('modallogin: refreshing form %s, delaying submit', form.id);
        }

        // Do not submit until the forms have been refreshed.
        refreshDefer.always(function() {
          form.submit();
        });
      });

    console.debug('modallogin: refresh forms: %s', form_ids.join(', '));

    $.post(Drupal.settings.basePath + 'modallogin/refresh_form', { forms: forms })
      .done(function(data) {
        for (var i = 0, l = data.length; i < l; i++) {
          var form = data[i];
          Drupal.modallogin.injectBuildId(form);
        }
        console.debug('modallogin: resolving form refresh');
      })
      .always(function() {
        refreshDefer.resolve();
      });
  };

  Drupal.modallogin.injectBuildId = function (form) {
    var $input = $('#' + form.id).find('[name="form_build_id"]');
    if ($input.val() !== form.build_id) {
      $input.val(form.build_id);
      console.debug('modallogin: refreshed form: %s', form.id);
    }
  };

  Drupal.modallogin.storeSourceUrlCookie = function() {
    var pathStart = Drupal.settings.basePath.length + Drupal.settings.pathPrefix.length
      , path = location.pathname.slice(pathStart);

    if (!path.length) path = '<front>';

    $.cookie('modallogin_source', path, { path: '/' });
  };
}(jQuery));

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
        console.debug('modallogin: remove hash to %s', this.id);
        window.location.hash = '';
      });
    }
  };
}(jQuery));

Modal login
===========

### TODO

- Fix redirects so the forms keep track of destination and the page it started
  on.
  `Drupal.visitor.current_uri` should be set (slightly used in
  `modallogin.connector.inc`).
- Fix form validation on modal forms (eg. user already exists, meaning the ones
  clientsidevalidation cannot detect).

### Usage

#### To automatically open a modal when a page loads

```js
Drupal.settings.modallogin.open.MODAL_ID = {};
```

```php
$form['#attached']['js'][] = array('data' => array(
  'modallogin' => array(
    'open' => array('modallogin-account' => array()),
  ),
), 'type' => 'setting');
```

#### Link to a modal

When a modal is opened a `#reveal_MODAL_ID` fragment is added to the url, this
can simply be copy pasted into an email or wherever.

#### Create a user from an email / link

1. Enable `modallogin_linkcreate`.
2. Add a link to `/signup/link/<TIMESTAMP>?mail=<MAIL>` eg.
  `/signup/link/1418827240?mail=m@oxy.fi`.
3. This creates the user with a machine generated name and password which the
   user is forced to set when the page loads.

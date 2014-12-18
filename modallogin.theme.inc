<?php

/**
 * Theme a reveal modal.
 */
function theme_modallogin_reveal($variables) {
  $id = $variables['id'];
  $content = $variables['content'];
  $close_button = $variables['close_button'];

  $variables['attributes'] += array(
    'class' => array(),
    // Required for foundation reveal.
    'data-reveal' => '',
  );
  // Set the ID, do not use drupal_html_id as links will be pointing to this.
  $variables['attributes']['id'] = $id;
  $variables['attributes']['class'] += array('reveal-modal', 'modallogin-modal');
  // Default to small if no other sizing clas has been given.
  if (!array_intersect($variables['attributes']['class'], array('tiny', 'small', 'medium', 'large', 'xlarge', 'full'))) {
    $variables['attributes']['class'][] = 'small';
  }
  // Append the close button.
  if ($close_button) {
    $content .= '<a class="close-reveal-modal">&#215;</a>';
  }
  return '<div ' . drupal_attributes($variables['attributes']) . '>' . $content . '</div>';
}

/**
 * Returns the HTML for a link triggering a reveal modal.
 *
 * Same parameters as theme_link with an additional $target_id that states 
 * which reveal modal to display. Must be specified as the HTML id.
 * Eg. modallogin-login
 */
function theme_modallogin_link($variables) {
  $variables['options']['attributes']['data-reveal-id'] = $variables['target_id'];
  return '<a href="' . check_plain(url($variables['path'], $variables['options'])) . '"' . drupal_attributes($variables['options']['attributes']) . '>' . ($variables['options']['html'] ? $variables['text'] : check_plain($variables['text'])) . '</a>';
}

/**
 * Returns all the form modals (displayed but hidden for each signed out user).
 */
function theme_modallogin($variables) {
  $build['modallogin-login'] = array(
    '#theme' => 'modallogin_login',
  );
  $build['modallogin-register'] = array(
    '#theme' => 'modallogin_register',
  );
  $build['modallogin-reset-password'] = array(
    '#theme' => 'modallogin_reset_password',
  );
  $content = drupal_render($build);
  return $content;
}

/**
 * The login modal.
 *
 * @see user_login().
 */
function theme_modallogin_login($variables) {
  module_load_include('inc', 'user', 'user.pages');
  $form = drupal_get_form('user_login');
  // Dont use the template faile.
  unset($form['#theme']);
  // Make the form cachable @TODO
  unset($form['#token']);

  $form['links'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => 'links'),
    '#weight' => 1000,
  );

  if (variable_get('user_register')) {
    $form['links']['register'] = array(
      '#theme' => 'modallogin_link',
      '#target_id' => 'modallogin-register',
      '#path' => 'user/register',
      '#text' => t('Sign up'),
    );
  }
  $form['links']['reset'] = array(
    '#theme' => 'modallogin_link',
    '#target_id' => 'modallogin-reset',
    '#path' => 'user/password',
    '#text' => t('Forgot your password?'),
  );

  // The OAuth connector buttons.
  $block = connector_block_view('one_click_block');
  $content = '<div class="connector-block">' . drupal_render($block) . '</div>';
  $content .= drupal_render($form);

  return theme('modallogin_reveal', array(
    'id' => 'modallogin-login',
    'content' => $content,
  ));
}

/**
 * The registration modal.
 *
 * @see user_register_form().
 *
 * @TODO hide username and password
 */
function theme_modallogin_register($variables) {
  $form = drupal_get_form('user_register_form');
  unset($form['#theme']);
  // Make the form cachable @TODO
  unset($form['#token']);
  // @TODO use custom submit and validation handlers same as 
  // modallogin_linkcreate so that we can create users without names and 
  // passwords.
  //
  // $form['picture']['#access'] = FALSE;
  // $form['signature_settings']['#access'] = FALSE;
  // $form['account']['name']['#access'] = FALSE;
  // $form['account']['pass']['#access'] = FALSE;
  // $form['#submit'] = array('modallogin_user_register_submit');

  $form['links'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => 'links'),
    '#weight' => 1000,
  );

  $form['links']['login'] = array(
    '#theme' => 'modallogin_link',
    '#target_id' => 'modallogin-login',
    '#path' => 'user/login',
    '#text' => t('Log in'),
  );
  $form['links']['reset'] = array(
    '#theme' => 'modallogin_link',
    '#target_id' => 'modallogin-reset',
    '#path' => 'user/password',
    '#text' => t('Forgot your password?'),
  );

  $block = connector_block_view('one_click_block');
  $content = '<div class="connector-block">' . drupal_render($block) . '</div>';
  $content .= drupal_render($form);

  return theme('modallogin_reveal', array(
    'id' => 'modallogin-register',
    'content' => $content,
  ));
}

/**
 * Reset password modal.
 * @see user_pass().
 */
function theme_modallogin_reset_password($variables) {
  $form = drupal_get_form('user_pass');
  unset($form['#theme']);
  // Make the form cachable @TODO
  unset($form['#token']);

  $form['links'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => 'links'),
    '#weight' => 1000,
  );

  if (variable_get('user_register')) {
    $form['links']['register'] = array(
      '#theme' => 'modallogin_link',
      '#target_id' => 'modallogin-register',
      '#path' => 'user/register',
      '#text' => t('Sign up'),
    );
  }
  $form['links']['login'] = array(
    '#theme' => 'modallogin_link',
    '#target_id' => 'modallogin-login',
    '#path' => 'user/login',
    '#text' => t('Log in'),
  );

  $content = drupal_render($form);

  return theme('modallogin_reveal', array(
    'id' => 'modallogin-reset',
    'content' => $content,
  ));
}

/**
 * Edit account modal. Used as a method to not to require users to set 
 * usernam/password while registering, but force them to set it as soon as they 
 * have an account (the next page load).
 *
 * @see modallogin_user_account_form().
 */
function theme_modallogin_account($variables) {
  $form = drupal_get_form('modallogin_user_account_form');
  unset($form['#theme']);

  // Automatically open the modal.
  $form['#attached']['js'][] = array('data' => array(
    'modallogin' => array(
      'open' => array('modallogin-account' => array()),
    ),
  ), 'type' => 'setting');

  $form['#attached']['css'][] = drupal_get_path('module', 'user') . '/user.css';
  $content = drupal_render($form);

  return theme('modallogin_reveal', array(
    'id' => 'modallogin-account',
    'content' => $content,
    // Hide the close button
    'close_button' => FALSE,
    'attributes' => array(
      // Disable close (esc still works).
      'data-options' => 'close_on_background_click:false',
    ),
  ));
}

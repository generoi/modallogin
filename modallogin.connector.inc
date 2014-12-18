<?php

/**
 * Implements hook_connector_action_alter().
 *
 * We change the default connector to redirect to our own action.
 */
function modallogin_connector_action_alter(&$connector_actions) {
  $connector_actions['default']['login callback'] = '_modallogin_connector_log_in';
}

/**
 * Redirect to register if we can may add a user.
 * @see _connector_log_in().
 */
function _modallogin_connector_log_in($connector_name, $cid = NULL, $consumer = NULL, $access_token = NULL, $request_token = NULL) {
  global $user;

  if (user_is_logged_in()) {
    return TRUE;
  }

  $connector = _connector_get_connectors($connector_name);
  if (!$connector) {
    return FALSE;
  }

  //Fetch connector ID
  if ($cid === NULL && isset($connector['id callback']) && is_callable($connector['id callback'])) {
    $cid = call_user_func($connector['id callback'], $connector);
  }

  if ($cid === NULL) {
    return FALSE;
  }
  $authname = $connector_name . '__' . $cid;
  $account = user_external_load($authname);
  if (!$account) {
    // Return NULL and not FALSE so that we know we didn't find a user.
    return NULL;
  }

  if (!$account) {
    return FALSE;
  }

  // @TODO hack! Logintoboggan doesnt allow use to both require validation 
  // email AND immediate login, this settings should be set in our own module. 
  // For now simply leave the logic intact and force immediate login here.
  global $conf;
  $conf['logintoboggan_immediate_login_on_register'] = TRUE;

  $pre_auth = _modallogin_pre_auth();
  $is_first_login = !$account->login;

  // User does not need administrator approval.
  if ($account->status) {
    // Email verification is required
    if ($pre_auth && $is_first_login) {
      // User can log in immediately but needs to verify email.
      // Requires LoginToBoggan
      if (variable_get('logintoboggan_immediate_login_on_register', TRUE)) {
        $action = 'create_login_verify';
      }
      // User still needs to verify the email address before logging in.
      else {
        $action = 'create_verify';
      }
    }
    // User doesnt require email verification simply login.
    // @TODO what about !$pre_auth && $is_first_login?
    else {
      $action = 'login';
    }
  }
  // User requires administrator approval
  else {
    $action = 'create_approval';
  }

  _modallogin_create_login_action($action, $account, $connector);

  return FALSE;
}

/**
 * Different login actions to take upon user creation / login.
 */
function _modallogin_create_login_action($action, $account, $connector) {
  // Redirect user to the page they were on or Drupal specified they were going to.
  // @TODO this is never set.
  $destination = $_COOKIE['Drupal_visitor_current_uri'];
  if (!drupal_valid_path($destination)) {
    $destination = '<front>';
  }
  $destination_query = array();

  // Trigger events which send emails among other things.
  rules_invoke_event("modallogin_$action", $account);
  $next_action = 'noop';
  $pre_auth = _modallogin_pre_auth();

  // Assign the pre auth role.
  switch ($action) {
    case 'create_login_verify':
    case 'create_verify':
    case 'create_approve':
      // Add logintoboggans pre auth role
      if ($pre_auth) {
        $validating_role = logintoboggan_validating_id();
        $account->roles[$validating_role] = 1;
        _logintoboggan_user_roles_alter($account);
      }
      // Mark this user as in need of pass.
      $account->data['modallogin_no_pass'] = TRUE;
      $account = user_save($account);
      break;
    case 'login':
      break;
  }

  switch ($action) {
    // User was created, and is now logged in and emailed a verification email.
    case 'create_login_verify':
      // Log in user
      global $user;
      $user = $account;
      user_login_finalize();
      // Let the user's password be changed without the current password check.
      $token = drupal_random_key();
      $_SESSION['pass_reset_' . $user->uid] = $token;

      // Password will be prompted via modallogin_init().
      // We wait with sending email until it has been set.
      $destination_query = array('pass-reset-token' => $token);
      // No message required as it's set in modallogin_init().

      $next_action = 'goto';
      break;

    // User was created but need to verify its email before logged in.
    case 'create_verify':
      $message = t('Your account is currently pending e-mail verification. You have receveid a email with further instructions. !link to start a new e-mail verification.', array('!link' => l('Request a new password', 'user/password')));
      $next_action = 'logout';
      break;

    // User was NOT necessarily created and is now logged in without requiring 
    // email verification.
    // @TODO this runs when user logs in with an existing account, does it also run in other situations?
    case 'login':
      // drupal_set_message(t('Further instructions have been sent to your e-mail address.'));
      //Log in user
      $form_state['uid'] = $account->uid;
      // Redirect user to the page they were on or Drupal specified they were going to.
      user_login_submit(array(), $form_state);
      $next_action = 'goto';
      break;

    // User was created but needs administration approval before logged in.
    case 'create_approval':
      $message = t('Your account is currently pending approval by the site administrator.');
      $next_action = 'logout';
      break;
  }

  if (!empty($message)) {
    drupal_set_message($message);
  }

  switch ($next_action) {
    case 'logout':
      if (isset($connector['logout callback']) && is_callable($connector['logout callback'])) {
        call_user_func($connector['logout callback'], $connector, $connection->cid);
      }
      break;
    case 'goto':
      drupal_goto($destination, array('query' => $destination_query));
      break;
  }
}

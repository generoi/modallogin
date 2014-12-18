<?php

/**
 * Implements hook_rules_event_info().
 */
function modallogin_rules_event_info() {
  $variables = array(
    'account' => array(
      'type' => 'user',
      'label' => t('The created users account.'),
    ),
  );

  return array(
    'modallogin_create_login_verify' => array(
      'label' => t('After a user has been created and logged in but needs to verify email'),
      'group' => t('Modallogin'),
      'variables' => $variables,
    ),
    'modallogin_create_verify' => array(
      'label' => t('After a user has been created but needs to verify email'),
      'group' => t('Modallogin'),
      'variables' => $variables,
    ),
    'modallogin_login' => array(
      'label' => t('After a user has been logged in using OAuth services'),
      'group' => t('Modallogin'),
      'variables' => $variables,
    ),
    'modallogin_create_approval' => array(
      'label' => t('After a user has been created but is pending approval'),
      'group' => t('Modallogin'),
      'variables' => $variables,
    ),
    'modallogin_user_account_set' => array(
      'label' => t('After a user sets their account information through the modal'),
      'group' => t('Modallogin'),
      'variables' => $variables,
    ),
  );
}

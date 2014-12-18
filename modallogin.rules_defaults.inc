<?php

/**
 * Implements hook_default_rules_configuration().
 */
function modallogin_default_rules_configuration() {
  $items = array();
  $items['modallogin_create_verify_email'] = entity_import('rules_config', '{
    "modallogin_create_verify_email" : {
      "LABEL" : "Send a verification email to a created user which they need to click to be upgraded to full user",
      "PLUGIN" : "reaction rule",
      "OWNER" : "rules",
      "REQUIRES" : [ "rules", "modallogin" ],
      "ON" : {
        "modallogin_create_verify" : [],
        "modallogin_user_account_set" : []
      },
      "DO" : [
        { "mail" : {
            "to" : [ "account:mail" ],
            "subject" : "Account details for [account:name] at [site:name]",
            "message" : "[account:name],\\r\\n\\r\\nThank you for registering at [site:name]. You may now log in by clicking this link or copying and pasting it to your browser:\\r\\n\\r\\n[account:validate-url]\\r\\n\\r\\nAfter setting your password, you will be able to log in at [site:login-url] in the future using:\\r\\n\\r\\nusername: [account:name]\\r\\npassword: Your password\\r\\n\\r\\n--  [site:name] team",
            "from" : "[site:mail]",
            "language" : [ "account:language" ]
          }
        }
      ]
    }
  }');
  // $items['modallogin_create_login_email'] = entity_import('rules_config', '{
  //   "modallogin_create_login_email" : {
  //     "LABEL" : "Send a notification email to a created user telling them about the site.",
  //     "PLUGIN" : "reaction rule",
  //     "OWNER" : "rules",
  //     "REQUIRES" : [ "rules", "modallogin" ],
  //     "ON" : { "modallogin_create_login" : [] },
  //     "DO" : [
  //       { "mail" : {
  //           "to" : [ "account:mail" ],
  //           "subject" : "Account details for [account:name] at [site:name]",
  //           "message" : "[account:name],\\r\\n\\r\\nThank you for registering at [site:name]. You are now able to log in at [site:login-url] using:\\r\\n\\r\\nusername: [account:name]\\r\\npassword: Your password\\r\\n\\r\\n--  [site:name] team",
  //           "from" : "[site:mail]",
  //           "language" : [ "account:language" ]
  //         }
  //       }
  //     ]
  //   }
  // }');
  $items['modallogin_create_approval_email'] = entity_import('rules_config', '{
    "modallogin_create_approval_email" : {
      "LABEL" : "Send a notification email to a created user informing them they are pending administration approval.",
      "PLUGIN" : "reaction rule",
      "OWNER" : "rules",
      "REQUIRES" : [ "rules", "modallogin" ],
      "ON" : { "modallogin_create_approval" : [] },
      "DO" : [
        { "mail" : {
            "to" : [ "account:mail" ],
            "subject" : "Account details for [account:name] at [site:name] (pending admin approval)",
            "message" : "[account:name],\\r\\n\\r\\nThank you for registering at [site:name]. Your application for an account is currently pending approval. Once it has been approved, you will receive another e-mail containing information about how to log in, set your password, and other details.\\r\\n\\r\\n--  [site:name] team",
            "from" : "[site:mail]",
            "language" : [ "account:language" ]
          }
        }
      ]
    }
  }');

  return $items;
}

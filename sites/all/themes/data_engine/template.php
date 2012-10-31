<?php

/**
 * BLOCKS
 */

/**
 * Implements hook_preprocess_block()
 */

function data_engine_preprocess_block(&$vars) {
  /* Set shortcut variables. Hooray for less typing! */
  $block_id = $vars['block']->module . '-' . $vars['block']->delta;
  $classes = &$vars['classes_array'];
  $title_classes = &$vars['title_attributes_array']['class'];
  $content_classes = &$vars['content_attributes_array']['class'];

  /* Add global classes to all blocks */
  $title_classes[] = 'block-title';
  $content_classes[] = 'block-content';

  /* Uncomment the line below to see variables you can use to target a block */
  // print $block_id . '<br/>';

  /* Add classes based on the block delta. */
  switch ($block_id) {
    /* Contact Information block */
    case 'block-2':
      $classes[] = 'contact-info';
      break;
    /* Main Menu block */
    case 'menu_block-1':
    /* User Menu block */
    case 'menu_block-2':
    /* Footer Menu block */
    case 'menu_block-3':
    /* User Login block */
    case 'user-login':
      $title_classes[] = 'element-invisible';
      break;
    /* Related Visualizations block */
    case 'views-visualizations_related-block':
      $title_classes[] = 'block-title--ruled';
      break;
    /* FAQs block */
    case 'views-aefb000727717838cdc758d4891a3dd4':
      $classes[] = 'accordion';
      drupal_add_library('system', 'ui.accordion');
      // Turn faq articles into accordians.
      drupal_add_js("(function ($) {
          $(document).ready(function(){
            $('.accordion').find('.faq-title').addClass('ss-navigateright');
            $('.accordion').find('.ds-header').addClass('is-collapsed').click(
              function() {
                $(this).toggleClass('is-collapsed is-expanded').next().toggle('fast');
                $(this).find('.faq-title').toggleClass('ss-navigateright ss-navigatedown');
                return false;
              }
            ).next().hide();
          });
        })(jQuery);",
        'inline');
      break;
  }
}


/**
 * FIELDS
 */

/**
 * Implements hook_preprocess_field()
 */

function data_engine_preprocess_field(&$vars) {
  /* Set shortcut variables. Hooray for less typing! */
  $field = $vars['element']['#field_name'];
  $bundle = $vars['element']['#bundle'];
  $mode = $vars['element']['#view_mode'];
  $classes = &$vars['classes_array'];
  $title_classes = &$vars['title_attributes_array']['class'];
  $content_classes = &$vars['content_attributes_array']['class'];
  $item_classes = array();

  /* Global field styles */
  $classes[] = 'field-wrapper';
  $title_classes[] = 'field-label';
  $content_classes[] = 'field-items';
  $item_classes[] = 'field-item';

  /* Uncomment the lines below to see variables you can use to target a field */
  // print '<strong>Field:</strong> ' . $field . '<br/>';
  // print '<strong>Bundle:</strong> ' . $bundle  . '<br/>';
  // print '<strong>Mode:</strong> ' . $mode .'<br/>';

  switch ($field) {
    case 'field_data_tags':
      $title_classes[] = 'ss-icon';
      break;
    case 'field_website':
      if ($mode != 'full') {
        $classes[] = 'ss-link';
      }
      break;
    case 'user_email_address':
      $classes[] = 'ss-mail';
      break;
    case 'field_request_status':
      $item_classes[] = 'request-status';
      $item_classes[] = 'request-status--' . drupal_html_class($vars['items'][0]['#markup']);
      break;
    case 'field_visualization_type':
      // Add visualization type icon classes and remove others.
      if ($mode == 'teaser') {
        $classes = array(
          'visualization-icon',
          drupal_html_class($vars['items'][0]['#title']),
        );

        // Point link to the parent visualization rather than the term.
        $path = drupal_get_path_alias('node/' . $vars['element']['#object']->nid);
        $vars['items'][0]['#href'] = $path;
      }
      break;
    case 'share_links':
      //$title_classes[] = 'field-label--ruled';
      break;
    case 'title':
      if ($bundle == 'faq') {
        $classes[] = 'faq-title';
      }
      break;
  }

  switch ($bundle) {
    case 'dataset':
    case 'visualization':
      switch ($field) {
        case 'author':
        case 'dataset_record_count':
        case 'field_location_type':
        case 'field_date_range':
        case 'field_visualization_type':
        case 'post_date':
          $classes[] = 'field--supplement';
          break;
        case 'field_data_tags':
        case 'field_data_categories':
          $classes[] = 'field--supplement';
          if ($mode == 'full') {
            $vars['theme_hook_suggestions'][] = 'field__custom_separated';
          }
          break;
        case 'field_utilized_datasets':
          $content_classes[] = 'field--supplement';
          $content_classes[] = 'related-list';
          $item_classes[] = 'related-item';
          break;
        case 'field_organization':
          if ($mode = 'full') {
            $title_classes[] = 'field-collection-label';
          }
          break;
        }
      break;
    case 'field_organization':
      switch ($field) {
        case 'field_organization_name':
        case 'field_website':
          if ($mode == 'full') {
            $classes[] = 'field--supplement';
          }
        break;
      }
    break;
  }

  // Apply odd or even classes along with our custom classes to each item */
  foreach ($vars['items'] as $delta => $item) {
    $vars['item_attributes_array'][$delta]['class'] = $item_classes;
    $striping = $delta % 2 ? 'odd' : 'even';
    $vars['item_attributes_array'][$delta]['class'][] = $striping;
  }
}


/**
 * FLAG
 */

/**
 * Implements preprocess_flag
 */

function data_engine_preprocess_flag(&$vars) {
  // Add classes to Remove from my Downloads link.
  $name = $vars['flag_name_css'];
  switch ($name) {
    case 'dataset-downloads':
      if ($vars['action'] == "unflag") {
        $vars['flag_classes'] = $vars['flag_classes'] . " button-remove ss-delete";
        break;
      }
      $vars['flag_classes'] = $vars['flag_classes'] . " button-add ss-plus";
      break;
    case 'dataset-request-interest':
      $vars['flag_classes'] = $vars['flag_classes'] . " button--interest ss-star";
      break;
    case 'dataset-request-removal':
      $vars['flag_classes'] = $vars['flag_classes'] . " button-remove ss-delete";
      break;
  }
}

 /**
 * FORMS
 */

/**
 * Implements hook_form_alter
 */

function data_engine_form_alter(&$form, &$form_state, $form_id) {

  /* Move the links on the user login block */
  if ($form_id == 'user_login_block') {
    $form['search_block_form']['#attributes']['placeholder'] = "Enter a search term…";
    $form['actions']['register'] = array(
      '#type' => 'link',
      '#title' => t('Create a New Account'),
      '#href' => 'user/register',
      '#options' => array(
        'attributes' => array(
          'class' => array('button--primary'),
        )
      ),
    );
    $form['actions']['new_password'] = array(
      '#type' => 'link',
      '#title' => t('Request new password'),
      '#href' => 'user/password',
      '#options' => array(
        'attributes' => array(
          'class' => array('button--tertiary'),
        )
      ),
    );
    unset($form['links']);
  }

  /* Add placeholder text to a form */
  if ($form_id == 'search_block_form') {
    $form['search_block_form']['#attributes']['placeholder'] = "Enter a search term…";
    $form['actions']['submit']['#attributes']['class'][] = "ss-icon";
  }

  /* Move the current password to the bottom */
  if ($form_id == 'user_profile_form') {
    $form['account']['current_pass']['#weight'] = 5;
    $form['account']['pass']['#prefix'] = '<h3 class="field-group-label">Password</h3>'
      . '<div class="description description--above">'
      . $form['account']['pass']['#description']
      . '</div>';
    unset($form['account']['pass']['#description']);
    $form['account']['current_pass']['#prefix'] = '<div class="description description--above">'
      . $form['account']['current_pass']['#description']
      . '</div>';
    unset($form['account']['current_pass']['#description']);
  }

  // Remove email class from email inputs to avoid uniform styling.
  if (isset($form['submitted'])) {
    foreach ($form['submitted'] as $key => &$item) {
      if($item['#type'] == 'webform_email' && in_array('email', $item['#attributes']['class'])) {
        unset($item['#attributes']['class']);
      }
    }
  }
}

/**
 * Implements hook_preprocess_form_element
 */

function data_engine_preprocess_form_element(&$vars) {
  $element = &$vars['element'];
  $title = &$vars['element']['#title'];
  switch ($title) {
    case 'E-mail address':
      if (in_array('account', $element['#array_parents'])) {
        $vars['theme_hook_suggestions'][] = 'form_element__description_above';
      }
      break;
  }
}

/**
 * Implements hook_preprocess_form_element_label
 */

function data_engine_preprocess_form_element_label(&$vars) {
  $element = &$vars['element'];
  $title = &$vars['element']['#title'];
  switch ($title) {
    case 'Password':
      if (in_array('account', $element['#array_parents'])) {
        $title = t('New Password');
      }
      break;
    case 'Confirm password':
        $title = t('Confirm New Password');
      break;
  }
}

/**
 * Custom implementation of theme_form_element
 * Used to move the description above the form field.
 */
function data_engine_form_element__description_above($variables) {
  $element = &$variables['element'];
  // This is also used in the installer, pre-database setup.
  $t = get_t();

  // This function is invoked as theme wrapper, but the rendered form element
  // may not necessarily have been processed by form_builder().
  $element += array(
    '#title_display' => 'before',
  );

  // Add element #id for #type 'item'.
  if (isset($element['#markup']) && !empty($element['#id'])) {
    $attributes['id'] = $element['#id'];
  }
  // Add element's #type and #name as class to aid with JS/CSS selectors.
  $attributes['class'] = array('form-item');
  if (!empty($element['#type'])) {
    $attributes['class'][] = 'form-type-' . strtr($element['#type'], '_', '-');
  }
  if (!empty($element['#name'])) {
    $attributes['class'][] = 'form-item-' . strtr($element['#name'], array(' ' => '-', '_' => '-', '[' => '-', ']' => ''));
  }
  // Add a class for disabled elements to facilitate cross-browser styling.
  if (!empty($element['#attributes']['disabled'])) {
    $attributes['class'][] = 'form-disabled';
  }
  $output = '<div' . drupal_attributes($attributes) . '>' . "\n";

  // If #title is not set, we don't display any label or required marker.
  if (!isset($element['#title'])) {
    $element['#title_display'] = 'none';
  }
  $prefix = isset($element['#field_prefix']) ? '<span class="field-prefix">' . $element['#field_prefix'] . '</span> ' : '';
  $suffix = isset($element['#field_suffix']) ? ' <span class="field-suffix">' . $element['#field_suffix'] . '</span>' : '';

  $description = '';
  if (!empty($element['#description'])) {
    $description .= '<div class="description description--above">' . $element['#description'] . "</div>\n";
  }

  switch ($element['#title_display']) {
    case 'before':
    case 'invisible':
      $output .= ' ' . theme('form_element_label', $variables);
      $output .= ' ' . $description . "\n";
      $output .= ' ' . $prefix . $element['#children'] . $suffix . "\n";
      break;

    case 'after':
      $output .= ' ' . $prefix . $element['#children'] . $suffix;
      $output .= ' ' . $description . "\n";
      $output .= ' ' . theme('form_element_label', $variables) . "\n";
      break;

    case 'none':
    case 'attribute':
      // Output no label and no required marker, only the children.
      $output .= ' ' . $prefix . $element['#children'] . $suffix . "\n";
      break;
  }

  $output .= "</div>\n";

  return $output;
}

/**
 * HTML
 */

/**
 * Implements hook_preprocess_html.
 */
function data_engine_preprocess_html(&$variables) {
  global $user;
  // Add class for login page if on /user and logged out.
  if($user->uid == 0 && arg(0) == 'user' && !arg(1)) {
    $variables['classes_array'][] = 'page-user-login';
  }

  $options = array(
    'group' => JS_THEME,
    'scope' => 'footer',
  );
  drupal_add_js(drupal_get_path('theme', 'data_engine') . '/webfonts/ss-standard.js',
    $options
  );
}

/**
 * MENUS
 */

/**
 * Overrides theme_menu_local_tasks().
 */

function data_engine_menu_local_tasks(&$variables) {
  $output = '';

  if (!empty($variables['primary'])) {
    $variables['primary']['#prefix'] = '<h2 class="element-invisible">' . t('Primary tabs') . '</h2>';
    $variables['primary']['#prefix'] .= '<ul class="nav nav-pills nav-inline">';
    $variables['primary']['#suffix'] = '</ul>';
    $output .= drupal_render($variables['primary']);
  }

  if (!empty($variables['secondary'])) {
    $variables['secondary']['#prefix'] = '<h2 class="element-invisible">' . t('Secondary tabs') . '</h2>';
    $variables['secondary']['#prefix'] .= '<ul class="nav nav-pills nav-inline">';
    $variables['secondary']['#suffix'] = '</ul>';
    $output .= drupal_render($variables['secondary']);
  }

  return $output;
}

/**
 * Overrides theme_menu_tree() for the main menu.
 */
function data_engine_menu_tree__main_menu($variables) {
  return '<ul class="nav--main">' . $variables['tree'] . '</ul>';
}

/**
 * Overrides theme_menu_tree() for the user menu.
 */
function data_engine_menu_tree__user_menu($variables) {
  return '<ul class="nav--user">' . $variables['tree'] . '</ul>';
}

/**
 * Overrides theme_menu_tree() for the footer menu.
 */
function data_engine_menu_tree__menu_footer($variables) {
  return '<ul class="nav--footer">' . $variables['tree'] . '</ul>';
}

/**
 * Implements hook_preprocess_menu_link()
 */

function data_engine_preprocess_menu_link(&$vars) {
  /* Set shortcut variables. Hooray for less typing! */
  $menu = $vars['element']['#original_link']['menu_name'];
  $mlid = $vars['element']['#original_link']['mlid'];
  $item_classes = &$vars['element']['#attributes']['class'];
  $link_classes = &$vars['element']['#localized_options']['attributes']['class'];

  /* Add global classes to all menu links */
  $item_classes[] = 'nav-item';
  $link_classes[] = 'nav-link';

  switch ($mlid) {
    // My dashboard
    case 2:
      if (arg(0) == 'user' && is_numeric(arg(1))) {
        $link_classes[] = 'active';
        $item_classes[] = 'active';
      }
      break;
    // Sign in
    case 538:
      $link_classes[] = 'button--sign-in';
      break;
    // Sign out
    case 15:
      $link_classes[] = 'button--sign-out';
      break;
  }
  // Add active classes to My Dashboard becuase they seem to not be working

}


/**
 * NODES
 */

/**
 * Implements hook_preprocess_node()
 */

function data_engine_preprocess_node(&$vars) {
  /* Set shortcut variables. Hooray for less typing! */
  $type = $vars['type'];
  $mode = $vars['view_mode'];
  $classes = &$vars['classes_array'];
  $title_classes = &$vars['title_attributes_array']['class'];
  $content_classes = &$vars['content_attributes_array']['class'];

  /* Example: Adding a classes base on View Mode */
  // switch ($mode) {
  //   case 'photo_teaser':
  //     $classes[] = 'bg-white gutters-half l-space-trailing';
  //     break;
  // }
}

/**
 * Implements hook_process_page()
 */
function data_engine_preprocess_page(&$vars) {
  $page = &$vars['page'];
  $title_classes = &$vars['title_attributes_array']['class'];

  // Set some default drupal_add_js options
  $options = array(
    'type' => 'inline',
    'scope' => 'footer',
  );

  // Move the view all visualizations button to a more visible spot.
  if($vars['is_front']) {
    drupal_add_js(
      "Drupal.behaviors.moveViewAll = {
        attach: function(context) {
          jQuery('.button--view-all').appendTo('#edit-term-node-tid-depth-wrapper');
        }
      };",
      $options
    );
  }

  if (arg(0) == 'find') {
    drupal_add_js(path_to_theme() . "/js/data_engine_fancybox_preview.js",
      array(
        'scope' => 'footer',
      )
    );
  }

  if (arg(0) == 'node' && isset($vars['node'])) {
    if($vars['node']->nid == 20) {
      $title_classes[] = 'element-invisible';
    }
  }

  if (arg(0) == 'user') {
    if (arg(2) == 'edit') {
      $vars['title'] = t('Edit Profile');
      // Add a Back to Dashboard link
      $page['content']['profile_link'] = array(
        '#type' => 'link',
        '#title' => t('Back to Dashboard'),
        '#href' => 'user',
        '#options' => array(
          'attributes' => array(
            'class' => array('link-dashboard'),
          ),
        ),
        '#weight' => -20,
      );
      // Make sure the weight is taken into account.
      $page['content']['#sorted'] = FALSE;
      // Remove local menu links.
      unset($vars['tabs']);
    } elseif (arg(1) == 'register') {
      $vars['title'] = t('Create a New Account');
    } elseif (arg(1) == 'password') {
      $vars['title'] = t('Request New Password');
    } elseif (arg(1) == 'Login' || $vars['user']->uid == 0) {
      $vars['title'] = t('Login');
    } elseif (!arg(1) || is_numeric(arg(1))) {
      $vars['title'] = t('Dashboard');
      $title_classes[] = 'page-title--small';
      // Remove local menu links.
      unset($vars['tabs']);
    }
  }
}

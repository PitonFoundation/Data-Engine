<?php

/**
 * Implements hook_js_alter()
 */

// function center_js_alter(&$javascript) {
//   // Collect the scripts we want in to remain in the header scope.
//   $header_scripts = array(
//     'sites/all/libraries/modernizr/modernizr.min.js',
//   );

//   // Change the default scope of all other scripts to footer.
//   // We assume if the script is scoped to header it was done so by default.
//   foreach ($javascript as $key => &$script) {
//     if ($script['scope'] == 'header' && !in_array($script['data'], $header_scripts)) {
//       $script['scope'] = 'footer';
//     }
//   }
// }

/**
 * MENUS
 */

/**
 * Overrides theme_menu_tree().
 */

function center_menu_tree($variables) {
  return '<ul class="nav">' . $variables['tree'] . '</ul>';
}

/**
 * Custom implementation of theme_menu_link()
 * This adds a span for including icons before a menu link.
 */

function center_menu_link__icon(array $variables) {
  $element = $variables['element'];
  $sub_menu = '';

  /* Prevent the <span> tag from being escaped */
  $element['#localized_options']['html'] = TRUE;

  if ($element['#below']) {
    $sub_menu = drupal_render($element['#below']);
  }
  $icon = '<span' . drupal_attributes($element['#icon_attributes']) . '></span>';
  $output = l($icon . $element['#title'], $element['#href'], $element['#localized_options']);
  return '<li' . drupal_attributes($element['#attributes']) . '>' . $output . $sub_menu . "</li>\n";
}

/**
 * Overrides theme_menu_local_tasks().
 */

function center_menu_local_tasks(&$variables) {
  $output = '';

  if (!empty($variables['primary'])) {
    $variables['primary']['#prefix'] = '<h2 class="element-invisible">' . t('Primary tabs') . '</h2>';
    $variables['primary']['#prefix'] .= '<ul class="nav nav-tabs">';
    $variables['primary']['#suffix'] = '</ul>';
    $output .= drupal_render($variables['primary']);
  }

  if (!empty($variables['secondary'])) {
    $variables['secondary']['#prefix'] = '<h2 class="element-invisible">' . t('Secondary tabs') . '</h2>';
    $variables['secondary']['#prefix'] .= '<ul class="nav nav-pills">';
    $variables['secondary']['#suffix'] = '</ul>';
    $output .= drupal_render($variables['secondary']);
  }

  return $output;
}

/**
 * LINKS
 */

/**
 * Overrides theme_links().
 * This adds classes to the lists to make them more in line with menus.
 */

function center_links($variables) {
  $links = $variables['links'];
  $attributes = $variables['attributes'];
  $heading = $variables['heading'];
  global $language_url;
  $output = '';

  if (count($links) > 0) {
    $output = '';

    // Treat the heading first if it is present to prepend it to the
    // list of links.
    if (!empty($heading)) {
      if (is_string($heading)) {
        // Prepare the array that will be used when the passed heading
        // is a string.
        $heading = array(
          'text' => $heading,
          // Set the default level of the heading.
          'level' => 'h2',
        );
      }
      $output .= '<' . $heading['level'];
      if (!empty($heading['class'])) {
        $output .= drupal_attributes(array('class' => $heading['class']));
      }
      $output .= '>' . check_plain($heading['text']) . '</' . $heading['level'] . '>';
    }

    $output .= '<ul' . drupal_attributes($attributes) . '>';

    $num_links = count($links);
    $i = 1;

    foreach ($links as $key => $link) {
      $class = array($key);
      $class[] = 'nav-item';
      $link['attributes']['class'][] = 'nav-link';

      // Add first, last and active classes to the list of links to help out themers.
      if ($i == 1) {
        $class[] = 'first';
      }
      if ($i == $num_links) {
        $class[] = 'last';
      }
      if (isset($link['href']) && ($link['href'] == $_GET['q'] || ($link['href'] == '<front>' && drupal_is_front_page()))
           && (empty($link['language']) || $link['language']->language == $language_url->language)) {
        $class[] = 'active';
      }
      $output .= '<li' . drupal_attributes(array('class' => $class)) . '>';

      if (isset($link['href'])) {
        // Pass in $link as $options, they share the same keys.
        $output .= l($link['title'], $link['href'], $link);
      }
      elseif (!empty($link['title'])) {
        // Some links are actually not links, but we wrap these in <span> for adding title and class attributes.
        if (empty($link['html'])) {
          $link['title'] = check_plain($link['title']);
        }
        $span_attributes = '';
        if (isset($link['attributes'])) {
          $span_attributes = drupal_attributes($link['attributes']);
        }
        $output .= '<span' . $span_attributes . '>' . $link['title'] . '</span>';
      }

      $i++;
      $output .= "</li>\n";
    }

    $output .= '</ul>';
  }

  return $output;
}

/**
 * FIELDS
 */

/**
 * Overrides theme_field()
 * Remove the hard coded classes so we can add them in preprocess functions.
 */

function center_field($variables) {
  $output = '';

  // Render the label, if it's not hidden.
  if (!$variables['label_hidden']) {
    $output .= '<div ' . $variables['title_attributes'] . '>' . $variables['label'] . ':&nbsp;</div>';
  }

  // Render the items.
  $output .= '<div ' . $variables['content_attributes'] . '>';
  foreach ($variables['items'] as $delta => $item) {
    $output .= '<div ' . $variables['item_attributes'][$delta] . '>' . drupal_render($item) . '</div>';
  }
  $output .= '</div>';

  // Render the top-level DIV.
  $output = '<div class="' . $variables['classes'] . '"' . $variables['attributes'] . '>' . $output . '</div>';

  return $output;
}

/**
 * Overrides theme_ds_field_minimal().
 * Still allow the normal drupal field classes to apply.
 */
function center_field__minimal($variables) {
  $output = '';

  $config = $variables['ds-config'];

  // Add a simple wrapper.
  $output .= '<div class="' . $variables['classes'] . '">';

  // Render the label.
  if (!$variables['label_hidden']) {
    $output .= '<div' . $variables['title_attributes'] . '>' . $variables['label'];
    if (!isset($config['lb-col'])) {
      $output .= ':&nbsp;';
    }
    $output .= '</div>';
  }

  // Render the items.
  foreach ($variables['items'] as $delta => $item) {
    $output .= drupal_render($item);
  }
  $output .="</div>";

  return $output;
}

/**
 * Overrides theme_ds_field_minimal().
 * Still allow the normal drupal field classes to apply.
 */
function center_field__raw($variables) {
  $output = '';

  // Render the items.
  foreach ($variables['items'] as $delta => $item) {
    $output .= drupal_render($item);
  }

  return $output;
}

/**
 * Custom implementation of theme_field()
 * Turns multivalued fields into a comma separated list.
 * USAGE: $vars['theme_hook_suggestions'][] = 'field__custom_separated';
 */

function center_field__custom_separated($variables) {
  $output = '';

  // Render the label, if it's not hidden.
  if (!$variables['label_hidden']) {
    $output .= '<label ' . $variables['title_attributes'] . '>' . $variables['label'] . '&nbsp;</label>';
  }

  // Render the items.
  //$output .= '<div ' . $variables['content_attributes'] . '>';
  $count = 1;
  foreach ($variables['items'] as $delta => $item) {
    $output .= '<span ' . $variables['item_attributes'][$delta] . '>' . drupal_render($item) . '</span>';
    if ($count < count($variables['items'])) { $output .= ', '; }
    $count++;
  }
  //$output .= '</div>';

  // Render the top-level DIV.
  $output = '<div class="' . $variables['classes'] . '"' . $variables['attributes'] . '>' . $output . '</div>';

  return $output;
}

/**
 * Custom implementation of theme_field()
 * Takes a url and renders it as a download link.
 * USAGE: $vars['theme_hook_suggestions'][] = 'field__custom_download';
 */

function center_field__custom_download($variables) {
  $output = '';

  // Render the label, if it's not hidden.
  if (!$variables['label_hidden']) {
    $output .= '<label ' . $variables['title_attributes'] . '>' . $variables['label'] . ':&nbsp;</label>';
  }

  // Render the items.
  $count = 1;
  foreach ($variables['items'] as $delta => $item) {
    $output .= l(
      'Download',
      $item['#markup'],
      array(
        'attributes' => array(
          'class' => array('download-link'),
        )
      )
    );
  }

  // Render the top-level DIV.
  $output = '<div class="' . $variables['classes'] . '"' . $variables['attributes'] . '>' . $output . '</div>';

  return $output;
}

/**
 * Custom implementation of theme_field()
 * Wraps the field in an h3 and nukes the label an outer wrapper.
 * USAGE: $vars['theme_hook_suggestions'][] = 'field__custom_h2';
 */

function center_field__custom_h2($variables) {
  $output = '';
  foreach ($variables['items'] as $delta => $item) {
    $output .= '<h2 ' . $variables['item_attributes'][$delta] . '>' . drupal_render($item) . '</h2>';
  }
  return $output;
}

/**
 * Custom implementation of theme_field()
 * Wraps the field in an h3 and nukes the label an outer wrapper.
 * USAGE: $vars['theme_hook_suggestions'][] = 'field__custom_h3';
 */

function center_field__custom_h3($variables) {
  $output = '';
  foreach ($variables['items'] as $delta => $item) {
    $output .= '<h3 ' . $variables['item_attributes'][$delta] . '>' . drupal_render($item) . '</h3>';
  }
  return $output;
}

/**
 * NODES
 */

/**
 * PAGE
 */

/**
 * Implements hook_preprocess_page()
 */
function center_preprocess_page(&$vars) {
  $vars['title_attributes_array']['class'][] = 'page-title';
}

/**
 * BLOCKS
 */

/**
 * BREADCRUMBS
 */

function center_breadcrumb($variables) {
  $breadcrumb = $variables['breadcrumb'];

  if (!empty($breadcrumb)) {
    // Provide a navigational heading to give context for breadcrumb links to
    // screen-reader users. Make the heading invisible with .element-invisible.
    $output = '<h2 class="element-invisible">' . t('You are here') . '</h2>';

    $output .= '<div class="breadcrumb">' . implode(' <span class="breadcrumb-separator">/</span> ', $breadcrumb) . '</div>';
    return $output;
  }
}

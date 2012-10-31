<?php

/**
 * @file
 * Default theme implementation to display the basic html structure of a single
 * Drupal page.
 *
 * Variables:
 * - $css: An array of CSS files for the current page.
 * - $language: (object) The language the site is being displayed in.
 *   $language->language contains its textual representation.
 *   $language->dir contains the language direction. It will either be 'ltr' or 'rtl'.
 * - $rdf_namespaces: All the RDF namespace prefixes used in the HTML document.
 * - $grddl_profile: A GRDDL profile allowing agents to extract the RDF data.
 * - $head_title: A modified version of the page title, for use in the TITLE
 *   tag.
 * - $head_title_array: (array) An associative array containing the string parts
 *   that were used to generate the $head_title variable, already prepared to be
 *   output as TITLE tag. The key/value pairs may contain one or more of the
 *   following, depending on conditions:
 *   - title: The title of the current page, if any.
 *   - name: The name of the site.
 *   - slogan: The slogan of the site, if any, and if there is no title.
 * - $head: Markup for the HEAD section (including meta tags, keyword tags, and
 *   so on).
 * - $styles: Style tags necessary to import all CSS files for the page.
 * - $scripts: Script tags necessary to load the JavaScript files and settings
 *   for the page.
 * - $page_top: Initial markup from any modules that have altered the
 *   page. This variable should always be output first, before all other dynamic
 *   content.
 * - $page: The rendered page content.
 * - $page_bottom: Final closing markup from any modules that have altered the
 *   page. This variable should always be output last, after all other dynamic
 *   content.
 * - $classes String of classes that can be used to style contextually through
 *   CSS.
 *
 * @see template_preprocess()
 * @see template_preprocess_html()
 * @see template_process()
 */
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML+RDFa 1.0//EN"
  "http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd">
<html>
<head>

  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <meta name="robots" content="noindex, nofollow" />
  <script type="text/javascript">
    var oEditor   = window.parent.CKEDITOR ;
    var instance  = oEditor.currentInstance ;
    var lang      = oEditor.lang ;
    var config    = oEditor.config ;
  </script>
  
  <?php print $head; ?>
  <title><?php print $head_title; ?></title>
  <?php print $styles; ?>
  <link type="text/css" rel="stylesheet" media="all" href="<?php print drupal_get_path("module", "node_embed") ?>/ckeditor/ck_nodeembed.css" />

  <!--[if IE]>
  <link type="text/css" rel="stylesheet" media="all" href="<?php print base_path() . path_to_theme();?>/css/ie.css" />
  <![endif]-->
  <!--[if IE 6]>
  <link type="text/css" rel="stylesheet" media="all" href="<?php print base_path() . path_to_theme();?>/css/ie6.css" />
  <![endif]-->
  <?php print $scripts; ?>
  <script type="text/javascript" src="<?php 
    print base_path() . drupal_get_path('module', 'node_embed')?>/ckeditor/ck_nodeembed.js">
  </script>
</head>
<body class="view-ck" style="overflow: auto">
  <div id="divInfo">
    <?php print $page; ?>
  </div>
</body>
</html>

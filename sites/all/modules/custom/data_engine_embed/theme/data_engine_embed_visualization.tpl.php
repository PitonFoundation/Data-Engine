<html>
<head>
  <meta charset="utf-8" />
  <title><?php echo $title; ?></title>
  <?php echo $head; ?>
  <?php echo '<link rel="stylesheet" type="text/css" href="'
    . url(drupal_get_path("theme", "data_engine") . '/css/wysiwyg.css',
        array ('absolute' => TRUE)) . '" />'; ?>
  <?php echo $js; ?>
</head>
<body class="embed--visualization embed--<?php echo drupal_html_class($type); ?>">
  <article class="embedded embedded--<?php echo drupal_html_class($type); ?>">
    <?php if (isset($header) && $header): ?>
    <header class="visualization-header"><h1 class="visualization-title"><?php echo $title; ?></h1></header>
    <?php endif ?>
    <?php echo $visualization; ?>
    <footer class="visualization-footer">
      <cite class="source">Source: <a href="<?php echo $url; ?>"><?php echo $url; ?></a></cite>
    </footer>
  </article>
  <?php echo $footerJS; ?>
</body>
</html>

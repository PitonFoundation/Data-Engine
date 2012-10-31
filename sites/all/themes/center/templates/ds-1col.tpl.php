<?php

/**
 * @file
 * Removes clearfix from the Display Suite 1 column template.
 */
?>
<div class="ds-1col <?php print $classes;?> <?php print $ds_content_classes;?>">

  <?php if (isset($title_suffix['contextual_links'])): ?>
  <?php print render($title_suffix['contextual_links']); ?>
  <?php endif; ?>

  <?php print $ds_content; ?>
</div>
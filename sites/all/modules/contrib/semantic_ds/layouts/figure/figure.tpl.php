<?php
/**
 * @file
 * Display Suite figure template.
 *
 * Available variables:
 *
 * Layout:
 * - $classes: String of classes that can be used to style this layout.
 * - $contextual_links: Renderable array of contextual links.
 *
 * Regions:
 *
 * - $figure: Rendered content for the "Figure" region.
 * - $figure_classes: String of classes that can be used to style
 *     the "Figure" region.
 *
 * - $figcaption: Rendered content for the "Content" region.
 * - $figcaption: String of classes that can be used to style
 *     the "Content" region.
 */
?>
<figure class="<?php print $classes . ' ' . $figure_classes; ?>">

  <?php if (isset($title_suffix['contextual_links'])): ?>
    <?php print render($title_suffix['contextual_links']); ?>
  <?php endif; ?>

  <?php if ($figure): ?>
    <?php print $figure; ?>
  <?php endif; ?>

  <?php if ($figcaption): ?>
    <figcaption class="ds-figcaption<?php print $figcaption_classes; ?>">
      <?php print $figcaption; ?>
    </figcaption>
  <?php endif; ?>

</figure>

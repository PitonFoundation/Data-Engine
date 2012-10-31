<?php
/**
 * @file
 * Display Suite Section template.
 *
 * Available variables:
 *
 * Layout:
 * - $classes: String of classes that can be used to style this layout.
 * - $contextual_links: Renderable array of contextual links.
 *
 * Regions:
 *
 * - $header: Rendered content for the "Header" region.
 * - $header_classes: String of classes that can be used to style
 *     the "Header" region.
 *
 * - $figure: Rendered content for the "Figure" region.
 * - $figure_classes: String of classes that can be used to style
 *     the "Figure" region.
 *
 * - $main_content: Rendered content for the "Content" region.
 * - $main_content_classes: String of classes that can be used to style
 *     the "Content" region.
 *
 * - $aside: Rendered content for the "Content" region.
 * - $aside_classes: String of classes that can be used to style
 *     the "Content" region.
 *
 * - $footer: Rendered content for the "Footer " region.
 * - $footer_classes: String of classes that can be used to style
 *     the "Footer " region.
 */
?>
<section class="<?php print $classes; ?>">

  <?php if (isset($title_suffix['contextual_links'])): ?>
    <?php print render($title_suffix['contextual_links']); ?>
  <?php endif; ?>

  <?php if ($header && $hgroup): ?>
    <header class="ds-header<?php print $header_classes; ?>">
      <hgroup class="ds-hgroup<?php print $hgroup_classes; ?>">
        <?php print $hgroup; ?>
      </hgroup>
      <?php print $header; ?>
    </header>
  <?php elseif ($header): ?>
    <header class="ds-header<?php print $header_classes; ?>">
      <?php print $header; ?>
    </header>
  <?php elseif ($hgroup): ?>
    <hgroup class="ds-hgroup<?php print $hgroup_classes; ?>">
      <?php print $hgroup; ?>
    </hgroup>
  <?php endif; ?>

  <?php if ($main_content): ?>
    <div class="ds-content<?php print $main_content_classes; ?>">
      <?php print $main_content; ?>
    </div>
  <?php endif; ?>

  <?php if ($figure): ?>
    <figure class="ds-figure<?php print $figure_classes; ?>">
      <?php print $figure; ?>
    </figure>
  <?php endif; ?>

  <?php if ($aside): ?>
    <aside class="ds-aside<?php print $aside_classes; ?>">
      <?php print $aside; ?>
    </aside>
  <?php endif; ?>

  <?php if ($footer): ?>
    <footer class="ds-footer<?php print $footer_classes; ?>">
      <?php print $footer; ?>
    </footer>
  <?php endif; ?>

</section>

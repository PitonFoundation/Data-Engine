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
 * - $main_content: Rendered content for the "Figure" region.
 * - $main_content_classes: String of classes that can be used to style
 *     the "Content" region. Not very useful since we aren't printing markup.
 */
?>
<?php if ($main_content): ?>

  <?php print $main_content; ?>

<?php endif; ?>

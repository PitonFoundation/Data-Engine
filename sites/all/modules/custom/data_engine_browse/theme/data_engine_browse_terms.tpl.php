<ul class="terms no-bullets <?php echo implode(' ', $class); ?>">
  <?php foreach ($terms as $term): ?>
    <li class="term">
      <input type="checkbox" name="<?php echo $name; ?>[]" id="checkbox-tid-<?php echo $term['tid']; ?>" value="<?php echo $term['tid']; ?>" <?php if (isset($_GET[$name]) && in_array($term['tid'], $_GET[$name])) { print 'checked="checked" '; } ?>/>
      <label for="checkbox-tid-<?php echo $term['tid']; ?>"><?php echo $term['name'] ?></label>
      <?php if (isset($term['children'])): ?>
        <?php echo theme('data_engine_browse_terms', array('terms' => $term['children'], 'class' => array('children'), 'name' => $name)); ?>
      <?php endif ?>
    </li>
  <?php endforeach ?>
</ul>
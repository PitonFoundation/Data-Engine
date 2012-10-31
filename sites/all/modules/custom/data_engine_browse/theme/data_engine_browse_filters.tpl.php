<form action="" method="get" class="data-engine-browse-filters">
  <div class="search">
    <h3 class="filter-title">Search within Results</h3>
    <input type="search" name="search" value="<?php print $search_value ?>" />
    <div class="description">Filter datasets by title or year.</div>
  </div>
  <?php if (isset($map)): ?>
    <div class="map">
      <h3 class="filter-title">Location</h3>
      <?php print theme('data_engine_browse_map', array('map' => $map)); ?>
      <div class="description">Filter datasets by area shown on the map.</div>
    </div>
  <?php endif; ?>
  <?php if (isset($terms) && count($terms) > 0): ?>
    <div class="categories">
      <h3 class="filter-title">Categories</h3>
      <?php print theme('data_engine_browse_terms', array('name' => 'categories', 'terms' => $terms)); ?>
    </div>
  <?php endif ?>
  <?php if (isset($sources) && count($sources) > 0): ?>
    <div class="sources">
      <h3 class="filter-title">Sources</h3>
      <?php print theme('data_engine_browse_terms', array('name' => 'sources', 'terms' => $sources)); ?>
    </div>
  <?php endif; ?>
  <input id="filter-submit-btn" type="submit" class="button-primary" value="Search" />
</form>

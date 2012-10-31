<div id="page" class="<?php print $classes; ?>">

  <div id="header">
    <div class="page-width">
      <hgroup id="branding">
        <h1>
          <?php if ($logo): ?>
            <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home" id="logo">
              <img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>" />
            </a>
          <?php endif; ?>
          <?php if ($site_name): ?>
            <a href="<?php print $front_page; ?>" rel="home" id="site-name">
             <?php print $site_name; ?>
            </a>
          <?php endif; ?>
        </h1>

        <?php if ($site_slogan): ?>
          <h2 id="site-slogan">
           <?php print $site_slogan; ?>
          </h2>
        <?php endif; ?>
      </hgroup>
      <?php if ($page['utility']): ?>
        <div id="utility">
          <div class="container">
            <?php print render($page['utility']); ?>
          </div>
        </div>
      <?php endif; // end utility ?>
      <?php if ($page['header']): ?>
          <?php print render($page['header']); ?>
      <?php endif; // end header ?>
    </div>
  </div>

  <?php if ($page['navigation']): ?>
    <div id="navigation" class="clearfix"><div class="page-width">
      <?php print render($page['navigation']); ?>
    </div></div>
  <?php endif; // end navigation?>

  <div id="main">
    <div class="page-width">

      <?php if ($messages): ?>
        <div id="messages">
            <?php print $messages; ?>
        </div>
      <?php endif; // end messages ?>

      <?php if ($page['above_content']): ?>
        <div id="above-content">
          <div class="container">
            <?php print render($page['above_content']); ?>
          </div>
        </div>
      <?php endif; // end Above Content ?>

      <div id="main-content" class="clearfix">

        <?php if (render($tabs)): ?>
          <div id="tabs">
            <?php print render($tabs); ?>
          </div>
        <?php endif; // end tabs ?>

        <div id="content">

          <?php if ($page['highlighted']): ?>
            <div id="highlighted">
              <div class="container">
                <?php print render($page['highlighted']); ?>
              </div>
            </div>
          <?php endif; // end highlighted ?>

          <?php if (!$is_front && strlen($title) > 0): ?>
            <h1 <?php print $title_attributes; ?>><?php print $title; ?></h1>
          <?php endif; ?>

          <?php if ($page['help']): ?>
            <div id="help">
              <?php print render($page['help']); ?>
            </div>
          <?php endif; // end help ?>

          <?php print render($page['content']); ?>

        </div>

        <?php if ($page['sidebar_first']): ?>
          <div id="sidebar-first" class="aside">
            <?php print render($page['sidebar_first']); ?>
          </div>
        <?php endif; // end sidebar_first ?>

        <?php if ($page['sidebar_second']): ?>
          <div id="sidebar-second" class="aside">
            <?php print render($page['sidebar_second']); ?>
          </div>
        <?php endif; // end sidebar_second ?>
      </div>

      <?php if ($page['below_content']): ?>
        <div id="below-content">
          <div class="container">
            <?php print render($page['below_content']); ?>
          </div>
        </div>
      <?php endif; // end Below Content ?>

    </div>
  </div>

  <div id="footer">
    <div class="page-width">
      <?php print render($page['footer']); ?>
    </div>
  </div>

  <?php if ($page['admin_footer']): ?>
    <div id="admin-footer">
      <div class="page-width">
        <?php print render($page['admin_footer']); ?>
      </div>
    </div>
  <?php endif; // end admin_footer ?>

</div>

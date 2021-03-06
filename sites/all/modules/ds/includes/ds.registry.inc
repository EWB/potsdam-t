<?php

/**
 * @file
 * Registry functions.
 */

/**
 * Return permissions.
 */
function _ds_ui_perm() {
  $perms = array(
    'access display suite',
    'administer styles',
    'use PHP in custom fields',
    'revert overridden settings',
    'export and import settings',
  );
  foreach (module_implements('ds_api') as $module) {
    $perms[] = 'administer '. $module;
    $perms[] = 'configure layout for '. $module;
  }
  return $perms;
}

/**
 * Return menu items and import default settings.
 */
function _ds_ui_menu() {

  $items = array();
  $all_build_modes = ds_get_build_modes(NULL, TRUE);

  // Display suite main menu block.
  $items[DS_PATH_BASE] = array(
    'title' => 'Display suite',
    'description' => 'Manage your site display.',
    'page callback' => 'ds_general_page',
    'access arguments' => array('access display suite'),
    'file' => 'includes/ds.admin.inc',
  );

  // Layouts
  $items[DS_PATH_LAYOUT] = array(
    'title' => 'Layouts',
    'description' => 'Go to the overview screen of all the objects in your Drupal site to theme.',
    'page callback' => 'ds_layout_overview',
    'access arguments' => array('access display suite'),
    'file' => 'includes/ds.display.inc',
    'weight' => -5,
  );
  $items[DS_PATH_LAYOUT .'/overview'] = array(
      'title' => 'Overview',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => 0,
  );

  // Modules
  $items[DS_PATH_MODULES] = array(
    'title' => 'Module display settings',
    'description' => 'Settings for modules which provide displays.',
    'page callback' => 'ds_module_list',
    'access arguments' => array('access display suite'),
    'file' => 'includes/ds.modules.inc',
    'weight' => 3,
  );

  foreach (module_implements('ds_api') as $module) {
    $api_info = ds_api_info($module);
    $module = $api_info['module'];
    $title = $api_info['title'];

    // Displays.
    $build_modes = $all_build_modes[$module];
    $first_title = '';
    $first_build_mode = key($build_modes);
    if (!empty($build_modes)) {
      foreach ($api_info['types']() as $tkey => $type) {

        $type_name = $type->type;
        $type_url_str = str_replace('_', '-', $type_name);

        // Global exclude.
        if (variable_get($module .'_type_'. $type->type, FALSE)) {
          continue;
        }

        $first = 0;
        $weight = 20;
        $build_modes_found = FALSE;
        $local_task_items = array();
        $exclude_build_modes = variable_get($module .'_buildmodes_exclude', array());
        foreach ($build_modes as $key => $value) {

          // Check if build mode is excluded for this object type.
          $excluded = (isset($exclude_build_modes[$type_name][$key]) && $exclude_build_modes[$type_name][$key] == TRUE) ? TRUE : FALSE;
          if ($excluded) {
            continue;
          }

          $build_modes_found = TRUE;

          // Tabs.
          $local_task_items[DS_PATH_LAYOUT .'/'. $type_url_str .'/'. $key] = array(
            'title' => $value['title'],
            'page callback' => 'drupal_get_form',
            'page arguments' => array('ds_display_overview_form', $type_name, "$key", $module),
            'access arguments' => array('configure layout for '. $module),
            'type' => $first == 0 ? MENU_DEFAULT_LOCAL_TASK : MENU_LOCAL_TASK,
            'file' => 'includes/ds.display.inc',
            'weight' => (isset($value['weight']) && !empty($value['weight'])) ? $value['weight'] : $weight++,
          );

          // Store the first build_mode
          if ($first == 0) {
            $first++;
            $first_title = $type->name;
            $first_build_mode = $key;
          }
        }

        // Default callback.
        if ($build_modes_found) {
          $items[DS_PATH_LAYOUT .'/'. $type_url_str] = array(
            'title' => drupal_ucfirst($first_title),
            'page callback' => 'drupal_get_form',
            'page arguments' => array('ds_display_overview_form', $type_name, "$first_build_mode", $module),
            'access arguments' => array('configure layout for '. $module),
            'file' => 'includes/ds.display.inc',
            'type' => MENU_NORMAL_ITEM,
            'weight' => 2,
          );
          $items += $local_task_items;
        }
      }
    }

    // Info.
    $items[DS_PATH_MODULES .'/'.  $module] = array(
      'title' => $title,
      'description' => 'Find help, configure build modes, fields, fieldgroups, plugins and possibly other settings for '. $title .'.',
      'page callback' => 'ds_info',
      'page arguments' => array($module),
      'access arguments' => array('administer '. $module),
      'file' => 'includes/ds.modules.inc',
    );
    $items[DS_PATH_MODULES .'/'.  $module .'/info'] = array(
      'title' => 'Info',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => 0,
    );

    // Fields.
    $items[DS_PATH_MODULES .'/'.  $module .'/fields'] = array(
      'title' => 'Fields',
      'page callback' => 'ds_fields',
      'page arguments' => array($module),
      'access callback' => '_ds_ui_menu_access',
      'access arguments' => array('administer '. $module, 'use PHP in custom fields'),
      'file' => 'includes/ds.fields.inc',
      'type' => MENU_LOCAL_TASK,
      'weight' => 1,
    );

    // Fields examples.
    if (!isset($api_info['no_fields_examples'])) {
      $items[DS_PATH_MODULES .'/'.  $module .'/fields-examples'] = array(
        'title' => 'Field examples',
        'page callback' => 'ds_fields_examples',
        'page arguments' => array($module),
        'access callback' => '_ds_ui_menu_access',
        'access arguments' => array('administer '. $module, 'use PHP in custom fields'),
        'file' => 'includes/ds.fields.inc',
        'type' => MENU_CALLBACK,
        'weight' => 1,
      );
    }

    // Field groups
    $items[DS_PATH_MODULES .'/'.  $module .'/fieldgroups'] = array(
      'title' => 'Fieldgroups',
      'page callback' => 'ds_fieldgroups',
      'page arguments' => array($module),
      'access arguments' => array('administer '. $module),
      'file' => 'includes/ds.groups.inc',
      'type' => MENU_LOCAL_TASK,
      'weight' => 2,
    );

    // Build modes.
    $items[DS_PATH_MODULES .'/'.  $module .'/buildmodes'] = array(
      'title' => 'Build modes',
      'page callback' => 'ds_build_modes_page',
      'page arguments' => array($module),
      'access arguments' => array('administer '. $module),
      'file' => 'includes/ds.buildmodes.inc',
      'type' => MENU_LOCAL_TASK,
      'weight' => 3,
    );

    // Plugins.
    $items[DS_PATH_MODULES .'/'.  $module .'/plugins'] = array(
      'title' => 'Plugins',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('ds_plugins', $module),
      'access arguments' => array('administer '. $module),
      'file' => 'includes/ds.plugins.inc',
      'type' => MENU_LOCAL_TASK,
      'weight' => 4,
    );
  }

  // Styles
  $items[DS_PATH_STYLES] = array(
    'title' => 'Styles',
    'description' => 'Configure generic CSS class settings for fields and regions.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('ds_styles_form'),
    'access arguments' => array('administer styles'),
    'file' => 'includes/ds.display.inc',
    'weight' => 5,
  );
  
  // Tools
  $items[DS_PATH_TOOLS] = array(
    'title' => 'Tools',
    'description' => 'Import and export settings and get a skeleton of your stylesheet depending on your configuration.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('ds_import'),
    'access arguments' => array('export and import settings'),
    'file' => 'includes/ds.tools.inc',
    'weight' => 10,
  );
  $items[DS_PATH_TOOLS .'/import'] = array(
    'title' => 'Import settings',
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => 0,
  );
  $items[DS_PATH_TOOLS .'/export'] = array(
    'title' => 'Export settings',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('ds_export'),
    'access arguments' => array('export and import settings'),
    'file' => 'includes/ds.tools.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 1,
  );
  $items[DS_PATH_TOOLS .'/revert'] = array(
    'title' => 'Revert',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('ds_revert_form'),
    'access arguments' => array('revert overridden settings'),
    'file' => 'includes/ds.tools.inc',
    'type' => MENU_CALLBACK,
  );

  return $items;
}

/**
 * Return theme functions.
 */
function _ds_theme() {
  $path = drupal_get_path('module', 'ds');

  $theme_functions = array(
    // Regions theming.
    'ds_regions' => array(
      'arguments' => array(
        'object_display' => NULL,
        'module' => '',
      ),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
    ),

    // General theming field.
    'ds_field' => array(
      'arguments' => array(
        'field' => NULL,
      ),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
    ),
    'ds_field_inline' => array(
      'arguments' => array(
        'field' => NULL,
      ),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
    ),
    
    // Evaluate code.
    'ds_eval_code' => array(
      'arguments' => array(
        'field' => NULL,
      ),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
    ),
    // Evaluate block.
    'ds_eval_block' => array(
      'arguments' => array(
        'field' => NULL,
      ),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
    ),

    // DS objects.
    'ds_objects' => array(
      'arguments' => array(
        'data' => array(),
        'extra' => array(),
      ),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
    ),

    // We'll only use one function for the fieldsets.
    'ds_group_fieldset_open' => array(
      'arguments' => array(
        'field' => NULL,
      ),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
      'function' => 'theme_ds_group_fieldset',
    ),
    'ds_group_fieldset_collapsible' => array(
      'arguments' => array(
        'field' => NULL,
      ),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
      'function' => 'theme_ds_group_fieldset',
    ),
    'ds_group_fieldset_collapsed' => array(
      'arguments' => array(
        'field' => NULL,
      ),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
      'function' => 'theme_ds_group_fieldset',
    ),

    // CCK 3.x multigroup subgroup formatters
    'ds_multigroup_div' => array(
      'arguments' => array(
        'group' => NULL,
      ),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
      'function' => 'theme_ds_multigroup_div',
    ),
    'ds_multigroup_fieldset_open' => array(
      'arguments' => array(
        'group' => NULL,
      ),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
      'function' => 'theme_ds_multigroup_fieldset',
    ),
    'ds_multigroup_fieldset_collapsible' => array(
      'arguments' => array(
        'group' => NULL,
      ),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
      'function' => 'theme_ds_multigroup_fieldset',
    ),
    'ds_multigroup_fieldset_collapsed' => array(
      'arguments' => array(
        'group' => NULL,
      ),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
      'function' => 'theme_ds_multigroup_fieldset',
    ),
    'ds_multigroup_table_simple' => array(
      'arguments' => array(
        'group' => NULL,
      ),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
      'function' => 'theme_ds_multigroup_table_simple',
    ),
    'ds_multigroup_table_multiple' => array(
      'arguments' => array(
        'group' => NULL,
      ),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
      'function' => 'theme_ds_multigroup_table_multiple',
    ),
    'ds_multigroup_horizontal_line' => array(
      'arguments' => array(
        'group' => NULL,
      ),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
      'function' => 'theme_ds_multigroup_horizontal_line',
    ),
    'ds_multigroup_list_ol' => array(
      'arguments' => array(
        'group' => NULL,
      ),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
      'function' => 'theme_ds_multigroup_list',
    ),
    'ds_multigroup_list_ul' => array(
      'arguments' => array(
        'group' => NULL,
      ),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
      'function' => 'theme_ds_multigroup_list',
    ),
    'ds_multigroup_comma_separated' => array(
      'arguments' => array(
        'group' => NULL,
      ),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
      'function' => 'theme_ds_multigroup_comma_separated',
    ),
    'ds_multigroup_first_label' => array(
      'arguments' => array(
        'group' => NULL,
      ),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
      'function' => 'theme_ds_multigroup_first_label',
    ),
    'ds_subgroup_label' => array(
      'arguments' => array(
        'label' => NULL,
        'class' => NULL,
      ),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
      'function' => 'theme_ds_subgroup_label',
    ),

    // Theming for fields which are shared across objects (name, picture etc).
    'ds_author_link' => array(
      'arguments' => array('object' => NULL),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
    ),
    'ds_author_nolink' => array(
      'arguments' => array('object' => NULL),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
    ),
    'ds_picture' => array(
      'arguments' => array('object' => NULL),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
    ),
  );

  // Tabs support
  if (module_exists('tabs')) {
    $theme_functions['ds_tabs'] = array(
      'arguments' => array('object' => NULL),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
    );
  }

  return $theme_functions;
}

/**
 * Return theme functions for ds ui.
 */
function _ds_theme_ui() {
  $path = drupal_get_path('module', 'ds');

  $theme_functions = array(
    // Display overview form.
    'ds_display_overview_form' => array(
      'template' => 'ds-display-overview-form',
      'file' => 'theme_ui.inc',
      'path' => $path .'/theme',
      'arguments' => array('form' => NULL),
    ),
    // Display overview form for views
    'ds_display_overview_views_form' => array(
      'template' => 'ds-display-overview-views-form',
      'file' => 'theme_ui.inc',
      'path' => $path .'/theme',
      'arguments' => array('form' => NULL),
    ),
    // Build modes matrix.
    'ds_buildmodes_matrix_form' => array(
      'file' => 'theme_ui.inc',
      'path' => $path .'/theme',
      'arguments' => array('form' => NULL),
    ),
    // Plugins form.
    'ds_plugins' => array(
      'file' => 'theme_ui.inc',
      'path' => $path .'/theme',
      'arguments' => array('form' => NULL),
    ),
  );

  return $theme_functions;
}

/**
 * Register build modes.
 */
function _ds_register_build_modes() {

  $weight = 10;
  $build_modes = array();

  // Build modes defined in hooks.
  foreach (module_implements('content_build_modes') as $module) {

    $module_data = array();
    $function = $module .'_content_build_modes';
    $cck_grouped = call_user_func($function);
    // We need to do some fiddling existing build modes.
    if (!empty($cck_grouped)) {
      foreach ($cck_grouped as $group_key => $group) {
        // Sane build modes.
        foreach ($group['build modes'] as $build_mode_key => $build_mode) {
          // If no module key is set, we are adding it ourselves. This hook only applies
          // to nodes anyway, so yes, we use nd as the $module key.
          $module_name = isset($build_mode['module']) ? $build_mode['module'] : 'nd';
          $build_modes[$module_name][$build_mode_key] = array(
            'weight' => isset($build_mode['weight']) ? $build_mode['weight'] : $weight++,
            'title' => $build_mode['title'],
            'type' => 'code',
            'module' => $module,
            'group' => check_plain($group['title']),
            'overridden' => FALSE,
          );
        }
      }
    }
  }
  $code_modes = $build_modes;
  // Custom build modes via the UI.
  foreach (module_implements('ds_api') as $module) {
    $weight = 10;
    $db_build_modes = variable_get($module .'_build_modes', array());

    if (!empty($db_build_modes)) {
      foreach ($db_build_modes as $key => $name) {
        $build_modes[$module][$key] = array(
          'weight' => $weight++,
          'title' => check_plain($name),
          'build modes' => array(
            $key => array(
              'title' => check_plain($name),
            ),
          ),
          'type' => 'db',
          'module' => isset($code_modes[$module][$key]['module']) && $module != 'nd' ? $code_modes[$module][$key]['module'] : '',
          'group' => t('Custom'),
          'overridden' => isset($code_modes[$module][$key]) ? TRUE : FALSE,
        );
      }
    }
    asort($build_modes[$module]);
  }

  // Give modules a change to alter build_modes.
  drupal_alter('ds_buildmodes', $build_modes);

  // Cache all build modes.
  variable_set('ds_all_build_modes', $build_modes);

  return $build_modes;
}

/**
 * Return blocks.
 */
function _ds_block_list() {
  $blocks = array();
  $all_blocks = array();

  foreach (module_implements('ds_api') as $module) {
    $plugins = variable_get($module .'_plugin_settings', array());
    if (!empty($plugins)) {
      foreach ($plugins as $key => $data) {
        if (isset($data['filename']) && isset($data['class'])) {
          require_once($data['filename']);
          $class = $data['class'];
          $plugin = new $class();
          if (method_exists($plugin, 'block_list')) {
            $plugin->block_list($module, $all_blocks, $data);
          }
        }
      }
    }
  }

  if (!empty($all_blocks)) {
    $i = 0;
    $ds_blocks = array();
    foreach ($all_blocks as $key => $block) {
      if (isset($block['info']) && isset($block['key']) && isset($block['data'])) {
        // Calculating the md5 from the key is effective here, because we'll
        // get back an int which is exactly 32 characters long, the length
        // of the delta field of the blocks table.
        $hash = md5($block['key']);
        $blocks[$hash] = array('info' => $block['info']);
        $ds_blocks[$hash] = $block;
      }
    }
    variable_set('ds_blocks', $ds_blocks);
  }

  return $blocks;
}

/**
 * Return ds plugins.
 */
function _ds_plugins() {
  $path = drupal_get_path('module', 'ds');
  return array(
    'emptyregionrender' => array(
      'title' => t('Empty region'),
      'class' => 'ds_emptyregionrender',
      'description' => t('Renders a region when there is no content in it.'),
      'file' => 'ds_emptyregionrender.inc',
      'path' => $path .'/plugins',
      'target' => 'all',
    ),
    'cssoverrider' => array(
      'title' => t('Simple CSS overrider'),
      'class' => 'ds_cssoverrider',
      'description' => t('Override region CSS with inline styles or add extra classes.'),
      'file' => 'ds_cssoverrider.inc',
      'path' => $path .'/plugins',
      'target' => 'all',
    ),
    'regiontoblock' => array(
      'title' => t('Region to block'),
      'class' => 'ds_regiontoblock',
      'description' => t('Render a display suite region into a theme region. That region will be made available as a block.'),
      'file' => 'ds_regiontoblock.inc',
      'path' => $path .'/plugins',
      'target' => 'all',
    ),
  );
}

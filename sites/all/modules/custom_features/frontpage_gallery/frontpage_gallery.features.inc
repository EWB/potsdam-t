<?php

/**
 * Implementation of hook_imagecache_default_presets().
 */
function frontpage_gallery_imagecache_default_presets() {
  $items = array(
    'large_480_sharpened' => array(
      'presetname' => 'large_480_sharpened',
      'actions' => array(
        '0' => array(
          'weight' => '0',
          'module' => 'imagecache',
          'action' => 'imagecache_scale',
          'data' => array(
            'width' => '480',
            'height' => '',
            'upscale' => 0,
          ),
        ),
        '1' => array(
          'weight' => '0',
          'module' => 'imagecache',
          'action' => 'imagecache_sharpen',
          'data' => array(
            'radius' => '0.5',
            'sigma' => '0.5',
            'amount' => '100',
            'threshold' => '0.05',
          ),
        ),
      ),
    ),
  );
  return $items;
}

/**
 * Implementation of hook_node_info().
 */
function frontpage_gallery_node_info() {
  $items = array(
    'banner' => array(
      'name' => t('Banner'),
      'module' => 'features',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Title'),
      'has_body' => '1',
      'body_label' => t('Body'),
      'min_word_count' => '0',
      'help' => '',
    ),
  );
  return $items;
}

/**
 * Implementation of hook_views_api().
 */
function frontpage_gallery_views_api() {
  return array(
    'api' => '2',
  );
}

<?php

/**
 * Implementation of hook_ds_default_settings().
 */
function event_feature_ds_default_settings() {
  $data = array(
    'nd' => array(
      'event' => array(
        'full' => array(
          'fields' => array(
            'field_event_location' => array(
              'region' => 'right',
              'weight' => '-48',
              'format' => 'default',
              'css-class' => '',
              'label_value' => 'Location',
              'labelformat' => 'inline',
            ),
            'field_event_image' => array(
              'region' => 'right',
              'weight' => '-46',
              'format' => 'imagefield__lightbox2__small_220x220_sharpened__large_480x480_sharpened',
              'css-class' => '',
              'labelformat' => 'hidden',
            ),
            'field_event_date' => array(
              'region' => 'right',
              'weight' => '-47',
              'format' => 'short',
              'css-class' => '',
              'label_value' => 'When',
              'labelformat' => 'inline',
            ),
            'title' => array(
              'region' => 'disabled',
              'weight' => '-41',
              'format' => 'nd_title_h1_nolink',
              'css-class' => '',
              'labelformat' => 'hidden',
            ),
            'author' => array(
              'region' => 'header',
              'weight' => '-41',
              'format' => 'ds_author_nolink',
              'css-class' => '',
              'label_value' => 'Contributed by',
              'labelformat' => 'inline',
            ),
            'links' => array(
              'region' => 'disabled',
              'weight' => '-42',
              'format' => '',
              'css-class' => '',
              'labelformat' => 'hidden',
            ),
            'read_more' => array(
              'region' => 'disabled',
              'weight' => '-45',
              'format' => 'ds_eval_code',
              'css-class' => '',
              'labelformat' => 'hidden',
            ),
            'post_date' => array(
              'region' => 'disabled',
              'weight' => '-44',
              'format' => 'nd_post_date_long',
              'css-class' => '',
              'labelformat' => 'hidden',
            ),
            'body' => array(
              'region' => 'middle',
              'weight' => '-49',
              'format' => 'nd_bodyfield',
              'css-class' => '',
              'labelformat' => 'hidden',
            ),
            'terms_1' => array(
              'region' => 'header',
              'weight' => '-50',
              'format' => 'nd_terms_per_vocabulary_comma',
              'css-class' => '',
              'label_value' => 'Tags',
              'labelformat' => 'inline',
            ),
            'terms' => array(
              'region' => 'disabled',
              'weight' => '-43',
              'format' => '',
              'css-class' => '',
              'labelformat' => 'hidden',
            ),
          ),
          'status' => 2,
          'region_styles' => array(
            'header' => '',
            'left' => '',
            'middle' => '',
            'right' => '',
            'footer' => '',
          ),
        ),
      ),
    ),
  );
  return $data;
}

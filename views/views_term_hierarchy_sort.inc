<?php

class views_term_hierarchy_sort extends views_handler_sort {

  var $term_weights = array();

  function can_expose() { return FALSE; }

  function query() {}

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['order']['#access'] = FALSE;
  }

  function post_execute(&$values) {
    $vocabularies = array();
    foreach ($values as $value) {
      if (isset($value->taxonomy_term_data_vocabulary)) {
        $vocabulary = $value->taxonomy_term_data_vocabulary;
        if (!isset($vocabularies[$vocabulary])) {
          $vocabularies[$vocabulary] = $vocabulary;
        }
      }
    }
    if (empty($vocabularies)) {
      return;
    }

    $term_trees = array();

    $this->term_weights = array();

    $vocabularies_by_name = taxonomy_vocabulary_load_multiple(FALSE);
    foreach ($vocabularies as $vocabulary) {
      $term_trees[$vocabulary] = taxonomy_get_tree($vocabularies_by_name[$vocabulary]->machine_name);
      foreach ($term_trees[$vocabulary] as $weight => $term) {
        $this->term_weights[$term->tid] = $weight;
      }
    }
    usort($values, array($this, 'hierarchy_sort_cmp'));
  }

  function hierarchy_sort_cmp($a, $b) {
    $a_tid = $a->tid;
    $b_tid = $b->tid;
    if ($a_tid === $b_tid) {
      return 0;
    }
    $a_weight = $a_tid;
    $b_weight = $b_tid;
    if ((isset($this->term_weights[$a_tid])) && (isset($this->term_weights[$b_tid]))) {
      $a_weight = $this->term_weights[$a_tid];
      $b_weight = $this->term_weights[$b_tid];
    }
    if ($a_weight > $b_weight) {
      return 1;
    }
    if ($a_weight < $b_weight) {
      return -1;
    }
    return 0;
  }

}

#pragma once

#include "common.h"

struct heap_node {
  struct heap_node *left;
  struct heap_node *right;
  struct heap_node *parent;
};

typedef int (*heap_comparator_t)(struct heap_node *a, struct heap_node *b);

struct heap {
  struct heap_node *root;
  size_t size;
  heap_comparator_t compare_fn;
};

typedef struct heap heap_t;

static struct heap heap_init(heap_comparator_t compare_fn) {
  struct heap h;
  h.root = NULL;
  h.size = 0;
  h.compare_fn = compare_fn;
  return h;
}

static void heap_init_node(struct heap_node *node) {
  node->left = NULL;
  node->right = NULL;
  node->parent = NULL;
}

void swap_arbitrary_nodes(struct heap *h, struct heap_node *a, struct heap_node *b) {
  struct heap_node tmp_a = *a;
  struct heap_node tmp_b = *b;

  if (a->parent) {
    if (a->parent->left == a) {
      a->parent->left = b;
    } else {
      a->parent->right = b;
    }
  } else {
    h->root = b;
  }

  if (b->parent) {
    if (b->parent->left == b) {
      b->parent->left = a;
    } else {
      b->parent->right = a;
    }
  } else {
    h->root = a;
  }

  if (a->left) 
    a->left->parent = b;
  if (a->right) 
    a->right->parent = b;

  if (b->left) 
    b->left->parent = a;
  if (b->right) 
    b->right->parent = a;

  struct heap_node temp = *a;
  *a = *b;
  *b = temp;

  a->parent = tmp_b.parent;
  b->parent = tmp_a.parent;

  a->left = tmp_b.left;
  a->right = tmp_b.right;
  b->left = tmp_a.left;
  b->right = tmp_a.right;
}

static void swap_nodes(struct heap *h, struct heap_node *parent, struct heap_node *child) {
  struct heap_node tmp = *child;

  child->parent = parent->parent;
  if (parent->left == child) {
    child->left = parent;
    child->right = parent->right;
    if (parent->right)
      parent->right->parent = child;
  } else {
    child->right = parent;
    child->left = parent->left;
    if (parent->left)
      parent->left->parent = child;
  }

  parent->parent = child;
  parent->left = tmp.left;
  parent->right = tmp.right;

  if (parent->left)
    parent->left->parent = parent;

  if (parent->right)
    parent->right->parent = parent;

  if (child->parent) {
    if (child->parent->left == parent) {
      child->parent->left = child;
    } else {
      child->parent->right = child;
    }
  } else {
    h->root = child;
  }
}

static void heapify_up(struct heap *h, struct heap_node *node) {
    while (node->parent != NULL && h->compare_fn(node, node->parent) > 0) {
        swap_nodes(h, node->parent, node);
    }
}

static void heap_insert(struct heap *h, struct heap_node *newnode) {
  if (h->root == NULL) {
    h->root = newnode;
    h->size++;
    return;
  }

  struct heap_node *last = h->root;

  unsigned n = h->size + 1;
  unsigned mask = 0;
  unsigned k = 0; // bits count

  // with each iteration we divide n by 2 and check if n is odd or even
  while(n >= 2) {
    mask = (mask << 1) | (n & 1);
    k++;
    n >>= 1; // same as n /= 2
  }

  while (k > 1) {
    if (mask & 1) {
      last = last->right;
    } else {
      last = last->left;
    }

    k--;
    mask >>= 1;
  }

  if (last->left == NULL) {
    last->left = newnode;
  } else {
    last->right = newnode;
  }

  newnode->parent = last;
  newnode->left = NULL;
  newnode->right = NULL;
  h->size++;

  heapify_up(h, newnode);
}

static struct heap_node *heap_remove(struct heap *h, struct heap_node *rnode) {
  if (h->root == NULL) {
      return NULL;
  }

  struct heap_node *node = h->root;
  struct heap_node *root = rnode;

  unsigned n = h->size;
  unsigned mask = 0;
  unsigned k = 0; // bits count

    // with each iteration we divide n by 2 and check if n is odd or even
  while (n >= 2) {
      mask = (mask << 1) | (n & 1);
      k++;
      n >>= 1; // same as n /= 2
  }

  while (k > 0) {
      if (mask & 1) {
          node = node->right;
      } else {
          node = node->left;
      }

      k--;
      mask >>= 1;
  }

  // Swap the root with the last node
  if (node->parent == rnode) {
    swap_nodes(h, root, node);
  } else {
    swap_arbitrary_nodes(h, node, root);
  }

  // After the swap, the last node is now the new root and needs to be detached
  if (root->parent->left == root) {
    root->parent->left = NULL;
  } else {
    root->parent->right = NULL;
  }

  h->size--;

  struct heap_node *last = node;
  node = root;
  root = h->root;

  while (true) {
    // if comparator > 0 we have to swap parent with smallest child
    bool swap_left = true;
    if (root->left && root->right) {
      swap_left = h->compare_fn(root->left, root->right) > 0;
    }

    if (root->left && h->compare_fn(root, root->left) <= 0 && swap_left) {
      swap_nodes(h, root, root->left);
    }
    else if (root->right && h->compare_fn(root, root->right) <= 0) {
      swap_nodes(h, root, root->right);
    } else {
      break;
    }
  }

  heapify_up(h, last);

  return node;
}

static struct heap_node *heap_pop(struct heap *h) {
  return heap_remove(h, h->root);
} 

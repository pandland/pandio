#include <stdlib.h>
#include <stdbool.h>

#define to_int(void_x) *(int*)(void_x)

struct heap_node {
  struct heap_node *left;
  struct heap_node *right;
  struct heap_node *parent;
  void *data;
};

// if comparator returns value > 0 - it will swap a with b
typedef int (*heap_comparator_t)(struct heap_node *a, struct heap_node *b);

struct heap {
  struct heap_node *root;
  size_t size;
  heap_comparator_t compare_fn;
};

void print_heap(struct heap*);

struct heap init_heap(heap_comparator_t compare_fn) {
  struct heap h;
  h.root = NULL;
  h.size = 0;
  h.compare_fn = compare_fn;
  return h;
}

struct heap_node create_heap_node(void *data) {
  struct heap_node new_node;
  new_node.left = NULL;
  new_node.right = NULL;
  new_node.parent = NULL;
  new_node.data = data;

  return new_node;
}

struct heap_node *swap_heap_nodes(
  struct heap_node *parent, 
  struct heap_node *child
) {
  void *tmp = child->data;

  child->data = parent->data;
  parent->data = tmp;

  return parent;
}

void heap_insert(struct heap *h, struct heap_node *new_node) {
  if (h->root == NULL) {
    h->root = new_node;
    h->size++;
    return;
  }

  struct heap_node *parent = h->root;
  unsigned n = h->size + 1; // heap size + node we are inserting
  unsigned path = 0; // each bit represents direction: 0 -> left; 1 -> right
  unsigned k = 0; // bits count in the path

  // with each iteration we divide n by 2 and check if n is odd or even
  while(n >= 2) {
    path = (path << 1) | (n & 1);
    k++;
    n >>= 1; // same as n /= 2
  }

  // k > 1, because we want to find parent of the insterted node
  while (k > 1) {
    if (path & 1) {
      parent = parent->right;
    } else {
      parent = parent->left;
    }

    k--;
    path >>= 1;
  }

  new_node->parent = parent;
  new_node->left = NULL;
  new_node->right = NULL;

  if (parent->left == NULL)
    parent->left = new_node;
  else if (parent->right == NULL)
    parent->right = new_node;

  h->size++;

  struct heap_node *child = new_node;
  while (child->parent != NULL && h->compare_fn(parent, child) > 0) {
    child = swap_heap_nodes(parent, child);
    parent = child->parent;
  }
}

struct heap_node *heap_pop(struct heap *h) {
  if (h->root == NULL) {
    return NULL;
  }

  struct heap_node *root = h->root;
  struct heap_node *node = h->root;

  if (h->size == 1) {
    h->root = NULL;
    h->size--;
    return node;
  }

  unsigned n = h->size;
  unsigned path = 0;
  unsigned k = 0;

  while(n >= 2) {
    path = (path << 1) | (n & 1);
    k++;
    n >>= 1; // same as n /= 2
  }

  while (k > 0) {
    if (path & 1) {
      node = node->right;
    } else {
      node = node->left;
    }

    k--;
    path >>= 1;
  }

  swap_heap_nodes(h->root, node);
  
  // unlink popped node
  if (node->parent->left == node) {
    node->parent->left = NULL;
  } else {
    node->parent->right = NULL;
  }

  h->size--;

  while (true) {
    // if comparator > 0 we have to swap parent with smallest child
    bool swap_left = true;
    if (root->left && root->right) {
      swap_left = h->compare_fn(root->left, root->right) <= 0;
    }

    if (root->left && h->compare_fn(root, root->left) > 0 && swap_left) {
      root = swap_heap_nodes(root->left, root);
    }
    else if (root->right && h->compare_fn(root, root->right) > 0) {
      root = swap_heap_nodes(root->right, root);
    } else {
      break;
    }
  }

  return node;
}


void print_heap_until(struct heap_node *node, int space) {
    if (node == NULL) {
      return;
    }
    int count = 20;
    space += count;
    print_heap_until(node->right, space);
    printf("\n");
    for (int i = count; i < space; i++)
        printf(" ");
    printf("%d\n", to_int(node->data));
    print_heap_until(node->left, space);
}

void print_heap(struct heap *h) {
  if (h->root == NULL) {
    return;
  }

  print_heap_until(h->root, 0);
}

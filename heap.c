#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct heap_node {
  struct heap_node *left;
  struct heap_node *right;
  struct heap_node *parent;
  void *data;
};

// if comparator returns value > 0 - it will swap parent and child
typedef int (*heap_comparator_t)(struct heap_node *parent, struct heap_node *child);

struct heap {
  struct heap_node *root;
  size_t size;
  heap_comparator_t compare_fn;
};

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

#define to_int(void_x) *(int*)(void_x)

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

/* ========= TESTING HEAP DS ========== */

int test_compare(struct heap_node *parent, struct heap_node *child) {
  int parent_value = to_int(parent->data);
  int child_value = to_int(child->data);
  if (parent_value > child_value) {
    printf("Swapping %d with %d\n", parent_value, child_value);
    return 1;
  }

  printf("%d vs %d\n", parent_value, child_value);
  return 0;
}

int main() {
  printf("Init heap...\n");
  struct heap h = init_heap(test_compare);
  printf("Creating heap nodes...\n");

  struct heap_node nodes[100];

  for (int i = 0; i < 16; ++i) {
    int *val = malloc(sizeof(int));
    *val = 200 - i;
    nodes[i] = create_heap_node(val);
    heap_insert(&h, &(nodes[i]));
  }

  int min = 2;
  struct heap_node node = create_heap_node(&min);
  heap_insert(&h, &node);

  printf("Printing heap nodes. Heap size: %ld\n", h.size);
  print_heap(&h);
}

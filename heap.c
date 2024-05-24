#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct heap_node {
  struct heap_node *left;
  struct heap_node *right;
  struct heap_node *parent;
  void *data;
};

typedef int (*heap_comparator_t)(struct heap_node *parent, struct heap_node *child);

struct heap {
  struct heap_node *root;
  size_t size;
  heap_comparator_t compare_fn;
};

struct heap init_heap() {
  struct heap h;
  h.root = NULL;
  h.size = 0;
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

void heap_insert(struct heap *h, struct heap_node *new_node) {
  if (h->root == NULL) {
    h->root = new_node;
    h->size++;
    return;
  }

  struct heap_node *parent = h->root;
  unsigned n = h->size+1; // heap size + inserted one
  unsigned path = 0; // each bit represents direction: 0 -> left; 1 -> right
  unsigned k = 0; // bits count in the path

  // with each iteration we divide n and check if n is odd or even
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

  if (parent->left == NULL) {
    new_node->parent = parent;
    parent->left = new_node;
    h->size++;
    
    return;
  }

  if (parent->right == NULL) {
    new_node->parent = parent;
    parent->right = new_node;
    h->size++;
    
    return;
  }

  // TODO: bubbling
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
    printf("%p\n", node->data);
    print_heap_until(node->left, space);
}

void print_heap(struct heap *h) {
  if (h->root == NULL) {
    return;
  }

  print_heap_until(h->root, 0);
}

int main() {
  printf("Init heap...\n");
  struct heap h = init_heap();
  printf("Creating heap nodes...\n");

  struct heap_node nodes[100];

  int val = 10;

  for (int i = 0; i < 5; ++i) {
    nodes[i] = create_heap_node(&val);
    heap_insert(&h, &(nodes[i]));
  } 

  printf("Printing heap nodes. Heap size: %ld\n", h.size);
  print_heap(&h);
}

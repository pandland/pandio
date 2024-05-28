#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct llist {
  struct lnode *head;
  struct lnode *tail;
};

struct lnode {
  struct lnode *next;
  void *data;
};

struct llist init_llist() {
  struct llist l;

  l.head = NULL;
  l.tail = NULL;
}

bool llist_empty(struct llist *l) {
  return l->head == NULL;
}

void llist_add(struct llist *l, struct lnode *node) {
  node->next = NULL;
  
  if (llist_empty(l)) {
    l->head = node;
    l->tail = node;
    return;
  }

  l->tail->next = node;
  l->tail = node;
}

struct lnode *llist_remove_last(struct llist *l) {
  if (l->head == NULL || l->tail == NULL) {
    return NULL;
  }

  struct lnode *it = l->head;
  struct lnode *node = NULL;

  if (l->head == l->tail) {
    node = l->tail;
    l->head = NULL;
    l->tail = NULL;
    return node;
  }

  while (it->next != l->tail) {
    it = it->next;
  }

  node = l->tail;
  it->next = NULL;
  l->tail = it;

  return node;
}

struct lnode *llist_remove(struct llist *l, struct lnode *node) {
  if (l->head == NULL) {
    return NULL;
  }
  
  if (node == l->head && node == l->tail) {
    l->head = NULL;
    l->tail = NULL;

    return node;
  }

  // O(n) removal
  if (node->next == NULL) {
    return llist_remove_last(l);
  }

  // O(1) removal
  void *old = node->data;
  struct lnode *deleted = node->next;
  node->data = deleted->data;
  node->next = deleted->next;

  deleted->data = old;
  return deleted;
}

typedef bool llist_find_t(void* data, void *search);

struct lnode *llist_find(struct llist *l, void *search, llist_find_t find_fn) {
  struct lnode *node = l->head;
  while (node != NULL) {
    if (find_fn(node->data, search)) {
      return node;
    }

    node = node->next;
  }

  return NULL;
}

struct lnode create_node(void *data) {
  struct lnode node;
  node.data = data;
  node.next = NULL;

  return node;
}

// printing purpose only:
typedef void llist_cb(void*);

void llist_iter(struct llist *l, llist_cb cb) {
  struct lnode *node = l->head;
  while (node != NULL) {
    cb(node->data);
    node = node->next;
  }
}

#define to_int(void_x) *(int*)(void_x)

void print_value(void *data) {
  printf("%d -> ", to_int(data));
}

bool find(void *data, void *search) {
  return to_int(data) == to_int(search);
}

int main() {
  struct llist list = init_llist();
  
  /*
  for (int i = 0; i < 20; ++i) {
    int *val = malloc(sizeof(int));
    *val = i;
    struct lnode node = create_node(val);
    llist_add(&list, &node);
  }
  */
  int v1 = 1;
  struct lnode n1 = create_node(&v1);
  llist_add(&list, &n1);

  int v2 = 2;
  struct lnode n2 = create_node(&v2);
  llist_add(&list, &n2);

  int v3 = 3;
  struct lnode n3 = create_node(&v3);
  llist_add(&list, &n3);

  int val = 20;
  struct lnode n4 = create_node(&val);
  llist_add(&list, &n4);

  struct lnode n5 = create_node(&val);
  llist_add(&list, &n5);

  llist_iter(&list, print_value);
  printf("(end)\n");

  struct lnode *deleted = llist_remove(&list, &n2);
  printf("Deleted item value: %d\n", to_int(deleted->data));
  
  llist_iter(&list, print_value);
  printf("(end)\n");

  int x = 2;
  struct lnode *found = llist_find(&list, &x, find);
  if (found != NULL) {
    printf("Found item: %d\n", to_int(found->data));
  } else {
    printf("Item not found\n");
  }
}

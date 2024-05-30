#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct llist {
  struct lnode *head;
  struct lnode *tail;
  size_t size;
};

struct lnode {
  struct lnode *next;
  void *data;
};

struct llist llist_init() {
  struct llist l;

  l.head = NULL;
  l.tail = NULL;
  l.size = 0;

  return l;
}

struct llist *llist_malloc() {
  struct llist *l = malloc(sizeof(struct llist));
  l->head = NULL;
  l->tail = NULL;
  l->size = 0;

  return l;
}

bool llist_empty(struct llist *l) {
  return l->head == NULL;
}

void llist_add(struct llist *l, struct lnode *node) {
  node->next = NULL;
  if (llist_empty(l)) {
    l->head = node;
  } else {
    l->tail->next = node;
  }

  l->tail = node;
  l->size++;
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
    l->size = 0;
    return node;
  }

  while (it->next != l->tail) {
    it = it->next;
  }

  node = l->tail;
  it->next = NULL;
  l->tail = it;
  l->size--;

  return node;
}

struct lnode *llist_remove(struct llist *l, struct lnode *node) {
  if (l->head == NULL) {
    return NULL;
  }
  
  if (node == l->head && node == l->tail) {
    l->head = NULL;
    l->tail = NULL;
    l->size = 0;
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
  l->size--;
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

struct lnode lnode_create(void *data) {
  struct lnode node;
  node.data = data;
  node.next = NULL;

  return node;
}

struct lnode *lnode_malloc(void *data) {
  struct lnode *node = malloc(sizeof(struct lnode));
  node->data = data;
  node->next = NULL;

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

void llist_print(struct llist *l) {
  printf("Size: %ld >>> ", l->size);
  llist_iter(l, print_value);
  printf("(end)\n");
}

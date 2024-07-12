#pragma once
#include "common.h"

typedef bool llist_find_t(const void *data, const void *search);

struct llist_node {
    struct llist_node *next;
    struct llist_node *prev;
};

struct llist {
    struct llist_node *head;
    struct llist_node *tail;
    size_t size;
};

static struct llist *llist_alloc() {
    struct llist *l = malloc(sizeof(struct llist));
    l->head = NULL;
    l->tail = NULL;
    l->size = 0;
    return l;
}

static struct llist llist_create() {
    struct llist l;
    l.head = NULL;
    l.tail = NULL;
    l.size = 0;
    return l;
}

static void llist_init_node(struct llist_node *node) {
    node->next = NULL;
    node->prev = NULL;
}

static bool llist_empty(struct llist *l) {
    return l->head == NULL;
}

static void llist_add(struct llist *l, struct llist_node *node) {
    if (llist_empty(l)) {
        l->head = node;
    } else {
        l->tail->next = node;
        node->prev = l->tail;
    }
    l->tail = node;
    l->size++;
}


static void llist_remove(struct llist *l, struct llist_node *node) {
    if (l->head == NULL) {
        return;
    }

    l->size--;
    if (node == l->head && node == l->tail) {
        l->head = NULL;
        l->tail = NULL;
        l->size = 0;
        return;
    }

    if (node->next == NULL) {
        node->prev->next = NULL;
        l->tail = node->prev;
        return;
    }

    if (l->head == node) {
        l->head = node->next;
        l->head->prev = NULL;
        return;
    }

    node->prev->next = node->next;
    node->next->prev = node->prev;
}

static struct llist_node *llist_find(struct llist *l, const void *search, llist_find_t compare) {
    struct llist_node *node = l->head;
    while (node != NULL) {
        if (compare(node, search)) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

typedef void llist_cb(void *);

static void llist_foreach(struct llist *l, llist_cb cb) {
    struct llist_node *node = l->head;
    while (node != NULL) {
        cb(node);
        node = node->next;
    }
}

#define llist_loop(list) \
  for (struct llist_node *node = list->head; node != NULL; node = node->next)

static void llist_free(struct llist *l) {
  free(l);
}

#include "def.h"

typedef bool llist_find_t(void *data, void *search);

struct llist_node {
    struct llist_node *next;
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

void llist_init_node(struct llist_node *node) {
    node->next = NULL;
}

static bool llist_empty(struct llist *l) {
    return l->head == NULL;
}

void llist_add(struct llist *l, struct llist_node *node) {
    if (llist_empty(l)) {
        l->head = node;
    } else {
        l->tail->next = node;
    }
    l->tail = node;
    l->size++;
}

static void llist_remove_last(struct llist *l) {
    if (l->head == NULL || l->tail == NULL) {
        return;
    }

    struct llist_node *it = l->head;
    struct llist_node *node = NULL;

    if (l->head == l->tail) {
        node = l->tail;
        l->head = NULL;
        l->tail = NULL;
        l->size = 0;
        return;
    }

    while (it->next != l->tail) {
        it = it->next;
    }

    node = l->tail;
    it->next = NULL;
    l->tail = it;
    l->size--;
}

static void llist_remove(struct llist *l, struct llist_node *node) {
    if (l->head == NULL) {
        return;
    }

    if (node == l->head && node == l->tail) {
        l->head = NULL;
        l->tail = NULL;
        l->size = 0;
        return;
    }

    // O(n) removal
    if (node->next == NULL) {
        llist_remove_last(l);
        return;
    }

    // O(1) removal
    struct llist_node *deleted = node->next;
    node->next = deleted->next;
    l->size--;
}

static struct llist_node *llist_find(struct llist *l, void *search, llist_find_t compare) {
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

void llist_foreach(struct llist *l, llist_cb cb) {
    struct llist_node *node = l->head;
    while (node != NULL) {
        cb(node);
        node = node->next;
    }
}

#define llist_loop(list) \
  for (struct llist_node *node = list->head; node != NULL; node = node->next)

void llist_free(struct llist *l) {
  free(l);
}

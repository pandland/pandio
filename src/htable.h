#pragma once

#include <string.h>
#include <stdio.h>
#include "common.h"
#include "llist.h"

#define HTABLE_CAPACITY 64

struct htable_entry {
    char *key;
    void *value;
    struct llist_node lnode;
};

struct htable {
    struct llist **buckets;
    size_t capacity;
};

typedef struct htable htable_t;

// djb2 hash algorithm - object to change
static int htable_hash(struct htable *h, const char *key) {
    unsigned long hash = 5381;
    int c;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash % h->capacity;
}

static struct htable htable_create() {
  struct htable h;
  h.buckets = calloc(HTABLE_CAPACITY, sizeof(struct llist *));
  h.capacity = HTABLE_CAPACITY;

  return h;
}

static struct htable *htable_alloc() {
    struct htable *h = malloc(sizeof(struct htable));
    h->buckets = calloc(HTABLE_CAPACITY, sizeof(struct llist *));
    h->capacity = HTABLE_CAPACITY;
    return h;
}

static struct htable_entry *htable_create_entry(char *key, char *value) {
    struct htable_entry *entry = malloc(sizeof(struct htable_entry));
    entry->key = key;
    entry->value = value;
    llist_init_node(&entry->lnode);
    return entry;
}

/* moves key ownership to the hash table */
static void htable_insert(struct htable *h, char *key, char *value) {
    int idx = htable_hash(h, key);
    struct htable_entry *entry = htable_create_entry(key, value);
    struct llist *bucket = h->buckets[idx];

    if (!bucket) {
        struct llist *new_bucket = llist_alloc();
        h->buckets[idx] = new_bucket;
        bucket = h->buckets[idx];
    }

    llist_add(bucket, &entry->lnode);
}

static struct htable_entry *htable_get_entry(const struct llist_node *node) {
  return container_of(node, struct htable_entry, lnode);
}

static bool htable_filter_bucket(const void *entry_ptr, const void *key_ptr) {
    struct htable_entry *entry = htable_get_entry(entry_ptr);
    char *key = (char *)key_ptr;

    return strcmp(entry->key, key) == 0;
}

static void *htable_get(struct htable *h, const char *key) {
    int idx = htable_hash(h, key);
    struct llist *bucket = h->buckets[idx];

    if (!bucket) {
        return NULL;
    }

    struct llist_node *node = llist_find(bucket, key, htable_filter_bucket);
    if (!node) {
        return NULL;
    }

    struct htable_entry *entry = htable_get_entry(node);
    return entry->value;
}

static void htable_free(struct htable *h) {
    for (size_t i = 0; i < h->capacity; ++i) {
        struct llist *bucket = h->buckets[i];
        if (bucket) {
            llist_loop(bucket) {
              struct htable_entry *entry = htable_get_entry(node);
              free(entry->key);
              // TODO: provide some callback to decide what to do with data pointer
              free(entry);
            }
            llist_free(bucket);
        }
    }
    free(h->buckets);
    free(h);
}

static void htable_entry_print(void *node) {
  struct htable_entry *entry = htable_get_entry(node);

  printf("  \"%s\": (%p),\n", entry->key, entry->value);
}

static void htable_print(struct htable *h) {
  printf("{\n");
  for (int i = 0; i < h->capacity; ++i) {
    struct llist *bucket = h->buckets[i];
    if (!bucket) {
      continue;
    }
    llist_foreach(bucket, htable_entry_print);
  }
  printf("}\n");
}

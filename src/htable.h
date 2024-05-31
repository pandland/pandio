#include <string.h>
#include "def.h"
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

// djb2 hash algorithm - object to change
int htable_hash(struct htable *h, char *key) {
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

void htable_insert(struct htable *h, char *key, char *value) {
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

static struct htable_entry *htable_get_entry(struct llist_node *node) {
  return container_of(node, struct htable_entry, lnode);
}

bool htable_filter_bucket(void *entry_ptr, void *key_ptr) {
    struct htable_entry *entry = htable_get_entry(entry_ptr);
    char *key = (char *)key_ptr;

    return strcmp(entry->key, key) == 0;
}

const void *htable_get(struct htable *h, char *key) {
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

void htable_free(struct htable *h) {
    for (size_t i = 0; i < h->capacity; ++i) {
        struct llist *bucket = h->buckets[i];
        if (bucket) {
            llist_loop(bucket) {
              struct htable_entry *entry = htable_get_entry(node);
              free(entry);
            }
            llist_free(bucket);
        }
    }
    free(h->buckets);
    free(h);
}

void htable_entry_print(void *node) {
  struct htable_entry *entry = htable_get_entry(node);

  printf("  \"%s\": (%p),\n", entry->key, entry->value);
}

void htable_print(struct htable *h) {
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


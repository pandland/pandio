#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "llist.c"

#define MAX_BUCKETS_SIZE 64

struct htable_entry {
  const char  *key;
  const void *data;
};

struct htable {
  struct llist **buckets;
  size_t size;
};

// djb2 hash algoritim - object to change
int htable_hash(struct htable *h, const char *key) {
  unsigned long hash = 5381;
  int c;

  while (c = *key++)
      hash = ((hash << 5) + hash) + c;

  return hash % h->size;
}

struct htable htable_init() {
  struct htable h;
  h.buckets = calloc(MAX_BUCKETS_SIZE, sizeof(struct llist));
  h.size = MAX_BUCKETS_SIZE;

  return h;
}

struct htable_entry htable_entry_init(const char *key, const void *data) {
  struct htable_entry entry;
  entry.key = key;
  entry.data = data;

  return entry;
}

struct htable_entry *htable_entry_malloc(const char *key, const void *data) {
  struct htable_entry *entry = malloc(sizeof(struct htable_entry));
  entry->key = key;
  entry->data = data;

  return entry;
}

void htable_insert(struct htable *h, const char *key, const void *data) {
  int idx = htable_hash(h, key);
  struct htable_entry *entry = htable_entry_malloc(key, data);
  struct llist *bucket = h->buckets[idx];

  if (!bucket) {
    struct llist *new_bucket = llist_malloc();
    h->buckets[idx] = new_bucket;
    bucket = h->buckets[idx];
  }

  struct lnode *node = lnode_malloc(entry);
  llist_add(bucket, node);
}

bool filter_bucket(void *entry_ptr, void *key_ptr) {
  struct htable_entry *entry =  (struct htable_entry *)entry_ptr;
  char *key = (char *)key_ptr;

  return strcmp(entry->key, key) == 0;
}

const void *htable_get(struct htable *h, char *key) {
  int idx = htable_hash(h, key);
  struct llist *bucket = h->buckets[idx];

  if (!bucket) {
    return NULL;
  }

  struct lnode *node = llist_find(bucket, key, filter_bucket);
  if (!node) {
    return NULL;
  }
  return ((struct htable_entry *)node->data)->data;
}

void htable_destroy_buckets() {}

void htable_entry_print(void *entry_ptr) {
  struct htable_entry *entry = (struct htable_entry*)entry_ptr;

  printf("  \"%s\": (%p)\n", entry->key, entry->data);
}

void htable_print(struct htable *h) {
  printf("{\n");
  for (int i = 0; i < h->size; ++i) {
    struct llist *bucket = h->buckets[i];
    if (!bucket) {
      continue;
    }
    llist_iter(bucket, htable_entry_print);
  }
  printf("}\n");
}

// TODO: fix memory leaks
int main() {
  struct htable h = htable_init();
  //htable_insert(&h, "Content-Type", "json");
  htable_insert(&h, "Content-Length", "299");
  //htable_insert(&h, "Authorization", "Bearer ey2173713723jdsfj832.w942urw9f.231323");
  const void *result = htable_get(&h, "Content-Length");
  if (!result) {
    printf("Item not found\n");
  } else {
    printf("%s\n", (char *)result);
  }
  htable_print(&h);
}

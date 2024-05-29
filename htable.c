#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "llist.c"

#define MAX_BUCKETS_SIZE 64

struct htable_item {
  char *key;
  void *data;
};

struct htable {
  struct llist *buckets;
  size_t size;
};

// djb2 hash algoritim
int hash(struct htable *h, const char *key) {
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

void htable_insert(char *key, void *data) {

}

void *htable_get(char *key) {

}

void htable_destroy_buckets() {}

int main() {
  struct htable h = htable_init();
  const char* ct = "Content-Type";
  printf("Index for %s: %d\n", ct, hash(&h, ct));

  const char *hello = "Hello World";
  printf("Index for %s: %d\n", hello, hash(&h, hello));

  const char *hello2 = "Hello";
  printf("Index for %s: %d\n", hello2, hash(&h, hello2));
  const char *hello3 = "Hello World2";
  printf("Index for %s: %d\n", hello3, hash(&h, hello3));

  const char *auth = "Authorization";
  printf("Index for %s: %d\n", auth, hash(&h, auth));

  const char *conn = "Connection";
  printf("Index for %s: %d\n", conn, hash(&h, conn));

  const char *cache = "X-Cache";
  printf("Index for %s: %d\n", cache, hash(&h, cache));

  const char *cl = "Content-Length";
  printf("Index for %s: %d\n", cl, hash(&h, cl));
  return 0;
}

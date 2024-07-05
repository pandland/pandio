#include <criterion/criterion.h>
#include "../src/htable.h"

Test(htable, init) {
  struct htable *h = htable_alloc();
  htable_free(h);
}

Test(htable, insert) {
  struct htable *h = htable_alloc();
  htable_insert(h, strdup("Content-Length"), "1223");
  htable_insert(h, strdup("Content-Type"), "application/json");

  const char *result = htable_get(h, "Content-Type");
  cr_expect_str_eq(result, "application/json");

  htable_free(h);
}

Test(htable, get_not_found) {
  struct htable *h = htable_alloc();
  htable_insert(h, strdup("Content-Length"), "1223");
  htable_insert(h, strdup("Content-Type"), "application/json");

  const char *result = htable_get(h, "Connection");
  cr_expect_eq(result, NULL);

  htable_free(h);
}

Test(htable, get_found) {
  struct htable *h = htable_alloc();
  htable_insert(h, strdup("Content-Length"), "128");

  const char *result = htable_get(h, "Content-Length");
  cr_expect_str_eq(result, "128");

  htable_free(h);
}

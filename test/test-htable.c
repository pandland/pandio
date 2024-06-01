#include <criterion/criterion.h>
#include "../src/htable.h"

Test(htable, init) {
  struct htable *h = htable_alloc();
  htable_free(h);
}

Test(htable, insert) {
  struct htable *h = htable_alloc();
  htable_insert(h, "Content-Type", "application/json");

  const char *result = htable_get(h, "Content-Type");
  cr_expect_str_eq(result, "application/json");
}

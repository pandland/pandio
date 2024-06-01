#include <criterion/criterion.h>
#include "../src/llist.h"

struct sample {
  int number;
  struct llist_node lnode;
} typedef sample_t;

sample_t sample_init(int number) {
  sample_t s;
  s.number = number;
  llist_init_node(&s.lnode);

  return s;
}

Test(llist, init) {
  struct llist *l = llist_alloc();
  sample_t s = sample_init(200);
  cr_expect_eq(s.number, 200);

  llist_free(l);
}

Test(llist, iterate) {
  struct llist *l = llist_alloc();
  
  sample_t s1 = sample_init(100);
  sample_t s2 = sample_init(300);
  sample_t s3 = sample_init(500);

  llist_add(l, &s1.lnode);
  llist_add(l, &s2.lnode);
  llist_add(l, &s3.lnode);

  int expected = 100;

  llist_loop(l) {
    sample_t *container = container_of(node, sample_t, lnode);
    cr_expect_eq(container->number, expected);
    expected += 200;
  }

  llist_free(l);
}

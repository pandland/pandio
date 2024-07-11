#include <criterion/criterion.h>
#include <stdio.h>
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
    printf("%d, ", container->number);
  }


  sample_t s4 = sample_init(200);
  sample_t s5 = sample_init(800);
  sample_t s6 = sample_init(50);
  sample_t s7 = sample_init(70);

  llist_remove(l, &s3.lnode);
  llist_remove(l, &s1.lnode);

  llist_add(l, &s4.lnode);
  llist_add(l, &s5.lnode);
  llist_remove(l, &s4.lnode);
  //llist_remove(l, &s2.lnode);
  llist_add(l, &s6.lnode);
  llist_add(l, &s7.lnode);
  llist_add(l, &s3.lnode);

  //llist_remove(l, &s6.lnode);
  llist_remove(l, &s7.lnode);
  //llist_remove(l, &s3.lnode);
  llist_remove(l, &s5.lnode);
  //llist_remove(l, &s2.lnode);


  printf("\nSize: %ld\n", l->size);
  llist_loop(l) {
    sample_t *container = container_of(node, sample_t, lnode);
    printf("%d, ", container->number);
  }

  llist_free(l);
}

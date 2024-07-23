#include <criterion/criterion.h>
#include "../src/slice.h"

Test(slice, lower_cstrcmp) {
  const char *buf = "Content-Length: hello";
  slice_t slice = { .start = buf, .size = 14 };
  bool is_equal = slice_lower_cstrcmp(slice, "content-length");
  bool isnt_equal = slice_lower_cstrcmp(slice, "content-lengthz");
  cr_expect_eq(is_equal, true);
  cr_expect_eq(isnt_equal, false);
}

// this function is supposed to check STRICT EQUALITY
Test(slice, lower_cstrcmp_strict) {
  const char *buf = "Content-lengthz: hello";
  slice_t slice = { .start = buf, .size = 15 };
  bool is_equal = slice_lower_cstrcmp(slice, "content-length");
  cr_expect_eq(is_equal, false);
}

Test(slice, toint64) {
  slice_t slice = { .start = "2003", .size = 4 };
  int64_t result = slice_toint64(slice);
  cr_expect_eq(result, 2003);
}

Test(slice, toint64_reject_overflow) {
  slice_t slice = { .start = "999999999999999999999999999999999999999", .size = 39 };
  int64_t result = slice_toint64(slice);
  cr_expect_eq(result, -1);
}

Test(slice, toint64_reject_negative) {
  slice_t slice = { .start = "-233", .size = 4 };
  int64_t result = slice_toint64(slice);
  cr_expect_eq(result, -1);
}

Test(slice, lower_startswith) {
  const char *buf = "Content-lengthz: hello";
  slice_t slice = { .start = buf, .size = 15 };
  bool is_equal = slice_lower_startswith(slice, "content-length");
  cr_expect_eq(is_equal, true);
}

Test(slice, lower_startswith_invalid) {
  const char *buf = "Content-lengtzh: hello";
  slice_t slice = { .start = buf, .size = 15 };
  bool is_equal = slice_lower_startswith(slice, "content-length");
  cr_expect_eq(is_equal, false);
}

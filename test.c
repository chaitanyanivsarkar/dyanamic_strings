#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ds.h"

enum Tests {create=10, destroy=20, push=30,
            append=40, pop=50, join=60,
	    length=70, capacity=80, copy=90,
	    substr=100, compare=110, contains=120, 
	    all=0};
enum Result {SUCCESS=0, FAIL=1};

int test_new_str(void);
int test_free_str(void);
int test_push(void);
int test_pop(void);
int test_append(void);
int test_join(void);
int test_length(void);
int test_capacity(void);
int test_copy(void);
int test_substring(void);
int test_compare(void);
int test_contains(void);

typedef int (*test_func)(void); 

static test_func all_tests[] = {
  test_new_str,
  test_free_str,
  test_push,
  test_pop,
  test_append,
  test_join,
  test_length,
  test_capacity,
  test_copy,
  test_substring,
  test_compare,
  test_contains
};

enum Tests getTest(char *name)
{
  printf("%s\n", name);
  if (!strncmp(name, "crt", 3)) {
    return create;
  } else if (!strncmp(name, "dst", 3)) {
    return destroy;
  } else if (!strncmp(name, "pus", 3)) {
    return push;
  } else if (!strncmp(name, "pop", 3)) {
    return pop;
  } else if (!strncmp(name, "app", 3)) {
    return append;
  } else if (!strncmp(name, "jon", 3)) {
    return join;
  } else if (!strncmp(name, "len", 3)) {
    return length;
  } else if (!strncmp(name, "cap", 3)) {
    return capacity;
  } else if (!strncmp(name, "cpy", 3)) {
    return copy;
  } else if (!strncmp(name, "con", 3)) {
    return contains;
  } else if (!strncmp(name, "sub", 3)) {
    return substr;
  } else if (!strncmp(name, "cmp", 3)) {
    return compare;
  } else if (!strncmp(name, "non", 3)) {
    return -2;
  }
  return all;
}

int run_all_tests() {
  int res = 0;
  for (size_t i = 0; i < (sizeof(all_tests)/sizeof(all_tests[0])); ++i) {
    res += all_tests[i]();
  }

  return res;
}

int
selector(enum Tests sel)
{
  switch(sel) {
  case create:
    return test_new_str();
  case destroy:
    return test_free_str();
  case append:
    return test_append();
  case join:
    return test_join();
  case push:
    return test_push();
  case pop:
    return test_pop();
  case length:
    return test_length();
  case capacity:
    return test_capacity();
  case copy:
    return test_copy();
  case contains:
    return test_contains();
  case substr:
    return test_substring();
  case compare:
    return test_compare();
  case all:
    return test_new_str() + test_free_str();
  default:
    return 1;
  }
}

void
print_help(void)
{
  printf("-cr: test create string\n");
  printf("-d: test destroy string\n");
  printf("-pu: test push string\n");
  printf("-po: test pop string\n");
  printf("-ap: test append string\n");
  printf("-j: test join strings\n");
  printf("-l: test length of string\n");
  printf("-ca: test string capacity\n");
  printf("-cp: test string copy\n");
  printf("-cm: test string compare\n");
  printf("-sb: test string substrong\n");
  printf("-cn: test string contains\n");
}

char *
getOption(int argc, char **argv)
{
  if (argc <= 1) return "all";
  if (!strncmp(argv[1], "-cr", 3)) return "crt";
  if (!strncmp(argv[1], "-d", 2)) return "dst";
  if (!strncmp(argv[1], "-pu", 3)) return "pus";
  if (!strncmp(argv[1], "-po", 3)) return "pop";
  if (!strncmp(argv[1], "-ap", 3)) return "app";
  if (!strncmp(argv[1], "-j", 2)) return "jon";
  if (!strncmp(argv[1], "-l", 2)) return "len";
  if (!strncmp(argv[1], "-ca", 3)) return "cap";
  if (!strncmp(argv[1], "-cp", 3)) return "cpy";
  if (!strncmp(argv[1], "-cm", 3)) return "cmp";
  if (!strncmp(argv[1], "-sb", 3)) return "sub";
  if (!strncmp(argv[1], "-cn", 3)) return "con";
  if (!strncmp(argv[1], "-h", 2)) {
    print_help();
    return "non";
  } 
  return "non";
}

int
main (int argc, char **argv)
{
  char *opt = getOption(argc, argv);
  if (!strncmp(opt, "non", 3)) return 0;

  enum Tests sel = getTest(opt);
  if (selector(sel) == 0) {
    printf("test success\n");
    return 0;
  }
  printf("test fail\n");
  return 1;
}

/* Test wether a new dynamic string has been created with the contents */
int
test_new_str(void) {
  dstr test_contents_ctor = ds_new(.contents="hi all");
  dstr test_substr_ctor = ds_new(.contents="hi all this", .len=6);
  dstr test_substr_ctor1 = ds_new(.contents="hi all this", .len=3, .pos=3);

  if (strncmp(test_contents_ctor, "hi all", 6)) return FAIL;
  if (strncmp(test_substr_ctor, "hi all", 6)) return FAIL;
  if (strncmp(test_substr_ctor1, "all", 3)) return FAIL;

  return SUCCESS;
}

/* Test wether the dynamic string pointer has been freed or not. */
/* No way to test wether a pointer is freed or not but in gdb it 
shows that correct pointer was freed. */
int
test_free_str(void)
{
  dstr test_free = ds_new(.contents="hi all");
  ds_free(test_free);
  return SUCCESS;
}

int
test_push(void)
{
  dstr target = "this is a test..........";
  dstr str = ds_new(.contents="this is a test");

  for (int i = 0; i < 10; ++i) str = ds_push(str, '.');

  if (strlen(str) != strlen(target)) return FAIL;
  if (strncmp(str, target, strlen(str))) return FAIL;

  return SUCCESS;
}

int
test_pop(void)
{
  dstr str = ds_new(.contents="test");

  for (int i = 0; i < 3; ++i) ds_pop(str);

  if (strlen(str) != 1) return FAIL;
  if (strncmp(str, "t", strlen(str))) return FAIL;

  return SUCCESS;
}

int
test_join(void)
{
  const dstr str_arr[] = {ds_new(.contents="this"), ds_new(.contents="is"),
                     ds_new(.contents="a"), ds_new(.contents="test")};

  dstr res = ds_join(ds_new(), str_arr, 4, ds_new(.contents=" "));
  dstr target = "this is a test";

  if (strlen(res) != strlen(target)) return FAIL;
  if (strncmp(res, target, strlen(target)+1)) return FAIL;

  return SUCCESS;
}

int
test_append(void)
{
  dstr str1 = ds_new(.contents="this ");
  dstr str2 = ds_new(.contents="is a test with some long string");
  dstr target = "this is a test with some long string";
  str1 = ds_append(str1, str2);

  if (strlen(str1) != strlen(target))
    return FAIL;
  if (strncmp(str1, target, strlen(target))) return FAIL;

  return SUCCESS;
}

int
test_length(void)
{
  dstr str = ds_new(.contents="chaitanyanivsarkar");
  if (ds_length(str) != 18) return FAIL;

  return SUCCESS;
}

int
test_capacity(void)
{
  dstr str = ds_new(.contents="hi");
  if (ds_sizeof(str) != 16) return FAIL;

  return SUCCESS;
}

int
test_copy(void)
{
  dstr str = ds_new(.contents="chaitanyanivsarkar");
  dstr cp_str = ds_copy(str);

  if (strncmp(str, cp_str, ds_length(str))) return FAIL;
  return SUCCESS;
}

int
test_compare()
{
  dstr larger = ds_new(.contents="z");
  dstr smaller = ds_new(.contents="aa");
  dstr eq = ds_new(.contents="aa");
  dstr mid = ds_new(.contents="aaa");

  if (ds_cmp(larger, smaller) < 1) {
    return FAIL;
  }
  if (ds_cmp(smaller, eq) != 0) {
    return FAIL;
  }
  if (ds_cmp(smaller, mid) >= 0) {
    return FAIL;
  }
  
  return SUCCESS;
}

int
test_substring()
{
  dstr text = ds_new(.contents="Alargestring");
  dstr sub = ds_substr(text, 1, 5);

  if (strncmp(sub, "large", 5)) return FAIL;
  
  return SUCCESS;
}

int test_contains()
{
  dstr text = ds_new(.contents="Alargestring");
  dstr sub = ds_new(.contents="large");

  if (ds_contains(text, sub) == ds_length(text)) return FAIL;
  
  return SUCCESS;
}

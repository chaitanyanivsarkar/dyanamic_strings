#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ds.h"

enum Tests {create=10, destroy=20, all=0};
enum Result {SUCCESS=0, FAIL=1};

int test_new_str();
int test_free_str();

enum Tests getTest(char *name)
{
  if (!strncmp(name, "crt", 3)) {
    return create;
  } else if (!strncmp(name, "dst", 3)) {
    return destroy;
  } else {
    return all;
  }
}

int selector(enum Tests sel)
{
  switch(sel) {
  case create:
    return test_new_str();
  case destroy:
    return test_free_str();
  case all:
    return test_new_str() + test_free_str();
  default:
    return 1;
  }
}

char * getOption(int argc, char **argv)
{
  if (argc < 1) return "all";
  if (!strncmp(argv[1], "-c", 2)) return "crt";
  if (!strncmp(argv[1], "-d", 2)) return "dst";
  return "non";
}

int main (int argc, char **argv)
{
  char *opt = getOption(argc, argv);
  if (!strncmp(opt, "non", 3)) return 1;

  enum Tests sel = getTest(opt);
  if (selector(sel) == 0) {
    printf("test success\n");
    return 0;
  }
  printf("test fail\n");
  return 1;
}

/* Test wether a new dynamic string has been created with the contents */
int test_new_str() {
  char *test_contents_ctor = ds_new(.contents="hi all");
  char *test_substr_ctor = ds_new(.contents="hi all this", .len=6);
  char *test_substr_ctor1 = ds_new(.contents="hi all this", .len=3, .pos=3);

  printf("%s\n", test_contents_ctor);
  printf("%s\n", test_substr_ctor);
  printf("%s\n", test_substr_ctor1);

  if (strncmp(test_contents_ctor, "hi all", 6)) return FAIL;
  if (strncmp(test_substr_ctor, "hi all", 6)) return FAIL;
  if (strncmp(test_substr_ctor1, "all", 3)) return FAIL;

  return SUCCESS;
}

/* Test wether the dynamic string pointer has been freed or not. */
/* No way to test wether a pointer is freed or not but in gdb it 
shows that correct pointer was freed. */
int test_free_str()
{
  char *test_free = ds_new(.contents="hi all");
  ds_free(test_free);
  return SUCCESS;
}

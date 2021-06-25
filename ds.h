#pragma once

#include <stdlib.h>

/*
multiple constructors less efficient than a normal function call but more 
conveniant
*/

/* constructor args: */
/* pos and length are only used if contents are provided. */
struct ds_new_args {
  size_t size; /* size to allocate default 16 */
  size_t len; /* length of string default 0 */
  size_t pos; /* begin position default 0 */
  char *contents; /* contents to copy default NULL */
};

void * ds_new_var(struct ds_new_args args);

#define ds_new(...) ds_new_var((struct ds_new_args){__VA_ARGS__})
void ds_free(char *);

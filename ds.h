#pragma once

#include <stdlib.h>
#include <string.h>
#include <stddef.h>


struct ds_string {
  size_t len;
  size_t size;
  char contents[];
};

struct ds_new_args {
  size_t size;
  size_t len;
  size_t pos;
  char *contents;
};


static void
__fill_str(struct ds_string *new_str, const char *contents,
	   size_t len, size_t pos)
{
  if (len == 0) len = strlen(contents);
  if (pos >= strlen(contents)) return;

  strncat(new_str->contents, contents+pos, len);
}

static char *
__new_base(size_t size, size_t len,
	   size_t pos, const char *contents)
{
  struct ds_string *new_str =
    malloc(sizeof(struct ds_string)+(size*sizeof(char)));
  if (!new_str) return NULL;

  if (contents) __fill_str(new_str, contents, len, pos); 

  return (char *)new_str + offsetof(struct ds_string, contents);
}


char *
__new_var(struct ds_new_args args)
{
  /* filling in default values if not provided */
  size_t size = args.size ? args.size : 16;
  size = size > args.len ? size : args.len;
  size_t len = args.len ? args.len : 0;
  size_t pos = args.pos ? args.pos : 0;
  const char *contents = args.contents == NULL ? NULL : args.contents;
  if (contents) {
    size_t contents_size = 1 + strlen(contents);
    size = size > contents_size ? size : contents_size;
  }

  return __new_base(size, len, pos, contents);
}

#define ds_new(...) __new_var((struct ds_new_args){__VA_ARGS__})

void
ds_free(char *str)
{
  struct ds_string *container;
  container = (struct ds_string *)
    (str - offsetof(struct ds_string, contents));
  free(container);
  return;
}

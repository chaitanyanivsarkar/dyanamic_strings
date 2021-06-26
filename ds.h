#pragma once

#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <stdio.h>

typedef char* dstr;

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
  new_str->len = len;
}

static char *
__new_base(size_t size, size_t len,
	   size_t pos, const char *contents)
{
  struct ds_string *new_str =
    malloc(sizeof(struct ds_string)+(size*sizeof(char)));
  if (!new_str) return NULL;

  if (contents) __fill_str(new_str, contents, len, pos); 
  new_str->size = size;
  return new_str->contents;
}

static struct ds_string *
__ds_relocate(struct ds_string *container, size_t new_size)
{
  struct ds_string *old = container;
  if (new_size == 0) new_size = 2*old->size;

  container = malloc(sizeof(struct ds_string)
		     + (sizeof(char)*new_size));
  if (container == NULL) return NULL;

  container->size = new_size;
  container->len = old->len;
  strncat(container->contents, old->contents, container->len);
  
  free(old);

  return container;
}

#define get_container(str) (struct ds_string *) \
    (str - offsetof(struct ds_string, contents))


char *
__new_var(struct ds_new_args args)
{
  /* filling in default values if not provided */
  size_t size = args.size > 16 ? args.size : 16;
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
ds_free(dstr str)
{
  struct ds_string *container = get_container(str);
  free(container);
  return;
}

/* push, append, and join take ownership of the pointer */
char *
ds_push(dstr str, char c)
{
  struct ds_string *container = get_container(str);

  if (container->size <= container->len+2) {
    container = __ds_relocate(container, 0);
  }

  container->contents[container->len] = c;
  container->contents[container->len+1] = '\0';
  ++container->len;
  
  return container->contents;
}

void
ds_pop(dstr str)
{
  struct ds_string *container = get_container(str);

  if (container->len == 0) return;
  container->contents[container->len-1] = '\0';
  --container->len;
}

char *
ds_append(dstr dest, const dstr cat)
{
  struct ds_string *dst_cont = get_container(dest);
  struct ds_string *cat_cont = get_container(cat);

  if (dst_cont->size <= dst_cont->len + cat_cont->len + 1) {
    size_t fac = (dst_cont->len + cat_cont->len + 1)/16 + 1;
    fac = fac < 2 ? 2 : fac;

    __ds_relocate(dst_cont, fac*dst_cont->size);
  }

  strncat(dst_cont->contents + dst_cont->len, cat_cont->contents,
	  cat_cont->len);
  
  return dst_cont->contents;
}

char *
ds_join(dstr dest, const dstr *str_arr, size_t len, dstr delim)
{
  for (size_t i = 0; i < len-1; ++i) {
    dest = ds_append(dest, str_arr[i]);
    dest = ds_append(dest, delim);
  }
  dest = ds_append(dest, str_arr[len-1]);
  
  return dest;
}

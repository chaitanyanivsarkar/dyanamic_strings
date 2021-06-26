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
__compute_lps_array(dstr pattern, size_t len_pat, int *lps)
{
  int len = 0;
  lps[0] = 0;
  size_t i = 1;
  while (i < len_pat) {
    if (pattern[i] == pattern[len]) {
      ++len;
      lps[i] = len;
      ++i;
    } else {
      if (len != 0) len = lps[len - 1];
      else lps[i++] = 0;
    }
  }
}

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
  if (container == NULL) return NULL;

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

    dst_cont = __ds_relocate(dst_cont, fac*dst_cont->size);
  }
  if (dst_cont == NULL) return NULL;

  strncat(dst_cont->contents + dst_cont->len, cat_cont->contents,
	  cat_cont->len);
  
  return dst_cont->contents;
}

char *
ds_join(dstr dest, const dstr *str_arr, size_t len, dstr delim)
{
  for (size_t i = 0; i < len-1; ++i) {
    dest = ds_append(dest, str_arr[i]);
    if (dest == NULL) return NULL;
    dest = ds_append(dest, delim);
    if (dest == NULL) return NULL;
  }
  dest = ds_append(dest, str_arr[len-1]);
  if (dest == NULL) return NULL;
  
  return dest;
}

size_t
ds_length(dstr str)
{
  struct ds_string *cont = get_container(str);
  return cont->len;
}

size_t
ds_sizeof(dstr str)
{
  struct ds_string *cont = get_container(str);
  return cont->size;
}

dstr
ds_copy(dstr str)
{
  struct ds_string *cont = get_container(str);
  size_t size = sizeof(struct ds_string) + sizeof(char)*cont->size;
  struct ds_string *copy = malloc(size);
  if (copy == NULL) return NULL;

  memcpy(copy, cont, size);
  
  return copy->contents;
}

dstr
ds_substr(dstr str, size_t begin, size_t len)
{
  struct ds_string *cont = get_container(str);
  return ds_new(.len=len, .pos=begin, .contents=cont->contents);
}

size_t
ds_contains(dstr text, dstr pattern)
{
  size_t len_txt = ds_length(text);
  size_t len_pat = ds_length(pattern);

  int lps[len_pat];
  __compute_lps_array(pattern, len_pat, lps);

  size_t i = 0, j = 0;
  while (i < len_txt) {
    if (pattern[j] == text[i]) {
      ++i;
      ++j;
    }

    if (j == len_pat) {
      return i-j;
    }

    else if (i < len_txt && pattern[j] != text[i]) {
      if (j != 0) j = lps[j-1];
      else ++i;
    }
  }
  
  return len_txt;
}

int
ds_cmp(dstr str_a, dstr str_b)
{
  struct ds_string *cont_a = get_container(str_a);
  struct ds_string *cont_b = get_container(str_b);
  size_t len_a = cont_a->len;
  size_t len_b = cont_b->len;

  size_t min_len = len_a < len_b ? len_a : len_b;
  size_t i = 0;
  while(i < min_len) {
    if (cont_a->contents[i] == cont_b->contents[i]) ++i;
    else if (cont_a->contents[i] > cont_b->contents[i]) return 1;
    else return -1;
  }

  if (len_a > len_b) return 1;
  if (len_b > len_a) return -1;

  return 0;
}

#include <oraccsys.h>

char **
vec_add (char **vec, char *ptr)
{
  size_t len;
  
  if (NULL == vec)
    len = 0;
  else
    {
      register char **end = vec;
      while (NULL != *end)
        ++end;
      len = (size_t) (end - vec);
    }
  vec = realloc (vec, (len + 2) * sizeof (char *));
  vec[len++] = ptr;
  vec[len] = NULL;
  return vec;
}

size_t
vec_len (char * const*vec)
{
  size_t ret = 0;
  if (NULL == vec)
    return 0;
  while (NULL != vec[ret])
    ++ret;
  return ret;
}

char *
vec_to_str (char **vec, size_t count, char *sep)
{
  size_t len, i, sep_len = strlen (sep);
  char *tmp;

  if (!count)
    return xstrdup ("");

  if (!vec || !*vec)
    abort();

  for (i = 0, len = 0; i < count; ++i)
    len += strlen (vec[i]) + sep_len;
  tmp = malloc (len + 1);
  *tmp = '\0';
  for (i = 0, len = 0; i < count; ++i)
    {
      if (i)
	(void)strcat (tmp, sep);
      (void)strcat (tmp, vec[i]);
    }
  return tmp;
}

const char *vec_sep_str = "\t\n ";
char **
vec_from_str (char * str, char *(*tok)(const char *), size_t *vecsize_p)
{
  extern char *strdup(const char *);
  int alloc = 4;
  char **tmp = calloc(alloc+1, sizeof(char*));
  size_t count = 0;
  char *t = strtok(str, vec_sep_str);
  while (t)
    {
      if (count >= alloc)
	{
	  alloc *= 2;
	  tmp = realloc(tmp, (1+alloc)*sizeof(char*));
	}
      tmp[count++] = strdup(t);
      t = strtok(NULL, vec_sep_str);
    }
  tmp[count] = NULL;
  if (NULL != vecsize_p)
    *vecsize_p = count;
  return tmp;
}

char **
vec_find (char **vec, char *str)
{
  if (!vec)
    return NULL;
  for (; *vec; ++vec)
    if (!strcmp (*vec, str))
      return vec;
  return vec;
}

static size_t stack_len = 0;
static const char **stack = NULL;

void
vec_sep_push (const char *new_sep)
{
  stack = realloc (stack, (stack_len + 1) * sizeof (char*));
  stack[stack_len++] = vec_sep_str;
  vec_sep_str = new_sep;
}

void
vec_sep_pop (void)
{
  if (stack_len && stack)
    {
      vec_sep_str = stack[--stack_len];
      if (stack_len)
	stack = realloc (stack, stack_len * sizeof (char *));
      else
	{
	  free(stack);
	  stack = NULL;
	}
    }
}

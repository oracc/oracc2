#include <oraccsys.h>
#include "lx.h"

/* If the result of processing the args is a non-null the list is
   guaranteed to have be at list one list followed by a sequence of
   alternating operator-filename pairs*/
List *
lx_set_args(char * const*argv, int start)
{
  if (!argv)
    return NULL;
  
  List *todo = list_create(LIST_SINGLE);
  int state = 0; /* 0 = expecting list file; 1 = expecting list math operator */
  int status = 0; /* 0 = OK to proceed after args; 1 = exit after args */  
  while (argv[start])
    {
      if (state)
	{
	  if (strlen(argv[start]) == 1 && strchr("-+/", *argv[start]))
	    {
	      if (!status)
		list_add(todo, (void*)argv[start]);
	    }
	  else
	    {
	      fprintf(stderr, "%s: %s: bad operator in list math (expected + - /).\n",
		      argv[0], argv[start]);
	      ++status;
	    }
	  ++start;
	  state = 0;
	}
      else
	{
	  Lxfile *lxf = lx_load(argv[start]);
	  if (lxf)
	    {
	      if (!status)
		list_add(todo, lxf);
	    }
	  else
	    {
	      fprintf(stderr, "%s: %s: list failed to load.\n", argv[0], argv[start]);
	      ++status;
	    }
	  ++start;
	  state = 1;
	}
    }
  if (!state && !status)
    {
      fprintf(stderr, "list math arguments must end with a list. Stop\n");
      ++status;
    }
  if (status)
    {
      list_free(todo, NULL);
      todo = NULL;
    }

  return todo;
}

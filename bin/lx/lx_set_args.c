#include <oraccsys.h>
#include "lx.h"

/* If the result of processing the args is a non-null the list is
   guaranteed to have be at list one list followed by a sequence of
   alternating operator-filename pairs*/
List *
lx_mathargs(char * const*argv)
{
  List *todo = list_create(LIST_SINGLE);
  int state = 0; /* 0 = expecting list file; 1 = expecting list math operator */
  int status = 0; /* 0 = OK to proceed after args; 1 = exit after args */
  while (argv[optind])
    {
      if (state)
	{
	  if (strlen(argv[optind]) == 1 && strchr("-+/", *argv[optind]))
	    {
	      if (!status)
		list_add(todo, (void*)argv[optind++]);
	    }
	  else
	    {
	      fprintf(stderr, "%s: %s: bad operator in list math (expected + - /).\n",
		      argv[0], argv[optind++]);
	      ++status;
	    }
	  state = 0;
	}
      else
	{
	  Lxfile *lxf = lx_load(argv[optind++]);
	  if (lxf)
	    {
	      if (!status)
		list_add(todo, lxf);
	    }
	  else
	    {
	      fprintf(stderr, "%s: %s: list failed to load.\n", argv[0], argv[optind]);
	      ++status;
	    }
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

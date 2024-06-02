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
  int optional = 0; /* set to 1 if an operator has a ? after it */
  int index = 0; /* number of args processed */
  const char *pending_op = 0;
  while (argv[start])
    {
      if (state)
	{
	  if (strlen(argv[start]) > 1 && '?' == argv[start][1])
	    optional = 1;
	  if ((strlen(argv[start]) == 1
	       || (optional && strlen(argv[start]) == 2))
	      && strchr("-+/", *argv[start]))
	    {
	      if (!status)
		pending_op = argv[start];
	      /*list_add(todo, (void*)argv[start]);*/
	    }
	  else
	    {
	      fprintf(stderr, "%s: %s: bad operator in list math (expected + - /).\n",
		      argv[0], argv[start]);
	      pending_op = NULL;
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
		{
		  if (index)
		    list_add(todo, (void*)pending_op);
		  list_add(todo, lxf);
		}
	    }
	  else
	    {
	      if (!optional)
		{
		  fprintf(stderr, "%s: %s: list failed to load.\n", argv[0], argv[start]);
		  ++status;
		}
	    }
	  ++start;
	  ++index;
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

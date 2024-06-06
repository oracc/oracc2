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
  int nextoptional = 0; /* set to 1 if an operator has a ? after it */
  int index = 0; /* number of args processed */
  const char *pending_op = 0;

  /* the -z arg means the first file is optional */
  nextoptional = zfirstoptional || all_optional;

  while (argv[start])
    {
      if (state)
	{
	  if (strlen(argv[start]) > 1 && '?' == argv[start][1])
	    nextoptional = 1;
	  else
	    nextoptional = all_optional;
	  if ((strlen(argv[start]) == 1
	       || (nextoptional && strlen(argv[start]) == 2))
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
	  if (!strcmp(argv[start], "-"))
	    {
	      Lxfile *lxf = lx_load(argv[start]);
	      if (!lxf || !lxf->nitems)
		{
		  if (!nextoptional)
		    {
		      fprintf(stderr, "%s: stdin is empty and is not optional.\n", argv[0]);
		      ++status;
		    }
		}
	      else
		list_add(todo, lxf);
	    }
	  else
	    {
	      struct stat st;	  
	      if (!stat(argv[start], &st))
		{
		  if (st.st_size)
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
			  if (!nextoptional)
			    {
			      fprintf(stderr, "%s: %s: list failed to load and is not optional.\n", argv[0], argv[start]);
			      ++status;
			    }
			}
		    }
		  else
		    {
		      if (!nextoptional)
			{
			  fprintf(stderr, "%s: %s: list is empty and is not optional.\n", argv[0], argv[start]);
			  ++status;
			}
		    }
		}
	      else
		{
		  if (!nextoptional)
		    {
		      fprintf(stderr, "%s: %s: list does not exist and is not optional.\n", argv[0], argv[start]);
		      ++status;
		    }
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

/* Only gx uses flags around @-names so we use our own custom parser for it

   We will need to stash the pre-flags and post-flags and pass them to
   the data parser for @entry, @sense
   
 */

char *
cat_name(void *t)
{
  char *name = NULL, *pre_flags = NULL, *post_flags = NULL, *data = NULL;
  /* get chunk name */
  pre_flags = name = cp->text;
  while (*name && '@' != *name)
    ++name;
  if ('@' == *name)
    {
      struct catnode *cn = mb_new(catnode_mem);
      *name++ = '\0';
      cn->n = name;
      post_flags = name;
      while (*post_flags && post_flags < 128 && isalnum(*post_flags))
	++post_flags;
      data = post_flags;
      while (*data && data < 128 && !isspace(*data))
	++data;
      if (*isspace(*data))
	*data++ = '\0';
      if (post_flags < data && *post_flags && *post_flags < 128)
	{
	  char *pf = npool_alloc((data - post_flags) + 1, catpool);
	  
	}
    }
}


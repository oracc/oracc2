static Blocktok *last_bp = NULL;
static Blocktok *last_fragp = NULL;
void
atf_bp_reset(void)
{
  last_fragp = last_bp = NULL;
}

static int
atf_block(const char *scanned_at)
{
  const char *atcmd = scanned_at + 1;

  Blocktok *bp = blocktok(atcmd, strlen(atcmd));
  if (bp)
    {
      if (bp->bison == Y_TRANSLATION)
	BEGIN s_TRD;
      if (bp->bison == Y_FRAGMENT)
	{
          if (atfp->edoc == EDOC_COMPOSITE || atfp->edoc == EDOC_SCORE)
	     bp = blocktok("cfragment", 9); /* c[omposite]fragment */
          else if (last_bp)
	    {
	      if (last_bp->type == B_OBJECT)
	        bp = blocktok("objfrag", 7);
	      else if (last_bp->type == B_SURFACE)
	        bp = blocktok("srffrag", 7);
	      else if (last_bp->type == B_COLUMN)
	        bp = blocktok("colfrag", 7);
	      else
	        bp = blocktok("srffrag", 7);
	    }
	  else
            bp = blocktok("srffrag", 7);

	  if (last_fragp)
	    {
	      if (last_fragp->type != bp->type)
		  mesg_verr(&atflloc, "text mixes @fragment at different levels (last=%d!=curr=%d: missing @column?)", last_fragp->type, bp->type);
	      else
		{
		    /*mesg_verr(&atflloc, "@fragment level == %d", bp->type);*/
 	 	  bp = last_fragp;
		}
	    }
	  else
	      /*mesg_verr(&atflloc, "@fragment level == %d", bp->type)*/;
	  last_fragp = bp;
        }
      else
	last_bp = bp;
      atflval.b = bp;
      return bp->bison;
    }
  else
    {
      last_bp = NULL;
      fprintf(stderr, "atf: bad at-command @%s\n", atcmd);
      return Y_BAD;
    }
}

void
atf_wrapup_buffer(void)
{
  atf_delete_buffer(YY_CURRENT_BUFFER);
}

ssize_t
atf_get_lineno(void)
{
    return last_atflineno;
}

void
atf_set_lineno(ssize_t n)
{
    atfset_lineno(n);
}

void
atf_set_file(const char *f)
{
  file = curratffile = f;
  last_atflineno = atflineno = 1;
  atfset_lineno(1);
}

char *
atf_set_file_line(char *a, ssize_t *n)
{
  char *s = a;
  while (!isspace(*a)) /* we know this line is `##file ' */
    ++a;
  while (isspace(*a))
    ++a;
  curratffile = a;
  while (*a && '\n' != *a)
    ++a;
  if (*a)
    {
      *a++ = '\0';
      if (*a)
        {
          while (*a && !isspace(*a))
	    ++a;
          while (isspace(*a))
            ++a;
          atflineno = (int)strtoul(a, NULL, 10);
          while (*a && '\n' != *a)
	    ++a;
          ++a;
          *n = (*n - (a - s));
          return a;
       }
    }
  return NULL;
}

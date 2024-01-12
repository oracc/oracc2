#include <oraccsys.h>
#include <tok.h>

/* Maintain a single token location status from location lines in a
   .tok file.  The location status can be frozen and retrieved by
   callers if desired.  For most tok uses it won't be necessary to
   maintain old location state in this way because the focus is on
   processing the token lines one-by-one. */

static Hash *tloc_h;
static Memo *tloc_m;
static Pool *tloc_p;

#define tloc_strdup(s) (ccp)hpool_copy((ucp)(s),tloc_p)

static Tloc *curr_tloc;

void
tloc_init(int many)
{
  if (!curr_tloc)
    {
      tloc_m = memo_init(sizeof(Tloc), many ? 1024 : 4);
      tloc_p = hpool_init();
      tloc_h = hash_create(1024);
      curr_tloc = memo_new(tloc_m);
    }    
}

void
tloc_term(void)
{
  if (curr_tloc)
    {
      memo_term(tloc_m);
      hpool_term(tloc_p);
      hash_free(tloc_h, NULL);
      curr_tloc = NULL;
    }
}

void
tloc_key(const char *k, const char *v)
{
  if (curr_tloc->nkeys == curr_tloc->nkeyp)
    {
      curr_tloc->nkeyp += 8;
      curr_tloc->keys = realloc(curr_tloc->keys,
				(1+curr_tloc->nkeyp) * sizeof(const char *));
    }
  curr_tloc->keys[curr_tloc->nkeys++] = tloc_strdup(k);
  curr_tloc->keys[curr_tloc->nkeys++] = tloc_strdup(v);
  curr_tloc->keys[curr_tloc->nkeys] = NULL;
}

Tloc *
tloc_line(Trun *r, char *lp)
{
  if (lp)
    {
      char **f = tab_split(lp);
      switch (*lp)
	{
	case 'W':
	  tlb_W(r, f[1], f[2]);
	  break;
	case 'L':
	  tlb_L(r, f[1], f[2], f[3]);
	  break;
	case 'T':
	  tlb_T(r, f[1], f[2], f[3]);
	  break;
	case 'K':
	  if (f[1] && f[2])
	    tlb_key(r, f[1], f[2]);
	  break;
	case 'F':
	  tlb_F(r, f[1]);
	  break;
	case 'P':
	  tlb_P(r, f[1]);
	  break;
	case 'Y':
	  tlb_Y(r, f[1]);
	  break;
	default:
	  fprintf(stderr, "tok_load_tab: unknown initial letter %c\n", *lp);
	  break;
	}
      free(f);
      return curr_tloc;
    }
  else
    return NULL;
}

Tloc *
tloc_dup(void)
{
  return tloc_dup_arg(curr_tloc);
}

Tloc *
tloc_dup_arg(Tloc *dup)
{
  Tloc *t = memo_new(tloc_m);
  if (dup)
    *t = *dup;
  return t;
}

Tloc *
tloc_get(void)
{
  return curr_tloc; 
}

void
tloc_show(char c)
{
  int i;
  switch (c)
    {
    case 'W':
      printf("W\t%s\t%s\n", curr_tloc->word_id, curr_tloc->word_form ? curr_tloc->word_form : "");
      break;
    case 'L':
      printf("L\t%s\t%s\t%s\n", curr_tloc->line_num, curr_tloc->line_id, curr_tloc->line_label);
      break;
    case 'T':
      printf("T\t%s\t%s\t%s\n", curr_tloc->text_project, curr_tloc->text_id, curr_tloc->text_name);
      break;
    case 'P':
      printf("P\t%s\n", curr_tloc->project);
      break;
    case 'K':
      for (i = 0; i < curr_tloc->nkeys; i+=2)
	printf("K\t%s\t%s\n", curr_tloc->keys[i], curr_tloc->keys[i+1]);
      break;
    case 'F':
      printf("F\t%s\n", curr_tloc->file);
      break;
    case 'Y':
      printf("Y\t%s\n", curr_tloc->type);
      break;
    default:
      fprintf(stderr, "tok_loc: unhandled char %c in tloc_show\n", c);
      break;
    }
}

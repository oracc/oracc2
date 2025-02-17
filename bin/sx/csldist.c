#include <oraccsys.h>
#include <tree.h>
#include <xml.h>

const char *template_file = NULL;
const char *template_tags[] = { "o" , "c1" , "c2", "c3", "c4", "c5", "c6" };

typedef struct M
{
  char *p;
  char *k;
  char *t;
  char *c;
} M;

typedef struct O
{
  Hash *o;
  Hash *p;
  const char *k;
} O;

static void
mpsplit(M *mp, char *s)
{
  char *c = strchr(s,':');
  if (c)
    {
      mp->p = s;
      *c++ = '\0';
      mp->k = c;
    }
  else
    {
      mp->k = s;
    }
  mp->t = strchr(mp->k, '\t');
  *mp->t++ = '\0';
  mp->c = strchr(mp->t, '\t');
  *mp->c++ = '\0';  
}

Tree *template;

static void
template_o(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  if (np->user && strcmp(np->name, (char*)np->user))
    fprintf(xhp->fp, "<%s p=\"%s\">", np->name, (char*)np->user);
  else
    fprintf(xhp->fp, "<%s>", np->name);
}

static void
count_tid_o(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  O* op = xhp->user;
  M* mp = NULL;
  if ('o' == *np->name)
    mp = hash_find(op->o, (uccp)op->k);
  else
    {
      Hash *p = hash_find(op->p, (uccp)np->user);
      if (p)
	mp = hash_find(p, (uccp)op->k);
    }

  if (mp)
    {
      fprintf(xhp->fp, "<%s ", np->name);
      if ('o' == *np->name)
	fprintf(xhp->fp, "k=\"%s\" ", mp->k);
      fprintf(xhp->fp, "c=\"%s\" t=\"%s\">", mp->c, mp->t);
    }
  else
    fprintf(xhp->fp, "<%s>", np->name);
}

static Tree *
load_template(const char *tf)
{
  treexml_o_handlers[0] = template_o;
  Tree *tp = tree_init();
  size_t nline;
  unsigned char *fmem;
  char **lp = (char**)loadfile_lines3((uccp)tf, &nline, &fmem);
  int i;
  int depth = 0;
  for (i = 0; lp[i]; ++i)
    {
      int ntab = 0, backup;
      char *s = lp[i];
      while ('\t' == *s)
	++s;
      ntab = s - lp[i];
      if (ntab < depth)
	{
	  backup = depth - ntab;
	  while (backup-- > 0)
	    tree_pop(tp);
	  depth = ntab;
	}
      else if (ntab && ntab == depth)
	;
      else
	{
	  (void)tree_push(tp);
	  depth = ntab;
	}
      Node *np = tree_add(tp, NS_NONE, template_tags[depth], depth, NULL);
      np->user = s;
    }
  return tp;
}

int
main(int argc, char **argv)
{
  options(argc,argv,"t:");
  size_t nline;
  unsigned char *fmem;
  char **lp = (char**)loadfile_lines3((uccp)"-", &nline, &fmem);
  O op;
  op.o = hash_create(1024);
  op.p = hash_create(10);
  Memo *m = memo_init(sizeof(M),8192);
  if (lp)
    {
      int i;
      for (i = 0; lp[i]; ++i)
	{
	  M* mp = memo_new(m);
	  mpsplit(mp, lp[i]);
	  if (!mp->p)
	    hash_add(op.o, (uccp)mp->k, mp);
	  else
	    {
	      Hash *ph = hash_find(op.p, (uccp)mp->p);
	      if (!ph)
		{
		  ph = hash_create(1024);
		  hash_add(op.p, (uccp)mp->p, ph);
		}
	      hash_add(ph, (uccp)mp->k, mp);
	    }
	}
    }

  FILE *fp = stdout;
  fputs("<csltab><template>", fp);
  
  Xmlhelper *xhp = xmlh_init(stdout);
  xhp->user = &op;
  treexml_o_handlers[0] = template_o;
  tree_iterator(template, xhp, tree_xml_node, tree_xml_post);
  fputs("</template><data>", fp);
  int nok;
  char **ok = (char**)hash_keys2(op.o, &nok);
  treexml_o_handlers[0] = count_tid_o;
  if (ok)
    {
      int i;
      for (i = 0; ok[i]; ++i)
	{
#if 1
	  op.k = ok[i];
	  tree_iterator(template, xhp, tree_xml_node, tree_xml_post);
#else
	  M *mp = hash_find(o, (uccp)ok[i]);
	  fprintf(fp, "<o k=\"%s\" c=\"%s\" t=\"%s\">", mp->k, mp->c, mp->t);
	  fputs("</o>", fp);
#endif
	}
    }
  fputs("</data></csltab>", fp);
  free(xhp);
}

int
opts(int argc, const char *arg)
{
  switch (argc)
    {
    case 't':
      template = load_template(template_file = arg);
#if 0
      tree_xml(stdout, template);
      exit(0);
#endif
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "csldist";
int major_version = 6, minor_version = 0, verbose;
const char *usage_string = "-t [template]";
void
help ()
{
  printf("  -t [template] Gives the name of the template for list organization\n");
}

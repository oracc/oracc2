#include <oraccsys.h>
#include <roco.h>
#include "xml2tsv.h"

List *z_list;
int fmp_mode, verbose, xml_mode;
size_t nrec = 0;
int nfld;
Roco *r;
const char **fields;

static void
x2t_set_mode(const char *fn)
{
  const char *xns = xmlns(fn);
  if (xns && !strcmp(xns, "http://www.filemaker.com/fmpxmlresult"))
    fmp_mode = 1;
  else
    xml_mode = 1;
}

int
x2t_fields_cmp(const void *p1, const void *p2)
{
#define x2t_s(p) *(char * const *)(p)
  if (!strcmp(x2t_s(p1), "id_text"))
    return -1;
  else if (!strcmp(x2t_s(p1), "id_composite"))
    return -1;
  else if (!strcmp(x2t_s(p2), "id_text"))
    return 1;
  else if (!strcmp(x2t_s(p2), "id_composite"))
    return 1;
  else
    return strcmp(x2t_s(p1), x2t_s(p2));
#undef x2t_s
}

int
main(int argc, char * const *argv)
{
  options(argc, argv, "v");
  
  if (!argv[1])
    {
      fprintf(stderr, "xml2tsv: must give file name on command line. Stop.\n");
      exit(1);
    }

  x2t_set_mode(argv[optind]);

  x2t_set_fields(argv[optind]);

  if (!nfld)
    {
      fprintf(stderr, "xml2tsv: XML file %s has no child nodes so output is empty\n", argv[optind]);
      exit(0);
    }

  if (xml_mode) /* can't sort in fmp_mode because the XML data is ordered by METADATA/FIELD */
    {
      qsort(fields, nfld, sizeof(const char *), x2t_fields_cmp);
      if (verbose)
	{
	  int i;
	  fprintf(stderr, "x2t_fields: sorted fields: ");
	  for (i = 0; fields[i]; ++i)
	    fprintf(stderr, "%s%s", fields[i], fields[i+1] ? "," : "\n");
	}
    }
  
  r = roco_create(nrec+1, nfld+1);
  r->maxcols = nfld;
  roco_fields_row(r, fields);
  roco_field_indexes(r);
  if (verbose)
    {
      int i;
      fprintf(stderr, "xml2tsv: field/index table:\n");
      for (i = 0; fields[i]; ++i)
	fprintf(stderr, "\t%s\t%d\n",
		fields[i],
		(int)(uintptr_t)hash_find(r->fields, (uccp)fields[i]));
    }

  x2t_load_data(argv[optind], r);
  r->rows[nrec+1] = NULL;
  const char *init_id = NULL;
  if (hash_find(r->fields, (uccp)"id_text"))
    init_id = "id_text";
  else if (hash_find(r->fields, (uccp)"id_composite"))
    init_id = "id_composite";
  else if (hash_find(r->fields, (uccp)"id_attr"))
    init_id = "id_attr";

  if (init_id)
    {
      z_list = list_create(LIST_SINGLE);
      list_add(z_list, (void*)init_id);
      r->fields_from_row1 = 1;
      roco_format = roco_z_format(z_list, r);
      if (verbose)
	fprintf(stderr, "xml2tsv: roco_format=`%s'", roco_format);
    }
  else
    roco_newline = 1;
  roco_write(stdout, r);
}

int
opts(int argc, const char *arg)
{
  switch (argc)
    {
    case 'v':
      verbose = 1;
      break;
    default:
      return 1;
    }
  return 0;
}

void help(void){ }

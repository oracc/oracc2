#include <oraccsys.h>
#include <xmlify.h>
#include <roco.h>
#include <list.h>

int fields_from_row1 = 0;
int suppress_xmlify = 0;
int template_add_empty = 0;
int trtd_output = 0;
int xml_output = 0;
int verbose;

const char *j_file;
List *j_list, *z_list;

const char *xmltag = NULL, *rowtag = NULL, *celtag = NULL, *class = NULL;
const char *template_row;

int
main(int argc, char *const *argv)
{
  Roco *r = NULL, *s = NULL;

  options(argc, argv, "c:C:eEfh::j:J:nor:R:stT:vx:Xz:?");

  if (!xmltag || suppress_xmlify)
    xmlify = xmlify_not;

  if (j_file)
    {
      char **jj = (char**)loadfile_lines3((uccp)j_file, NULL, NULL);
      if (jj)
	j_list = list_from_ary((void**)jj, LIST_SINGLE);
    }
  
  if (j_list)
    {
      r_list = list_create(LIST_SINGLE);
      const char *j;
      for (j = list_first(j_list); j; j = list_next(j_list))
	{
	  Roco *jr = roco_load(j, fields_from_row1, NULL, NULL, NULL, NULL);
	  if (jr)
	    {
	      jr->hdata = roco_hash_r(jr);
	      jr->joiner = 1;
	      roco_empty_row(jr);
	      list_add(r_list, jr);
	    }
	  else
	    fprintf(stderr, "rocox: ignoring empty joiner %s\n", j);
	}
    }
  
  r = roco_load(argv[optind] ? argv[optind] : "-", fields_from_row1, xmltag, rowtag, celtag, class);

  if (template_row)
    {
      roco_newline = 1;
      r->fields_from_row1 = 1;
      char *t = strdup(template_row), *e = t;
      while (*e++)
	if (e[-1] == '+')
	  e[-1] = '\t';
      char **v = vec_from_str(t, NULL, NULL);
      roco_format = roco_row_template(r, v, template_add_empty);
      if (verbose)
	fprintf(stderr, "rocox: template_row => roco_format `%s'\n", roco_format);
      r->row1_literal = 1;
      r->rows[0] = (unsigned char**)v;
      roco_write(stdout, r);
      return 0;
    }


  if (roco_swap_axes)
    r = roco_swap(s=r);

  if (z_list)
    {
      roco_format = roco_z_format(z_list, r);
      if (verbose)
	fprintf(stderr, "rocox: roco_format=`%s'", roco_format);
    }
  
  if (xml_output)
    roco_write_xml(stdout, r);
  else
    roco_write(stdout, r);
}

const char *prog = "rocox";
int major_version = 1, minor_version = 0;
const char *usage_string = "";

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case 'C':
      roco_colorder = arg;
      break;
    case 'c':
      celtag = arg;
      break;
    case 'e':
      roco_esp_ns = 1;
      break;
    case 'E':
      template_add_empty = 1;
      break;
    case 'f':
      fields_from_row1 = 1;
      break;
    case 'h':
      roco_html_ns = 1;
      if (!xmltag)
	{
	  xmltag = "table";
	  rowtag = "tr";
	  celtag = "td";
	  class = arg ? arg : "pretty";
	}
      xml_output = 1;
      break;
    case 'J':
      j_file = arg;
      break;
    case 'j':
      if (!j_list)
	j_list = list_create(LIST_SINGLE);
      list_add(j_list, (void*)arg);
      break;
    case 'n':      
      roco_newline = 1;
      break;
    case 'o':
      roco_ood_ns = 1;
      break;
    case 'R':
      roco_format = arg;
      break;
    case 'r':
      rowtag = arg;
      break;
    case 's':
      roco_swap_axes = 1;
      break;
    case 't':
      if (!xmltag)
	xmltag = "-";
      rowtag = "tr";
      celtag = "td";
      xml_output = 1;
      break;
    case 'T':
      template_row = arg;
      break;
    case 'v':
      verbose = 1;
      break;
    case 'V':
      roco_no_void_xml = 1;
      break;
    case 'x':
      xmltag = arg;
      xml_output = 1;
      break;
    case 'X':
      suppress_xmlify = 1;
      break;
    case 'z':
      if (!z_list)
	z_list = list_create(LIST_SINGLE);
      list_add(z_list, (void*)arg);
      fields_from_row1 = 1;
      break;
    case '?':
      help();
      exit(1);
      break;
    default:
      return 1;
      break;
    }
  return 0;
}

void help (void)
{
}

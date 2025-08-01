#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <oraccsys.h>
#include <xmlify.h>
#include <roco.h>

int fields_from_row1 = 0;
int suppress_xmlify = 0;
int trtd_output = 0;
int xml_output = 0;

const char *xmltag = NULL, *rowtag = NULL, *celtag = NULL, *class = NULL;

int
main(int argc, char *const *argv)
{
  Roco *r = NULL, *s = NULL;
  
  options(argc, argv, "c:C:efh::nor:R:stvx:X?");

  if (!xmltag || suppress_xmlify)
    xmlify = xmlify_not;

  r = roco_load("-", fields_from_row1, xmltag, rowtag, celtag, class);

  if (roco_swap_axes)
    r = roco_swap(s=r);
  
  if (xml_output)
    roco_write_xml(stdout, r);
  else
    roco_write(stdout, r);
}

const char *prog = "rocox";
int major_version = 1, minor_version = 0, verbose;
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
    case 'v':
      roco_no_void_xml = 1;
      break;
    case 'x':
      xmltag = arg;
      xml_output = 1;
      break;
    case 'X':
      suppress_xmlify = 1;
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

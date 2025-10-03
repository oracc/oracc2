void
show_vl(CGI_varlist *vl)
{
  const char *name;
  CGI_value  *value;

  printf("====dumping CGI value list====\n");

  for (name = CGI_first_name(vl); name != 0;
       name = CGI_next_name(vl))
    {
      int i;
      value = CGI_lookup_all(vl, 0);
      for (i = 0; value[i] != 0; i++) 
	printf("%s [%d] >>%s<<\n", name, i, value[i]);
    }

  printf("====done=====\n");
}

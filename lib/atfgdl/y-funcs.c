void
atf_set_tree(Tree *tp)
{
  ytp = tp;
  atf_bld_tree(tp);
}

void
atferror(const char *e)
{
  extern Stck *div_stck;
  Node *np = NULL;

  if (strstr(e, "unexpected end of file")
      && div_stck && ((intptr_t)(np = (Node*)stck_peek(div_stck)) != -1))
    mesg_verr(np->mloc, "atf: unclosed @division\n");
  else
    mesg_vwarning(curratffile, atflineno, "atf: %s\n", e);
}

#ifndef ATF_LINE_H_
#define ATF_LINE_H_

extern void atf_line_lg(Mloc *mp);

#define atf_line_node(ltype)			\
  struct node *lnode = atf_push("l", &l);	\
  lnode->mloc = mloc_mloc(&l);			\
  line_register(l, ltype, lnode)

#define atf_free_wd_list() \
  if (curr_lem_host && curr_lem_host->wl)   \
    {					    \
      list_free(curr_lem_host->wl, NULL);   \
      wd_list = curr_lem_host->wl = NULL;   \
    }					    \

#define atf_line_host() \
  atf_free_wd_list() \
  curr_lem_host = curr_line

#define atf_line_id()							\
  atf_set_line_id(&l);							\
  /* In lg_mode the line group carries the ID that is used for word ID bases--the code here \
     adds an 'l' suffix to the <l> ID then removes it so the right ID base is used for words \
  */									\
  if (lg_mode)								\
    {									\
      (void)strcat(line_id_buf,(const char *)"l");			\
      /*set_tr_id(line_id_buf);*//*re-expose when translations are reimplemented*/ \
    }									\
  atf_xprop(lnode, "xml:id", (clid = (ccp)pool_copy((uccp)line_id_buf, atfmp->pool))); \
  									\
  if (etcsl_labels)							\
    etcsl_lid = (ccp)pool_copy((uccp)line_id_buf, atfmp->pool);		\
  									\
  if (lg_mode)								\
    line_id_buf[strlen(line_id_buf)-1] = '\0';				\
  									\
  if (last_tlit_h_decay)						\
    {									\
      last_tlit_h_decay = 0;						\
      if (!last_tlit_h_decay && lth_used)				\
	{								\
	  struct node **tmp = calloc(lth_used+1,sizeof(struct node*));	\
	  memcpy(tmp,last_tlit_h,lth_used * sizeof(struct node *));	\
	  if (!last_tlit_h_hash)					\
	    last_tlit_h_hash = hash_create(3);				\
	  hash_add(last_tlit_h_hash,(const unsigned char *)clid,tmp);	\
	}								\
      lth_used = 0;							\
    }									\
  atf_xprop(lnode, "n", (ccp)(tok = (ccp)lnstr_tok((char*)tok)))

#define atf_line_label()						\
  curr_line_label = line_label(lnode->mloc,(uccp)tok,0,(unsigned const char *)clid); \
  if (curr_line_label)							\
    {									\
      const char *label;						\
      atf_xprop(lnode, "label", (label = (ccp)pool_copy((uccp)curr_line_label, atfmp->pool))); \
      list_add(atfmp->label_xmlid, keva_create(tm_keva(abt), label, clid)); \
      extern char *label2;						\
      if (label2)							\
	atf_xprop(lnode, "label2", (ccp)pool_copy((uccp)label2, atfmp->pool)); \
									\
      register_label(clid,label);					\
      curr_line_label = ucc(label);					\
    }

#define atf_lg_guard() \
  Node *lg = node_ancestor_or_self(abt->curr, "lg");	\
  if (!lg)						\
    mesg_verr(&l, "misplaced line type: need a '<LINE>. ' first");	\
  else									\
    tree_curr(lg)

#endif/*ATF_LINE_H_*/

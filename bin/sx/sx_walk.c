#include <signlist.h>
#include <sx.h>

/** Walk the signlist letter/group structure and apply the handler
    routines that are passed in via \c sl_functions.
 */
void
sx_walk(struct sx_functions *f, struct sl_signlist *sl)
{
  f->sll(f, sl, sx_pos_init);
  /*f->not(f, sl, sl->notes);*/ /* This should be handled in f->sll because that also processes @listdef and @sysdef */
  if (sl->nletters)
    {
      int i;

      f->let(f, sl, NULL, sx_pos_init);
      for (i = 0; i < sl->nletters; ++i)
	{
	  f->let(f, sl, &sl->letters[i], sx_pos_inst);
	  if (sl->letters[i].ngroups)
	    {
	      int j;
	      f->grp(f, sl, NULL, sx_pos_init);
	      for (j = 0; j < sl->letters[i].ngroups; ++j)
		{
		  f->grp(f, sl, &sl->letters[i].groups[j], sx_pos_inst);
		  if (sl->letters[i].groups[j].nsigns)
		    {
		      int k;
		      for (k = 0; k < sl->letters[i].groups[j].nsigns; ++k)
			{
			  if (sl->h_kdata
			      && (!sl->letters[i].groups[j].signs[k]->key
				  || (!hash_find(sl->h_kdata,
						 sl->letters[i].groups[j].signs[k]->key))))
			    continue;
			  
#define QV(vp) ((vp)->u.v->qvsign || (vp)->u.v->qvform || (vp)->u.v->qvmust)
			  List *qv = list_create(LIST_SINGLE);
			  f->sgn(f, sl, sl->letters[i].groups[j].signs[k], sx_pos_init);
			  f->sgn(f, sl, sl->letters[i].groups[j].signs[k], sx_pos_inst);
			  if ('s' == sl->letters[i].groups[j].signs[k]->type
			      && sl->letters[i].groups[j].signs[k]->valid)
			    {
			      if (sl->letters[i].groups[j].signs[k]->u.s->nlists)
				{
				  int l;
				  f->lst(f, sl, NULL, sx_pos_init);
				  for (l = 0; l < sl->letters[i].groups[j].signs[k]->u.s->nlists; ++l)
				    {
				      f->lst(f, sl, sl->letters[i].groups[j].signs[k]->u.s->lists[l], sx_pos_inst);
				      f->not(f, sl, sl->letters[i].groups[j].signs[k]->u.s->lists[l]);
				    }
				  f->lst(f, sl, NULL, sx_pos_term);
				}
			      f->not(f, sl, sl->letters[i].groups[j].signs[k]);
			      f->uni(f, sl, &sl->letters[i].groups[j].signs[k]->u.s->U);
			      if (sl->letters[i].groups[j].signs[k]->lp)
				f->lem(f,sl,sl->letters[i].groups[j].signs[k]);
			      if (sl->letters[i].groups[j].signs[k]->u.s->nvalues)
				{
				  int l;
				  f->val(f, sl, NULL, sx_pos_init);
				  for (l = 0; l < sl->letters[i].groups[j].signs[k]->u.s->nvalues; ++l)
				    {
				      f->val(f, sl, sl->letters[i].groups[j].signs[k]->u.s->values[l], sx_pos_inst);
				      if (QV(sl->letters[i].groups[j].signs[k]->u.s->values[l]))
					list_add(qv, sl->letters[i].groups[j].signs[k]->u.s->values[l]);
				      f->not(f, sl, sl->letters[i].groups[j].signs[k]->u.s->values[l]);
				    }
				  f->val(f, sl, NULL, sx_pos_term);
				}
			      if (sl->letters[i].groups[j].signs[k]->sys)
				f->sys(f, sl, sl->letters[i].groups[j].signs[k]);
			      if (sl->letters[i].groups[j].signs[k]->links)
				f->lnk(f, sl, sl->letters[i].groups[j].signs[k]);
			      if (sl->images && f->img)
				f->img(f, sl, sl->letters[i].groups[j].signs[k]);
			      if (sl->letters[i].groups[j].signs[k] && f->cpd)
				f->cpd(f, sl, sl->letters[i].groups[j].signs[k]);
			      if (sl->letters[i].groups[j].signs[k]->u.s->nforms)
				{
				  int l;
				  f->frm(f, sl, NULL, sx_pos_init);
				  for (l = 0; l < sl->letters[i].groups[j].signs[k]->u.s->nforms; ++l)
				    {
				      if (sl->h_kdata
					  && (!sl->letters[i].groups[j].signs[k]->u.s->forms[l]->key
					      || (!hash_find(sl->h_kdata,
							     sl->letters[i].groups[j].signs[k]->u.s->forms[l]->key))))
					continue;

				      f->frm(f, sl, sl->letters[i].groups[j].signs[k]->u.s->forms[l], sx_pos_inst);
				      if (sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->nlists)
					{
					  int m;
					  f->lst(f, sl, NULL, sx_pos_init);
					  for (m = 0; m < sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->nlists; ++m)
					    {
					      f->lst(f, sl, sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->lists[m],
						     sx_pos_inst);
					      f->not(f, sl, sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->lists[m]);
					    }
					  f->lst(f, sl, NULL, sx_pos_term);
					}
				      f->not(f, sl, sl->letters[i].groups[j].signs[k]->u.s->forms[l]);
				      f->uni(f, sl, &sl->letters[i].groups[j].signs[k]->u.s->forms[l]->u.f->sign->U);
				      if (sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->nvalues
					  && (sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->nvalues
					      != sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->nivalues))
					{
					  int m;
					  f->val(f, sl, NULL, sx_pos_init);
					  for (m = 0; m < sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->nvalues; ++m)
					    {
					      struct sl_inst *v = sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->values[m];
					      if (!v->inherited)
						{
						  f->val(f, sl, v, sx_pos_inst);
						  f->not(f, sl, v);
						}
					      else if (v->inherited && v->u.v->atf)
						{
						  f->val(f, sl, v, sx_pos_inst);
						  f->not(f, sl, v);
						}
					      if (QV(sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->values[m]))
						list_add(qv, sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->values[m]);
					    }
					  f->val(f, sl, NULL, sx_pos_term);
					}
				      if (sl->letters[i].groups[j].signs[k]->u.s->forms[l]->sys)
					f->sys(f, sl, sl->letters[i].groups[j].signs[k]->u.s->forms[l]);
				      if (sl->letters[i].groups[j].signs[k]->u.s->forms[l]->links)
					f->lnk(f, sl, sl->letters[i].groups[j].signs[k]->u.s->forms[l]);
				      if (sl->images && f->img)
					f->img(f, sl, sl->letters[i].groups[j].signs[k]->u.s->forms[l]);
				      /* Inherited values are stored in lv->values but lv->nivalues
					 has a count of them so we can avoid generating empty <inherited/> nodes */
				      if (sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->nivalues)
					{
					  int m;
					  f->inh(f, sl, NULL, sx_pos_init);
					  for (m = 0; m < sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->nvalues; ++m)
					    if (sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->values[m]->inherited)
					      f->inh(f, sl, sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->values[m],
						     sx_pos_inst);
					  f->inh(f, sl, NULL, sx_pos_term);
					}
				    }
				  f->frm(f, sl, sl->letters[i].groups[j].signs[k], sx_pos_term);
				}
			    }
#undef QV
			  if (list_len(qv))
			    {
			      struct sl_inst *ip;
			      f->qvs(f, sl, NULL, sx_pos_init);
			      for (ip = list_first(qv); ip; ip = list_next(qv))
				f->qvs(f, sl, ip, sx_pos_inst);
			      f->qvs(f, sl, NULL, sx_pos_term);
			    }
			  list_free(qv,NULL);
			  f->sgn(f, sl, sl->letters[i].groups[j].signs[k], sx_pos_term);
			}
		    }
		}
	      f->grp(f, sl, NULL, sx_pos_term);
	    }
	}
      f->let(f, sl, NULL, sx_pos_term);
    }
  f->sll(f, sl, sx_pos_term);
}

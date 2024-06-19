#include <oraccsys.h>
#include <xmlify.h>
#include "iso/iso.h"
#include "isp/isp.h"
#include "iss/iss.h"
void
pr_isp_list_loc(FILE *fp, struct isp_list_loc *pr, const char *name)
{
  fprintf(fp, "<h3>\tstruct isp_list_loc ip->%s</h3>\n", name);
  fprintf(fp,"\t\ttype=%s\n",(char*)xmlify((ucp)pr->type));
  fprintf(fp,"\t\tlang=%s\n",(char*)xmlify((ucp)pr->lang));
  fprintf(fp,"\t\tmethod=%s\n",(char*)xmlify((ucp)pr->method));
  fprintf(fp,"\t\tdbpath=%s\n",(char*)xmlify((ucp)pr->dbpath));
  fprintf(fp,"\t\tdbname=%s\n",(char*)xmlify((ucp)pr->dbname));
  fprintf(fp,"\t\tpath=%s\n",(char*)xmlify((ucp)pr->path));
}

void
pr_isp_srchdata(FILE *fp, struct isp_srchdata *pr, const char *name)
{
  fprintf(fp, "<h3>\tstruct isp_srchdata ip->%s</h3>\n", name);
  fprintf(fp,"\t\ttmp=%s\n",(char*)xmlify((ucp)pr->tmp));
  fprintf(fp,"\t\tbar=%s\n",(char*)xmlify((ucp)pr->bar));
  fprintf(fp,"\t\tcount=%lu\n",(unsigned long)pr->count);
  fprintf(fp,"\t\tlist=%s\n",(char*)xmlify((ucp)pr->list));
  fprintf(fp,"\t\tnew=%d\n",(int)pr->new);
}

void
pr_isp_cache(FILE *fp, struct isp_cache *pr, const char *name)
{
  fprintf(fp, "<h3>\tstruct isp_cache ip->%s</h3>\n", name);
  fprintf(fp,"\t\tsys=%s\n",(char*)xmlify((ucp)pr->sys));
  fprintf(fp,"\t\tproject=%s\n",(char*)xmlify((ucp)pr->project));
  fprintf(fp,"\t\tsub=%s\n",(char*)xmlify((ucp)pr->sub));
  fprintf(fp,"\t\tout=%s\n",(char*)xmlify((ucp)pr->out));
  fprintf(fp,"\t\tlist=%s\n",(char*)xmlify((ucp)pr->list));
  fprintf(fp,"\t\tsort=%s\n",(char*)xmlify((ucp)pr->sort));
  fprintf(fp,"\t\tcsi=%s\n",(char*)xmlify((ucp)pr->csi));
  fprintf(fp,"\t\ttsv=%s\n",(char*)xmlify((ucp)pr->tsv));
  fprintf(fp,"\t\tmax=%s\n",(char*)xmlify((ucp)pr->max));
  fprintf(fp,"\t\tmol=%s\n",(char*)xmlify((ucp)pr->mol));
  fprintf(fp,"\t\tpkey=%s\n",(char*)xmlify((ucp)pr->pkey));
  fprintf(fp,"\t\tpgin=%s\n",(char*)xmlify((ucp)pr->pgin));
  fprintf(fp,"\t\tpage=%s\n",(char*)xmlify((ucp)pr->page));
  fprintf(fp,"\t\tzout=%s\n",(char*)xmlify((ucp)pr->zout));
  fprintf(fp,"\t\titem=%s\n",(char*)xmlify((ucp)pr->item));
  fprintf(fp,"\t\tprox=%s\n",(char*)xmlify((ucp)pr->prox));
  fprintf(fp,"\t\tmeta=%s\n",(char*)xmlify((ucp)pr->meta));
  fprintf(fp,"\t\thtml=%s\n",(char*)xmlify((ucp)pr->html));
  fprintf(fp,"\t\tltab=%s\n",(char*)xmlify((ucp)pr->ltab));
  fprintf(fp,"\t\thilite=%s\n",(char*)xmlify((ucp)pr->hilite));
  fprintf(fp,"\t\tpub=%s\n",(char*)xmlify((ucp)pr->pub));
  fprintf(fp,"\t\tuse=%s\n",(char*)xmlify((ucp)pr->use));
  fprintf(fp,"\t\ttxtindex=%s\n",(char*)xmlify((ucp)pr->txtindex));
  fprintf(fp,"\t\tt_sort=%s\n",(char*)xmlify((ucp)pr->t_sort));
  fprintf(fp,"\t\tt_tsv=%s\n",(char*)xmlify((ucp)pr->t_tsv));
  fprintf(fp,"\t\tt_max=%s\n",(char*)xmlify((ucp)pr->t_max));
  fprintf(fp,"\t\tt_mol=%s\n",(char*)xmlify((ucp)pr->t_mol));
}

void
pr_item(FILE *fp, struct item *pr, const char *name)
{
  fprintf(fp, "<h3>\tstruct item ip->%s</h3>\n", name);
  fprintf(fp,"\t\ts=%s\n",(char*)xmlify((ucp)pr->s));
  fprintf(fp,"\t\tpq=%s\n",(char*)xmlify((ucp)pr->pq));
  fprintf(fp,"\t\tqpq=%s\n",(char*)xmlify((ucp)pr->qpq));
  fprintf(fp,"\t\tskey=%d\n",(int)pr->skey);
  fprintf(fp,"\t\tlkey=%d\n",(int)pr->lkey);
  fprintf(fp,"\t\tgrp=%d\n",(int)pr->grp);
}

void
pr_isp_mapdata(FILE *fp, struct isp_mapdata *pr, const char *name)
{
  fprintf(fp, "<h3>\tstruct isp_mapdata ip->%s</h3>\n", name);
  fprintf(fp,"\t\tzmax=%d\n",(int)pr->zmax);
  fprintf(fp,"\t\tzimx=%d\n",(int)pr->zimx);
  fprintf(fp,"\t\thtell=%lu\n",(unsigned long)pr->htell);
  fprintf(fp,"\t\thlen=%d\n",(int)pr->hlen);
  fprintf(fp,"\t\tptell=%lu\n",(unsigned long)pr->ptell);
  fprintf(fp,"\t\tplen=%d\n",(int)pr->plen);
}

void
pr_isp_itemdata(FILE *fp, struct isp_itemdata *pr, const char *name)
{
  fprintf(fp, "<h3>\tstruct isp_itemdata ip->%s</h3>\n", name);
  fprintf(fp,"\t\tlangs=%s\n",(char*)xmlify((ucp)pr->langs));
  fprintf(fp,"\t\tnlangs=%lu\n",(unsigned long)pr->nlangs);
  fprintf(fp,"\t\txtflang=%s\n",(char*)xmlify((ucp)pr->xtflang));
  fprintf(fp,"\t\tlmem=%s\n",(char*)xmlify((ucp)pr->lmem));
  fprintf(fp,"\t\titem=%s\n",(char*)xmlify((ucp)pr->item));
  fprintf(fp,"\t\tblock=%s\n",(char*)xmlify((ucp)pr->block));
  fprintf(fp,"\t\tproj=%s\n",(char*)xmlify((ucp)pr->proj));
  fprintf(fp,"\t\thtmd=%s\n",(char*)xmlify((ucp)pr->htmd));
  fprintf(fp,"\t\thtml=%s\n",(char*)xmlify((ucp)pr->html));
  fprintf(fp,"\t\tdots=%s\n",(char*)xmlify((ucp)pr->dots));
  fprintf(fp,"\t\tindex=%s\n",(char*)xmlify((ucp)pr->index));
  fprintf(fp,"\t\tpage=%s\n",(char*)xmlify((ucp)pr->page));
  fprintf(fp,"\t\tpindex=%s\n",(char*)xmlify((ucp)pr->pindex));
  fprintf(fp,"\t\tzoom=%s\n",(char*)xmlify((ucp)pr->zoom));
  fprintf(fp,"\t\tzpag=%s\n",(char*)xmlify((ucp)pr->zpag));
  fprintf(fp,"\t\tzindex=%s\n",(char*)xmlify((ucp)pr->zindex));
  fprintf(fp,"\t\tprev=%s\n",(char*)xmlify((ucp)pr->prev));
  fprintf(fp,"\t\tnext=%s\n",(char*)xmlify((ucp)pr->next));
  fprintf(fp,"\t\ttmax=%s\n",(char*)xmlify((ucp)pr->tmax));
  fprintf(fp,"\t\txmdxsl=%s\n",(char*)xmlify((ucp)pr->xmdxsl));
  fprintf(fp,"\t\tbld=%s\n",(char*)xmlify((ucp)pr->bld));
  fprintf(fp,"\t\tnot=%d\n",(int)pr->not);
}

void
pr_isp_glosdata(FILE *fp, struct isp_glosdata *pr, const char *name)
{
  fprintf(fp, "<h3>\tstruct isp_glosdata ip->%s</h3>\n", name);
  fprintf(fp,"\t\tdir=%s\n",(char*)xmlify((ucp)pr->dir));
  fprintf(fp,"\t\tweb=%s\n",(char*)xmlify((ucp)pr->web));
  fprintf(fp,"\t\tlet=%s\n",(char*)xmlify((ucp)pr->let));
  fprintf(fp,"\t\tlmax=%s\n",(char*)xmlify((ucp)pr->lmax));
  fprintf(fp,"\t\tent=%s\n",(char*)xmlify((ucp)pr->ent));
  fprintf(fp,"\t\txis=%s\n",(char*)xmlify((ucp)pr->xis));
  fprintf(fp,"\t\tltab=%s\n",(char*)xmlify((ucp)pr->ltab));
  fprintf(fp,"\t\tlbase=%s\n",(char*)xmlify((ucp)pr->lbase));
  fprintf(fp,"\t\tlpath=%s\n",(char*)xmlify((ucp)pr->lpath));
  fprintf(fp,"\t\tecpath=%s\n",(char*)xmlify((ucp)pr->ecpath));
  fprintf(fp,"\t\temax=%s\n",(char*)xmlify((ucp)pr->emax));
  fprintf(fp,"\t\tipath=%s\n",(char*)xmlify((ucp)pr->ipath));
}

void
pr_isp_config(FILE *fp, struct isp_config *pr, const char *name)
{
  fprintf(fp, "<h3>\tstruct isp_config ip->%s</h3>\n", name);
  fprintf(fp,"\t\tleftmenu=%d\n",(int)pr->leftmenu);
  fprintf(fp,"\t\tselect=%d\n",(int)pr->select);
  fprintf(fp,"\t\tsort_fields=%s\n",(char*)xmlify((ucp)pr->sort_fields));
  fprintf(fp,"\t\tsort_labels=%s\n",(char*)xmlify((ucp)pr->sort_labels));
  fprintf(fp,"\t\thead_template=%s\n",(char*)xmlify((ucp)pr->head_template));
  fprintf(fp,"\t\tcat_fields=%s\n",(char*)xmlify((ucp)pr->cat_fields));
  fprintf(fp,"\t\tcat_links=%s\n",(char*)xmlify((ucp)pr->cat_links));
  fprintf(fp,"\t\tcat_widths=%s\n",(char*)xmlify((ucp)pr->cat_widths));
}

void
px_print_ip(FILE *fp, Isp *ip)
{
  Isp *pr = ip;
  fputs("<h2>Internal Data Structure State (Isp *ip)</h2>\n", fp);
  fprintf(fp,"\toracc=%s\n",(char*)xmlify((ucp)pr->oracc));
  fprintf(fp,"\tfrom=%s\n",(char*)xmlify((ucp)pr->from));
  fprintf(fp,"\tdata=%s\n",(char*)xmlify((ucp)pr->data));
  fprintf(fp,"\tshow=%s\n",(char*)xmlify((ucp)pr->show));
  fprintf(fp,"\tproject=%s\n",(char*)xmlify((ucp)pr->project));
  fprintf(fp,"\tprojdir=%s\n",(char*)xmlify((ucp)pr->projdir));
  fprintf(fp,"\tlist_name=%s\n",(char*)xmlify((ucp)pr->list_name));
  fprintf(fp,"\top_nlevels=%d\n",(int)pr->op_nlevels);
  fprintf(fp,"\tdors=%s\n",(char*)xmlify((ucp)pr->dors));
  fprintf(fp,"\tperm=%s\n",(char*)xmlify((ucp)pr->perm));
  fprintf(fp,"\tzoom=%s\n",(char*)xmlify((ucp)pr->zoom));
  fprintf(fp,"\tpage=%s\n",(char*)xmlify((ucp)pr->page));
  fprintf(fp,"\tpsiz=%s\n",(char*)xmlify((ucp)pr->psiz));
  fprintf(fp,"\tsrch=%s\n",(char*)xmlify((ucp)pr->srch));
  fprintf(fp,"\tglos=%s\n",(char*)xmlify((ucp)pr->glos));
  fprintf(fp,"\titem=%s\n",(char*)xmlify((ucp)pr->item));
  fprintf(fp,"\thili=%s\n",(char*)xmlify((ucp)pr->hili));
  fprintf(fp,"\titem_replace=%s\n",(char*)xmlify((ucp)pr->item_replace));
  fprintf(fp,"\tbkmk=%s\n",(char*)xmlify((ucp)pr->bkmk));
  fprintf(fp,"\tceid=%s\n",(char*)xmlify((ucp)pr->ceid));
  fprintf(fp,"\tcemd=%s\n",(char*)xmlify((ucp)pr->cemd));
  fprintf(fp,"\tlang=%s\n",(char*)xmlify((ucp)pr->lang));
  fprintf(fp,"\txhmd=%s\n",(char*)xmlify((ucp)pr->xhmd));
  fprintf(fp,"\tpart=%s\n",(char*)xmlify((ucp)pr->part));
  fprintf(fp,"\tform=%s\n",(char*)xmlify((ucp)pr->form));
  fprintf(fp,"\twhat=%s\n",(char*)xmlify((ucp)pr->what));
  fprintf(fp,"\tuimd=%s\n",(char*)xmlify((ucp)pr->uimd));
  fprintf(fp,"\tpack=%s\n",(char*)xmlify((ucp)pr->pack));
  fprintf(fp,"\thost=%s\n",(char*)xmlify((ucp)pr->host));
  fprintf(fp,"\taapi=%s\n",(char*)xmlify((ucp)pr->aapi));
  fprintf(fp,"\thost_path=%s\n",(char*)xmlify((ucp)pr->host_path));
  fprintf(fp,"\tsig=%s\n",(char*)xmlify((ucp)pr->sig));
  fprintf(fp,"\ttmp_dir=%s\n",(char*)xmlify((ucp)pr->tmp_dir));
  fprintf(fp,"\terr=%s\n",(char*)xmlify((ucp)pr->err));
  fprintf(fp,"\terrx=%s\n",(char*)xmlify((ucp)pr->errx));
  fprintf(fp,"\tforce=%d\n",(int)pr->force);
  fprintf(fp,"\tolev=%d\n",(int)pr->olev);
  fprintf(fp,"\tdebug=%d\n",(int)pr->debug);
  fprintf(fp,"\tpub_output=%d\n",(int)pr->pub_output);
  fprintf(fp,"\tverbose=%d\n",(int)pr->verbose);
  fprintf(fp,"\tweb=%d\n",(int)pr->web);
  fprintf(fp,"\tzlev=%d\n",(int)pr->zlev);
  fprintf(fp,"\targc=%d\n",(int)pr->argc);
  fprintf(fp,"\treferer=%s\n",(char*)xmlify((ucp)pr->referer));
  fprintf(fp,"\ttmpdir=%s\n",(char*)xmlify((ucp)pr->tmpdir));
  pr_isp_cache(fp, &ip->cache, "cache");
  pr_isp_config(fp, &ip->default_cfg, "default_cfg");
  pr_isp_config(fp, &ip->special_cfg, "special_cfg");
  pr_isp_glosdata(fp, &ip->glosdata, "glosdata");
  pr_isp_itemdata(fp, &ip->itemdata, "itemdata");
  pr_isp_list_loc(fp, &ip->lloc, "lloc");
  pr_isp_srchdata(fp, &ip->srchdata, "srchdata");
  fprintf(fp,"\tzmax=%d\n",(int)pr->zmax);
}

void
pr_ispargstab(FILE *fp, struct ispargstab *pr, const char *name)
{
  fprintf(fp, "<h3>\tstruct ispargstab ip->%s</h3>\n", name);
  fprintf(fp,"\t\tname=%s\n",(char*)xmlify((ucp)pr->name));
  fprintf(fp,"\t\tstep=%d\n",(int)pr->step);
}


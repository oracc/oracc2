#include <oraccsys.h>
#include <signlist.h>
#include <sx.h>



/**sx_images -- load the image manifests into a Roco
 *
 * -the number of columns is the number of image manifests;
 * -the number of rows is the number of OIDs referenced in oidindexes
 * -the value of oidindexes{OID} is the integer index into the rows
 * -each entry in a manifest is stored in row[oidindexes{OID}][manifest-index]
 *
 */
void
sx_images(struct sl_signlist *sl)
{
  if (sl->images)
    {
      int ncols = list_len(sl->images)+1;
      int nrows = sl->oidindexes->key_count+1;
      Roco *r = roco_create(nrows, ncols+1); /* we added one for the OID;
						this +1 is for the NULL
						term on each row of cols */
      Mloc *m;
      int nm;
      sl->iheaders = calloc(list_len(sl->images), sizeof(struct sx_iheader));
      r->linkcells = 1;
      for (m = list_first(sl->images), nm=0; m; m = list_next(sl->images), ++nm)
	{
	  const char *improj = NULL;
	  const char *imfile = m->user;
	  int sparse_flag = 0; /* means ignore OIDs that aren't found in glossary */
	  if ('-' == *imfile)
	    {
	      sparse_flag = 1;
	      ++imfile;
	    }
	  if (access(imfile, R_OK))
	    {
	      char buf[strlen(oracc())+strlen("/lib/data/0")+strlen(imfile)];
	      sprintf(buf, "%s/lib/data/%s", oracc(), imfile);
	      if (access(buf, R_OK))
		{
		  fprintf(stderr, "sx: @img file %s or %s not readable; ignoring entry\n", imfile, buf);
		  continue;
		}
	      imfile = pool_copy(buf, sl->p);
	      improj = "@@oracc@@";
	    }
	  Roco *mr = roco_load(imfile, 0, NULL, NULL, NULL);
	  char buf[16];
	  sprintf(buf, "i%d", nm);
	  sl->iheaders[nm].r = mr;
	  sl->iheaders[nm].id = (ccp)pool_copy((uccp)buf, sl->p);
	  sl->iheaders[nm].mloc = *m;
	  sl->iheaders[nm].sparse = sparse_flag;
	  if (improj)
	    sl->iheaders[nm].proj = (ccp)pool_copy((uccp)improj, sl->p);
	  else
	    sl->iheaders[nm].proj = sl->project;
	  if (mr)
	    {
	      int i;
	      for (i = 0; i < mr->nlines; ++i)
		{
		  switch (*mr->rows[i][0])
		    {
		    case '@':
		      if (!strcmp((ccp)mr->rows[i][0], "@label"))
			sl->iheaders[nm].label = (ccp)mr->rows[i][1];
		      else if (!strcmp((ccp)mr->rows[i][0], "@title"))
			sl->iheaders[nm].title = (ccp)mr->rows[i][1];
		      else if (!strcmp((ccp)mr->rows[i][0], "@page"))
			sl->iheaders[nm].page = (ccp)mr->rows[i][1];
		      else if (!strcmp((ccp)mr->rows[i][0], "@path"))
			sl->iheaders[nm].path = (ccp)mr->rows[i][1];
		      else if (!strcmp((ccp)mr->rows[i][0], "@thumb"))
			sl->iheaders[nm].thumb = (ccp)mr->rows[i][1];
		      else if (!strcmp((ccp)mr->rows[i][0], "@list"))
			sl->iheaders[nm].list = (ccp)mr->rows[i][1];
		      else if (!strcmp((ccp)mr->rows[i][0], "@font"))
			{
			  sl->iheaders[nm].path = (ccp)mr->rows[i][1];
			  sl->iheaders[nm].type = IMH_FONT;
			}
		      else if (!strcmp((ccp)mr->rows[i][0], "@css"))
			sl->iheaders[nm].css = (ccp)mr->rows[i][1];
		      else if (!strcmp((ccp)mr->rows[i][0], "@mag"))
			sl->iheaders[nm].mag = (ccp)mr->rows[i][1];
		      else
			fprintf(stderr, "%s:%d: %s: unknown @-command in image map\n", imfile, i+1, mr->rows[i][0]);
		      break;
		    case '#':
		      break;
		    default:
		      {
			int s = 0;
			if ((s = (uintptr_t)hash_find(sl->oidindexes, mr->rows[i][0])))
			  {
			    Link *lp = memo_new(sl->m_links);
			    if (!r->rows[s-1][0])
			      {
				lp->data = mr->rows[i][0];
				r->rows[s-1][0] = (ucp)lp;
			      }
			    lp = memo_new(sl->m_links);
			    lp->data = mr->rows[i]/*[1]*/; /* after row[0] the link data is row not cell */
			    if (r->rows[s-1][nm+1])
			      {
				Link *xp = (Link*)r->rows[s-1][nm+1];
				while (xp->next)
				  xp = xp->next;
				xp->next = lp;
			      }
			    else
			      r->rows[s-1][nm+1] = (ucp)lp;
			  }
			else if (!sl->iheaders[nm].sparse)
			  fprintf(stderr, "%s:%d: no OID found for %s or %s; s=%d\n",
				  mr->file, i, (char*)mr->rows[i][0], (char*)mr->rows[i][1], s);
		      }
		    }
		}
	    }
	  if (!sl->iheaders[nm].label || !sl->iheaders[nm].path)
	    mesg_verr(m, "image table loaded via @images %s must contain both @label and @path lines", mr->file);
	}
      sl->iarray = r;
    }
}

void
sx_images_dump(FILE *fp, struct sl_signlist *sl)
{
  roco_write(fp, sl->iarray);
}

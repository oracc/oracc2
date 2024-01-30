#include <oraccsys.h>
#include "vido.h"

/* VIDO = Virtualized ID Organizer -- derived from the Oracc1 vid
   subsystem as used by the search engine but generalized and with
   some facilities removed */

static char *basebuf = NULL;
static int basebuf_alloced = 0;

static void vido_hash_data(Vido *vp);

#undef VIDO_DEBUG

Vido *
vido_init(char prefix, int dotstop)
{
  Vido *vp;
  pool_init();
  vp = calloc(1,sizeof(Vido));
  vp->prefix = prefix;
  vp->dotstop = dotstop;
  vp->vidh = hash_create(1000);
  vp->pool = pool_init();
  vp->seen = hash_create(1000);
  return vp;
}

void
vido_term(Vido *vp)
{
  if (vp)
    {
      hash_free(vp->vidh,NULL);  
      hash_free(vp->seen,NULL);
      pool_term(vp->pool);
      free(vp->ids);
      free(vp);
    }
}

const char *
vido_new_id(Vido *vp, const char *xid)
{
  const char *pd = NULL;
  char *vidp;
  int len;

  if (vp->dotstop && ((pd = strchr(xid,'.'))))
    len = pd-xid;
  else
    len = strlen(xid);
  
  if (len > (basebuf_alloced-1))
    {
      basebuf_alloced = (len*2) + 1;
      basebuf = realloc(basebuf,basebuf_alloced);
    }
  memcpy(basebuf,xid,len);
  basebuf[len] = '\0';

#ifdef VIDO_DEBUG
  fprintf(stderr,"vido_new_id: looking for %s to resolve %s\n",basebuf,xid);
#endif

  if (!(vidp = hash_find(vp->vidh,(unsigned char *)basebuf)))
    {
      char vido_buf[8];
      char *xid_pool = (char *)pool_copy((unsigned char *)basebuf, vp->pool);
      sprintf(vido_buf,"%c%06d",vp->prefix,vp->ids_used);
      hash_add(vp->vidh,
	       pool_copy((unsigned char *)basebuf, vp->pool),
	       pool_copy((unsigned char *)vido_buf, vp->pool));
      vidp = vido_buf;
      if (vp->ids_used == vp->ids_alloced)
	{
	  vp->ids_alloced += 256;
	  vp->ids = realloc(vp->ids,vp->ids_alloced * sizeof(const char *));
	}
      vp->ids[vp->ids_used++] = xid_pool;
      if (strlen(basebuf) >= vp->max_len)
	vp->max_len = strlen(basebuf) + 1;
      
#ifdef VIDO_DEBUG
  fprintf(stderr,"vido_new_id: mapped %s to %s\n",basebuf,vidp);
#endif

    }
  return vidp;
}

const char *
vido_get_id(Vido *vp, int vid)
{
  return &vp->padded_ids[vid * vp->max_len];
}

void
vido_dump_data(Vido *vp, const char *fname, const char *tsvname)
{
  char *buf;
  FILE *dp = NULL;
  FILE *tp = NULL;
  int i;

  if (fname)
    dp = fopen(fname,"wb");
  if (tsvname)
    tp = fopen(tsvname,"w");
  
  if (fname && !dp)
    {
      fprintf(stderr, "vido: can't open '%s' to write data\n",fname);
      return;
    }
  if (tsvname && !tp)
    {
      fprintf(stderr, "vido: can't open '%s' to write table\n",tsvname);
      return;
    }
  if (dp)
    fprintf(dp, "%c%c%d%c%d%c", vp->prefix,0,vp->ids_used,0,vp->max_len,0);
  buf = malloc(vp->max_len);
  for (i = 0; i < vp->ids_used; ++i)
    {
      memset(buf,'\0',vp->max_len);
      strcpy(buf,vp->ids[i]);
      if (dp)
	fwrite(buf,1,vp->max_len,dp);
      if (tp)
	fprintf(tp, "%c%06d\t%s\n", vp->prefix, i, buf);
    }
  fclose(dp);
  fclose(tp);
}

Vido *
vido_load_data(const char *fname, int hash_too)
{
  Vido *vp = NULL;
  size_t fsize;
  char *vpfile = (char*)loadfile((unsigned char *)fname,&fsize);
  if (vpfile)
    {
      vp = calloc(1,sizeof(Vido));
      char *maxp = vpfile + strlen(vpfile) + 1;
      vp->prefix = *vpfile;
      vpfile += 2;
      vp->ids_used = atoi(vpfile);
      vp->max_len = atoi(maxp);
      vp->padded_ids = maxp + strlen(maxp) + 1;
      vp->file = vpfile;
      vp->ids = NULL;
      if (hash_too)
	{
	  vp->pool = pool_init();
	  vp->seen = hash_create(1000);
	  vido_hash_data(vp);
	}
    }
  return vp;
}

static void
vido_hash_data(Vido *vp)
{
  int i = 0;
  char *keybuf = malloc(vp->max_len+1);
  const char *atptr = NULL;
  unsigned char *keyptr;
  vp->vpool = malloc(vp->ids_used * 8);
  vp->ids = malloc(vp->ids_used * sizeof(char*));
  vp->vidh = hash_create(vp->ids_used);
  vp->pool = pool_init();
  for (i = 0; i < vp->ids_used; ++i)
    {
      vp->ids[i] = &vp->vpool[i*8];
      sprintf(vp->ids[i], "%c%06d", vp->prefix, i);
      keyptr = (unsigned char *)&vp->padded_ids[i*vp->max_len];
      if ((atptr = strchr((const char*)keyptr, '@')))
	{
	  /* This is a double-qualified entry: the first project, before the '@',
	     is the one for XTF files; the second, after the '@', is for XMD files.
	     We make entries for both in the hash so that any indexer finds the 
	     common VID.  This means that when the VID is dereferenced the search
	     engine needs to select the correct project when handed a double-qualified
	     QID */
	  const char *colon = NULL;
	  strcpy(keybuf, (const char *)keyptr);
	  colon = strchr((const char *)keyptr, ':'); /* colon can't point at keybuf because overlapping copy */
	  strcpy(keybuf + (atptr-(const char*)keyptr), colon);
	  hash_add(vp->vidh, 
		   pool_copy((const unsigned char*)keybuf,vp->pool),
		   vp->ids[i]);
	  hash_add(vp->vidh, 
		   (const unsigned char*)(atptr+1),
		   vp->ids[i]);
	}
      else
	{
	  hash_add(vp->vidh, 
		   keyptr,
		   vp->ids[i]);
	}
    }
  free(keybuf);
}

void
vido_free(Vido*vp)
{
  if (vp)
    {
      if (vp->vidh)
	hash_free(vp->vidh,NULL);
      if (vp->vpool)
	free(vp->vpool);
      if (vp->ids)
	free(vp->ids);
      free(vp->file);
    }
}

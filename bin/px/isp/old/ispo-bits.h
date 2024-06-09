/* Top-level structure for the outline */
struct ispo
{
  unsigned char **zlines;
  size_t zmax;
  int zlev;
  List *l; /* data items are isph w level = 1 */
};

/* Per-heading structure for a zoom line */
struct isph
{
  int zoom;
  int level;
  unsigned char *h;
  int zimx; /* z-item max */
  List *l;
};


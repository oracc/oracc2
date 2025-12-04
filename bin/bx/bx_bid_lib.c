const char *BID_STOPS = "{}, \t\n";
const char *
bx_bid_first(const char *ids)
{
  const char *B, *s = ids;
  
  while ((B = strchr(s, 'B')))
    {
      if (B == s || strchr(BID_STOPS, B[-1]))
	{
	  const char *b = B;
	  while (isdigit(b[1]))
	    ++b;
	  if (!*b || strchr(BID_STOPS, b[1]))
	    return B;
	}
    }
  return NULL;
}

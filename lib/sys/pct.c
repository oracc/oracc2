double
pctd(double amount, double total) {
  return (amount / total) * 100;
}

int
pctint(int amount, int total)
{
  float i = amount;
  i /= total;
  i *= 100;
  return (int)(i+.5);
}

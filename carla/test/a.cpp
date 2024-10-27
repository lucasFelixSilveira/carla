void u(int v, int *w) {
  *w = v;
  return;
}

int
main()
{
  int x = 0;
  int y = 2;
  u(y, &x);
  return x;
}
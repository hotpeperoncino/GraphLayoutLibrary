#include <LayoutManager/LayoutManager.h>

int main(int ac, char **av)
{
  LayoutManager layoutManager;
  const char *inf = av[1];
  const char *outf = av[2];
  layoutManager.applyGridBasedLayout(inf, outf);
  return 0;
}


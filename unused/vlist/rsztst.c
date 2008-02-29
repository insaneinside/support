#include <stdio.h>
#include "vlist.h"

#define printlist(list) _printlist(list,#list)

void
_printlist(void **list, char *title)
{
  int i, sz;
  sz = vlist_size(list);
  if(sz < 1)
    {
      fprintf(stderr, "vlist_size(%s) < 1; nothing to print.\n", title);
      return;
    }
    
  for ( i = 0; i < sz; i++ )
    printf("%s[%d]= \"%s\"\n",title,i,(char *) vlist_get(list,i));

  return ;
}


int
main(int argc, char** argv)
{
  void** l = vlist_new(2);

  printlist(l);

  l[0] = "a";
  l[1] = "b";

  printlist(l);

  l = vlist_resize(l, 4);

  printlist(l);
}

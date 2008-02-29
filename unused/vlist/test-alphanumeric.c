#include <stdio.h>
#include "vlist.h"
#include "../macro.h"

#ifndef NULL
#define NULL ((void *) 0)
#endif

int
main ( int argc, char **argv )
{
  int i;
  void** lst;
  lst = NULL;

  lst = vlist_new(26);
  
  for ( i = 'a'; i <= 'z'; i++ )
    {
      lst[i - 'a'] = UINT2PTR(i);//lst = vlist_append( vlist_cast(lst), UINT2PTR(i) );
    }

  for ( i = 0; lst[i]; i++ )
    {
      printf("%c ", PTR2UINT(lst[i]));
    }

  printf("\n");

  vlist_free(lst);

  return 0;
}

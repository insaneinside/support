#include <stdio.h>

#include "vlist.h"

#define printlist(list) _printlist(list,#list)

void
_printlist(char** list, char* title)
{
  int i;
  size_t sz;
  sz = vlist_size(vlist_cast(list));
  if(sz < 1)
    {
      fprintf(stderr, "vlist_size(%s) < 1; nothing to print.\n", title);
      return;
    }
    
  for ( i = 0; i < sz; i++ )
    printf("%s[%d]= \"%s\"\n", title, i, (char *) list[i]);

  return ;
}

char**
create_list(const char** vl1, const char** vl2, const char* title)
{
  int i;
  char** out = NULL;

  puts("Testing vlist_new()");
  out = (char**) vlist_new(0);
  printf("`%s' initialized.\n", title);

  for ( i = 0; vl1[i] != NULL; i++ )
    {
      puts("Testing vlist_add()");
      out = (char**) vlist_add(vlist_cast(out));
      puts("Testing vlist_set()");
      vlist_set(vlist_cast(out), vl1[i], -1);

      printf("List size: %d item(s)\n", vlist_size(vlist_cast(out)));
      _printlist(out, title);
      printf("\n");
    }


  for ( i = 0; vl2[i] != NULL; i++ )
    {
      printf("Testing vlist_append()\n");
      out = (char**) vlist_append(vlist_cast(out), vl2[i]);

      printf("List size: %d item(s)\n", vlist_size(vlist_cast(out)));
      _printlist(out, title);
      printf("\n");
    }

  printf("\n--------\ncreate_list routine finished. State at end of routine:\n");

  printf("List size: %d item(s)\n",vlist_size(vlist_cast(out)));
  _printlist(out, title);
  printf("\n");
  printf("\n");

  return out;
}


int
main(int argc, char **argv)
{
  char** land = NULL;
  char** sea = NULL;

  const char* land1[] = { "Alligators", "Cats", "Dogs", "Frogs", "Antelopes", NULL };
  const char* land2[] = { "GNU", "YACC", "HURD", "BISON", "Python", NULL };
  const char* sea1[] = { "Fish", "Whale", "Shark", "Anemone", "Giant Squid", NULL };
  const char* sea2[] = { "um", "like", "what", "ever", "dude", NULL };
  land = create_list(land1, land2, "land");
  sea = create_list(sea1, sea2, "sea");

  printf( "Testing vlist_append_list()\n");
  land = (char**) vlist_append_list(vlist_cast(land), vlist_cast(sea));
  printf("Sea appended to land.\n\n");
  printf("vlist_size(land)==%d\n", vlist_size(vlist_cast(land)));
  printf("vlist_size(sea) ==%d\n\n", vlist_size(vlist_cast(sea)));
  printlist(land);
  printlist(sea);

  vlist_free(vlist_cast(land));
  vlist_free(vlist_cast(sea));

  return 0;
}

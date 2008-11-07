#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>

#include <support/mlog.h>
#include <support/dllist.h>

#define NTESTSTRINGS 10
#define WORDLIST "/home/collin/.wordlists/2of12.txt"


static int
_print_string(dllist_t* node, const void* userdata)
{
  char *s;
  s = (char *) node->data;

  printf("node %p: %s\n", (void*) node, s);

  return 1;
}

static int
_free_string(dllist_t* node, const void* userdata)
{
  free(node->data);
  node->data = NULL;
  return 1;
}
static int
alphacmp(void* a, void* b)
{
  char* sa = (char*) a;
  char* sb = (char*) b;
  unsigned int i;
  signed char d;

  for ( i = 0; sa[i] != '\0' && sb[i] != '\0'; i++ )
    {
      d = sa[i] - sb[i];
      if ( d )
	return d;
    }
  return 0;
}


static unsigned int
file_count_lines(FILE *fp)
{
  off_t pos;
  char *buf;
  int c;			/* read count */
  int n = 0;			/* newline count */
  int i;

  /* save current file offset */
  pos = ftello(fp);
  fseeko(fp, 0, SEEK_SET);

  buf = (char *) malloc(BUFSIZ);

  while((c = fread(buf, 1, BUFSIZ, fp)) > 0) {
    for(i=0; i < c; i++)
      if(buf[i] == '\n')
	n++;
  }

  /* clean up */
  free(buf);
  fseeko(fp, pos, SEEK_SET);	/* restore offset */

  return n;
}

static ssize_t
file_get_line(FILE* fp, const unsigned int ln, char** buf, size_t* count)
{
  unsigned int i;
  ssize_t r;

  fseeko(fp, 0, SEEK_SET);

  for ( i = 0; i <= ln; i++ )
    r = getline(buf, count, fp);

  return r;
}



int
main(int argc, char** argv)
{
  FILE* fp;
  char** str;
  unsigned int lines;
  int i, sl;
  size_t n;
  dllist_t* list;
  unsigned int nteststrings = NTESTSTRINGS;
  clock_t st, en;
  mlog_set_level(V_DEBUG);

  if ( argc > 1 )
    {
      errno = 0;
      char* tail = NULL;
      nteststrings = strtoul(argv[1], &tail, 0);

      if ( *tail != '\0' || errno == ERANGE || errno == EINVAL )
	{
	  mlog(V_FATAL, "Bad number \"%s\"", argv[1]);
	  return 1;
	}
    }

  srand(time(NULL));

  /* Grab some words at random for testing */
  mlog(V_INFO, "Retrieving %d words for use as test strings", nteststrings);
  mlog(V_INFO, "from wordlist \"%s\"", WORDLIST);
  str = (char**) malloc(sizeof(char*) * nteststrings);
  fp = fopen(WORDLIST, "r");
  mlog(V_INFO | F_NONEWLINE, "Counting lines... ");
  st = clock();
  lines = file_count_lines(fp);
  en = clock();
  mlog(V_INFO, "%u total lines (%.03fs)", lines, (en - st)/CLOCKS_PER_SEC);

  mlog(V_INFO | F_NONEWLINE, "Selecting random words... ");
  st = clock();
  for ( i = 0; i < nteststrings; i++ )
    {
      str[i] = NULL;
      n = 0;
      sl = file_get_line(fp, (rand() % lines), &(str[i]), &n);

      /* remove the trailing newline */
      if ( str[i][sl - 1] == '\n' )
	  str[i][sl - 1] = '\0';
/*       for(j=0; j < sl; j++) */
/* 	if(str[i][j] == '\n') */
/* 	  str[i][j] = '\0'; */
      
    }
  en = clock();
  fclose(fp);
  mlog(V_INFO, "done (%.03fs)", ( en - st ) / CLOCKS_PER_SEC );


  /* Populate the list */
  list = NULL;

  printf("Populating list... ");
  st = clock();
  for(i=0; i < nteststrings; i++)
    list = dllist_append(list, str[i]);
  en = clock();
  printf("done. (%.03fs)\n", (en - st)/CLOCKS_PER_SEC);

  /* Print out list */
  printf("List contents:\n");
  dllist_foreach(list, &_print_string, NULL);

  printf("\nSorting list using dllist_sort... ");
  st = clock();
  list = dllist_sort(list, &alphacmp);
  en = clock();
  printf("done. (%.03fs)\n", (en - st)/CLOCKS_PER_SEC);

  printf("List contents:\n");
  dllist_foreach(list, &_print_string, NULL);
  dllist_free(list);
  list = NULL;

  /* \n in middle of next string because it's been wrapped to 70 columns. */
  printf("\nOkay, now building a sorted structure from scratch using the same\nstrings... ");

  st = clock();
  for ( i = 0; i < nteststrings; i++ )
    list = dllist_insert_sorted(list, str[i], &alphacmp);
  en = clock();
  printf("done. (%.03fs)\n", (en - st)/CLOCKS_PER_SEC);

  printf("List contents:\n");
  dllist_foreach(list, &_print_string, NULL);

  dllist_foreach(list, &_free_string, NULL);
  dllist_free(list);  

  /* for(i=0; i < nteststrings; i++)
   *   free(str[i]); */
  free(str);


  return 0;
}

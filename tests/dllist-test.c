#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>

#include <support/dllist.h>
#include "timeutil.h"

#define NTESTSTRINGS 10
#define WORDLIST "/home/collin/.wordlists/2of12.txt"


static int
_print_string(dllist_t* node, const void* userdata __attribute__ (( unused )) )
{
  char *s;
  s = (char *) node->data;

  printf("node %p: %s\n", (void*) node, s);

  return 1;
}

static int
_free_string(dllist_t* node, const void* userdata __attribute__ (( unused )) )
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
  signed int d;

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
  size_t c;			/* read count */
  unsigned int n = 0;			/* newline count */
  unsigned int i;

  /* save current file offset */
  pos = ftello(fp);
  fseeko(fp, 0, SEEK_SET);

  buf = (char *) malloc(BUFSIZ);

  /* Read the file in blocks, and count the number of newline
   * characters in each block.
   */
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
  timeutil_init_mark_variables();
  FILE* fp;
  char** str;
  unsigned int lines;
  unsigned int i;
  ssize_t sl;
  size_t n;
  dllist_t* list;
  unsigned int nteststrings = NTESTSTRINGS;

  if ( argc > 1 )
    {
      errno = 0;
      char* tail = NULL;
      nteststrings = strtoul(argv[1], &tail, 0);

      if ( *tail != '\0' || errno == ERANGE || errno == EINVAL )
	{
	  fprintf(stderr, "Bad number \"%s\"\n", argv[1]);
	  return 1;
	}
    }

  srand((unsigned int) time(NULL));

  /* Grab some words at random for testing */
  fprintf(stderr, "Retrieving %d words from \"%s\" for use as test strings\n", nteststrings, WORDLIST);
  str = (char**) malloc(sizeof(char*) * nteststrings);
  fp = fopen(WORDLIST, "r");
  if ( !fp )
    {
      perror(WORDLIST);
      abort();
    }
  timeutil_begin("Counting lines");
  lines = file_count_lines(fp);
  timeutil_endf("%u total lines", lines);

  timeutil_begin("Selecting random words");
  for ( i = 0; i < nteststrings; i++ )
    {
      str[i] = NULL;
      n = 0;
      sl = file_get_line(fp, ((unsigned) rand() % lines), &(str[i]), &n);

      /* remove the trailing newline */
      if ( sl < 0 )
	{
	  assert( sl > 0 || ! feof(fp) ); /* tried to fetch a nonexistent line */
	  perror("error");
	  abort();
	}
      if ( str[i][sl - 1] == '\n' )
	str[i][sl - 1] = '\0';
/*       for(j=0; j < sl; j++) */
/*	if(str[i][j] == '\n') */
/*	  str[i][j] = '\0'; */

    }
  timeutil_end();
  fclose(fp);


  /* Populate the list */
  list = NULL;

  timeutil_begin("Populating list");
  for(i=0; i < nteststrings; i++)
    list = dllist_append(list, str[i]);
  timeutil_end();

  /* Print out list */
  fprintf(stderr, "List contents:\n");
  dllist_foreach(list, &_print_string, NULL);

  timeutil_begin("\nSorting list using dllist_sort");
  list = dllist_sort(list, &alphacmp);
  timeutil_end();

  printf("List contents:\n");
  dllist_foreach(list, &_print_string, NULL);
  dllist_free(list);
  list = NULL;

  /* \n in middle of next string because it's been wrapped to 70 columns. */
  timeutil_begin("\nOkay, now building a sorted structure from scratch using the same\nstrings");
  for ( i = 0; i < nteststrings; i++ )
    list = dllist_insert_sorted(list, str[i], &alphacmp);
  timeutil_end();

  printf("List contents:\n");
  dllist_foreach(list, &_print_string, NULL);

  dllist_foreach(list, &_free_string, NULL);
  dllist_free(list);

  /* for(i=0; i < nteststrings; i++)
   *   free(str[i]); */
  free(str);


  return 0;
}

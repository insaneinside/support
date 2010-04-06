#define _GNU_SOURCE
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <support/strutils.h>

char*
stalins_cat(char** dest, size_t *n, const char* a, const char* b)
{
  size_t need, h;
  char* op;
  char* d = NULL;

  if ( dest )
    d = *dest;

  if ( n )
    h = *n;
  else
    h = 0;

  assert(a != NULL && b != NULL);
  need = strlen(a) + strlen(b) + 1;

  if ( !d || !h )
    {
      d = (char*) malloc(need);
      h = need;
    }
  else if ( h < need )
    {
      d = (char*) realloc(d, need);
      h = need;
    }

  memset(d, 0, h);
  op = d;
  op = stpcpy(op, a);
  op = stpcpy(op, b);

  if ( dest )
    *dest = d;

  if ( n )
    *n = h;

  return d;
}


char*
strchomp(const char* s)
{
  char* o = (char*) s;

  while ( isspace(*o) )
    o++;

  return o;
}
/*   for ( iter = o.begin(); iter != o.end()) */
/*     { */
/*       if(isspace(*iter)) */
/* 	iter = o.erase(iter); */
/*       else */
/* 	break; */
/*     } */

/*   return o; */
/* } */


char*
strchug(const char* s)
{
  assert(s != NULL);
  char* o = NULL;
  ssize_t i;

  for ( i = (ssize_t) strlen(s) - 1; i >= 0 && isspace(s[i]); i-- )
    ;

  if ( i > 0 )
    {
      o = (char*) malloc((unsigned) i + 2);
      if ( o != NULL )
        {
          o[i] = '\0';
          memcpy(o, s, (unsigned) i + 1);
        }
    }

  return o;
}


char*
strip_args(char* s)
{
  char* p = NULL;

  if ( ( p = strchr(s, '(') ) )
    *p = '\0';

  return s;
}

char*
strip_pretty_function(char* s)
{
  return strrchr(strip_args(s), ' ') + 1;
}

char*
astringf(const char* format, ...)
{
  va_list ap;
  char* o = NULL;

  va_start(ap, format);
  vasprintf(&o, format, ap);
  va_end(ap);

  return o;
}


/* std::string */
/* strchug(const std::string s) */
/* { */
/*   std::string::iterator iter; */
/*   std::string o(s); */
  
/*   for ( iter = o.end() - 1; iter != o.begin(); iter-- ) */
/*     { */
/*       if ( isspace(*iter) ) */
/* 	o.erase(iter); */
/*       else */
/* 	break; */
/*     } */

/*   return o; */
/* } */

/* std::string */
/* strstrip(const std::string s) */
/* { */
/*   return strchomp(strchug(s)); */
/* } */

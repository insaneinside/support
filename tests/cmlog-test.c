#define _GNU_SOURCE
#include <mcheck.h>
#include <stdio.h>
#include <stdlib.h>
#include <support/mlog.h>
#include <assert.h>
#include <string.h>
#include "timeutil.h"

#if 0
static int
_print_pspec(const struct parse_spec* ps)
{
#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
  assert(ps->magic == PARSE_SPEC_MAGIC);
#endif

  printf("spec %p (%u): %s",
	 (void*) ps,
	 ps->name_array_length,
	 ps->flags & MLOG_CONTEXT_EXPLICIT_STATE ? "+" : "-"
	 );
  unsigned int i;
  char end = 0;
  for ( i = 0; i < ps->name_array_length; i++ )
    {
      if ( i < ps->name_array_length - 1 )
	end = 0;
      else
	end = 1;

      printf("%s%s%s",
	     ps->name_array[i],
	     end ? "" : CONTEXT_NAME_SEPARATOR,
	     end ? "\n" : "");
    }

  return 1;
}
#endif	/* 0 */

static void
do_test(const char* spec)
{
  char* ok = "ok";
  char* err = "error";

  init_mark_variables();
  fprintf(stderr, "These baseline marks have no code in between:\n");
  mark_label("baseline");
  mark_label("baseline 2");
  mark_label("baseline 3");
  mark_label("baseline 4");

  if ( spec != NULL )
    {
      ok = "Test!";
      begin("Parsing spec string");
      int r = mlog_context_parse_spec(spec);
      end();
      fprintf(stderr, "    -> mlog_context_parse_spec returned %d\n", r);
    }

  begin("Creating contexts");
  mlog_context_t *A, *B, *C, *D, *E, *E2;
  A = mlog_context_create(NULL, "A", "Test context A");
  B = mlog_context_create(A, "B", "Test context B");
  C = mlog_context_create(A, "C", "Test context C");
  D = mlog_context_create(C, "D", "Test context D");
  E = mlog_context_create(C, "E", "Test context E");
  E2 = mlog_context_create(E, "E", "Test context E (number two)");
  end();

  /* Should activate all contexts. */
  if ( !spec )
    mlog_context_enable(A);	/* implicitly enables B, C, D, E */
  else
    begin_nl("Printing test messages for spec-enabled contexts");

  cmlog(A, V_DEBUG, ok);	/* visible */
  cmlog(B, V_DEBUG, ok);	/* visible */
  cmlog(C, V_DEBUG, ok);	/* visible */
  cmlog(D, V_DEBUG, ok);	/* visible */
  cmlog(E, V_DEBUG, ok);	/* visible */
  cmlog(E2, V_DEBUG, ok);	/* visible */
  mark();
  if ( !spec )
    {
      /* should deactivate C, D, E */
      mlog_context_disable(C);	/* implicitly disables D, E */
      cmlog(A, V_DEBUG, ok);	/* visible */
      cmlog(B, V_DEBUG, ok);	/* visible */
      cmlog(C, V_DEBUG, err);	/* invisible */
      cmlog(D, V_DEBUG, err);	/* invisible */
      cmlog(E, V_DEBUG, err);	/* invisible */
      cmlog(E2, V_DEBUG, ok);	/* visible */
      mark();

      /* shouldn't do anything (D has never been explicitly set) */
      mlog_context_reset(D);
      cmlog(A, V_DEBUG, ok);	/* visible */
      cmlog(B, V_DEBUG, ok);	/* visible */
      cmlog(C, V_DEBUG, err);	/* invisible */
      cmlog(D, V_DEBUG, err);	/* invisible */
      cmlog(E, V_DEBUG, err);	/* invisible */
      cmlog(E2, V_DEBUG, ok);	/* visible */
      mark();

      /* should activate D. */
      mlog_context_enable(D);
      cmlog(A, V_DEBUG, ok);	/* visible */
      cmlog(B, V_DEBUG, ok);	/* visible */
      cmlog(C, V_DEBUG, err);	/* invisible */
      cmlog(D, V_DEBUG, ok);	/* visible */
      cmlog(E, V_DEBUG, err);	/* invisible */
      cmlog(E2, V_DEBUG, ok);	/* visible */
      mark();

      /* should activate C, E  */
      mlog_context_reset(C);
      cmlog(A, V_DEBUG, ok);	/* visible */
      cmlog(B, V_DEBUG, ok);	/* visible */
      cmlog(C, V_DEBUG, ok);	/* visible */
      cmlog(D, V_DEBUG, ok);	/* visible */
      cmlog(E, V_DEBUG, ok);	/* visible */
      cmlog(E2, V_DEBUG, ok);	/* visible */
      mark();

      /* should deactivate C, E */
      mlog_context_disable(C);
      cmlog(A, V_DEBUG, ok);	/* visible */
      cmlog(B, V_DEBUG, ok);	/* visible */
      cmlog(C, V_DEBUG, err);	/* invisible */
      cmlog(D, V_DEBUG, ok);	/* visible */
      cmlog(E, V_DEBUG, err);	/* invisible */
      cmlog(E2, V_DEBUG, ok);	/* visible */
      mark();
    }
  begin("Destroying contexts");
  /* Could just destroy A recursively, but this should -- must -- work
   * too.
   */
  mlog_context_destroy(A);
  mlog_context_destroy(B);
  mlog_context_destroy_recursive(C);
  end();
}

int main(int argc, char** argv)
{
  char* s;
  if ( argc > 1 )
    s = argv[1];
  else
    s = NULL;

  if ( s && ( !strcmp(s, "-h") || !strcmp(s, "--help") || !strcmp(s, "-?") ) )
    {
      printf("Usage: %s [PSPEC]\n", basename(argv[0]));
      return 0;
    }

  mtrace();
  mlog_set_level(V_DEBUG);
  do_test(s);
  muntrace();

  return 0;
}

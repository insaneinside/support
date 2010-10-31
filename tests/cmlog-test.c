#define _GNU_SOURCE
#include <mcheck.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <support/spt-context.h>
#include <support/timeutil.h>

#ifdef SPT_CONTEXT_ENABLE_DESCRIPTION
#define CONTEXT_DESCRIPTION(d) , d
#else
#define CONTEXT_DESCRIPTION(d)
#endif
#define CONTEXT_NAME_SEPARATOR "."

void
_print_pspec(const spt_context_parse_spec_t* ps)
{
#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
  assert(SPT_IS_CONTEXT_PARSE_SPEC(ps));
#endif

  printf("spec %p (%zu): %s",
	 (void*) ps,
	 ps->name_array_length,
	 ps->flags & SPT_CONTEXT_EXPLICIT_STATE ? "+" : "-"
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

  return;
}

static int
_fe_print_pspec(dllist_t* node, const void* udata __attribute__((unused)) )
{
  _print_pspec(node->data);
  return 1;
}

static void
do_test(const char* spec)
{
  char* ok = "ok";
  char* err = "error";
  dllist_t* pspec_list = NULL;
  timeutil_init_mark_variables();
  fprintf(stderr, "These baseline marks have no code in between:\n");
  timeutil_mark_label("baseline");
  timeutil_mark_label("baseline 2");
  timeutil_mark_label("baseline 3");
  timeutil_mark_label("baseline 4");

  if ( spec != NULL )
    {
      ok = "Test!";
      timeutil_begin("Parsing spec string");
      pspec_list = spt_context_parse_specs(spec);
      timeutil_end();
      int size = dllist_size(pspec_list);
      fprintf(stderr, "Found %d parse specs.\n", size);

      if ( size > 0 )
	dllist_foreach(pspec_list, &_fe_print_pspec, NULL);
    }

  timeutil_begin("Creating contexts");
  spt_context_t *all, *A, *B, *C, *D, *E, *E2;
  all = spt_context_create(NULL, "all" CONTEXT_DESCRIPTION("Test hidden context"));
  all->flags |= SPT_CONTEXT_HIDE_NAME;
  A = spt_context_create(all, "A"   CONTEXT_DESCRIPTION("Test context A"));
  B = spt_context_create(A, "B"   CONTEXT_DESCRIPTION("Test context B"));
  C = spt_context_create(A, "C"   CONTEXT_DESCRIPTION("Test context C"));
  D = spt_context_create(C, "D"   CONTEXT_DESCRIPTION("Test context D"));
  E = spt_context_create(C, "E"   CONTEXT_DESCRIPTION("Test context E"));
  E2 = spt_context_create(E, "E"   CONTEXT_DESCRIPTION("Test context E (number two)"));
  timeutil_end();

  assert(spt_context_get_num_ancestors(all) == 6);
  assert(spt_context_get_num_ancestors(A) == 5);
  assert(spt_context_get_num_ancestors(B) == 0);
  assert(spt_context_get_num_ancestors(C) == 3);
  assert(spt_context_get_num_ancestors(D) == 0);
  assert(spt_context_get_num_ancestors(E) == 1);
  assert(spt_context_get_num_ancestors(E2) == 0);

  if ( spec != NULL )
    {
      timeutil_begin("Applying parse specs");
      spt_context_apply_parse_specs(all, pspec_list);
      timeutil_end();
      spt_context_parse_spec_destroy_list(pspec_list);
    }

  /* Should activate all contexts. */
  if ( spec == NULL )
    spt_context_enable(A);	/* implicitly enables B, C, D, E */
  else
    timeutil_begin_nl("Printing test messages for spec-enabled contexts...");

  cmlog(A, V_DEBUG, ok);	/* visible */
  cmlog(B, V_DEBUG, ok);	/* visible */
  cmlog(C, V_DEBUG, ok);	/* visible */
  cmlog(D, V_DEBUG, ok);	/* visible */
  cmlog(E, V_DEBUG, ok);	/* visible */
  cmlog(E2, V_DEBUG, ok);	/* visible */
  timeutil_end_label("-- ...done");
  if ( !spec )
    {
      /* should deactivate C, D, E */
      spt_context_disable(C);	/* implicitly disables D, E */
      cmlog(A, V_DEBUG, ok);	/* visible */
      cmlog(B, V_DEBUG, ok);	/* visible */
      cmlog(C, V_DEBUG, err);	/* invisible */
      cmlog(D, V_DEBUG, err);	/* invisible */
      cmlog(E, V_DEBUG, err);	/* invisible */
      cmlog(E2, V_DEBUG, err);	/* invisible */
      timeutil_mark();

      /* shouldn't do anything (D has never been explicitly set) */
      spt_context_reset(D);
      cmlog(A, V_DEBUG, ok);	/* visible */
      cmlog(B, V_DEBUG, ok);	/* visible */
      cmlog(C, V_DEBUG, err);	/* invisible */
      cmlog(D, V_DEBUG, err);	/* invisible */
      cmlog(E, V_DEBUG, err);	/* invisible */
      cmlog(E2, V_DEBUG, err);	/* invisible */
      timeutil_mark();

      /* should activate D. */
      spt_context_enable(D);
      cmlog(A, V_DEBUG, ok);	/* visible */
      cmlog(B, V_DEBUG, ok);	/* visible */
      cmlog(C, V_DEBUG, err);	/* invisible */
      cmlog(D, V_DEBUG, ok);	/* visible */
      cmlog(E, V_DEBUG, err);	/* invisible */
      cmlog(E2, V_DEBUG, err);	/* invisible */
      timeutil_mark();

      /* should activate C, E  */
      spt_context_reset(C);
      cmlog(A, V_DEBUG, ok);	/* visible */
      cmlog(B, V_DEBUG, ok);	/* visible */
      cmlog(C, V_DEBUG, ok);	/* visible */
      cmlog(D, V_DEBUG, ok);	/* visible */
      cmlog(E, V_DEBUG, ok);	/* visible */
      cmlog(E2, V_DEBUG, ok);	/* visible */
      timeutil_mark();

      /* should deactivate C, E */
      spt_context_disable(C);
      cmlog(A, V_DEBUG, ok);	/* visible */
      cmlog(B, V_DEBUG, ok);	/* visible */
      cmlog(C, V_DEBUG, err);	/* invisible */
      cmlog(D, V_DEBUG, ok);	/* visible */
      cmlog(E, V_DEBUG, err);	/* invisible */
      cmlog(E2, V_DEBUG, ok);	/* visible */
      timeutil_mark();
    }
  timeutil_begin("Destroying contexts");
  spt_context_destroy_recursive(all);
  timeutil_end();
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

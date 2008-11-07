#include <mcheck.h>
#include <stdio.h>
#include <support/mlog.h>

static void
do_test()
{
  mlog_context_t *A, *B, *C, *D, *E;
  A = mlog_context_create(NULL, "A", "Test context A");
  B = mlog_context_create(A, "B", "Test context B");
  C = mlog_context_create(A, "C", "Test context C");
  D = mlog_context_create(C, "D", "Test context D");
  E = mlog_context_create(C, "E", "Test context E");

  mlog(V_DEBUG, "-- MARK --");

  /* Should activate all contexts. */
  mlog_context_enable(A);	/* implicitly enables B, C, D, E */
  cmlog(A, V_DEBUG, "ok");	/* visible */
  cmlog(B, V_DEBUG, "ok");	/* visible */
  cmlog(C, V_DEBUG, "ok");	/* visible */
  cmlog(D, V_DEBUG, "ok");	/* visible */
  cmlog(E, V_DEBUG, "ok");	/* visible */

  mlog(V_DEBUG, "-- MARK --");

  /* should deactivate C, D, E */
  mlog_context_disable(C);	/* implicitly disables D, E */
  cmlog(A, V_DEBUG, "ok");	/* visible */
  cmlog(B, V_DEBUG, "ok");	/* visible */
  cmlog(C, V_DEBUG, "error");	/* invisible */
  cmlog(D, V_DEBUG, "error");	/* invisible */
  cmlog(E, V_DEBUG, "error");	/* invisible */

  mlog(V_DEBUG, "-- MARK --");

  /* shouldn't do anything ( D has never been explicitly set */
  mlog_context_reset(D);
  cmlog(A, V_DEBUG, "ok");	/* visible */
  cmlog(B, V_DEBUG, "ok");	/* visible */
  cmlog(C, V_DEBUG, "error");	/* invisible */
  cmlog(D, V_DEBUG, "error");	/* invisible */
  cmlog(E, V_DEBUG, "error");	/* invisible */

  mlog(V_DEBUG, "-- MARK --");

  /* should activate D. */
  mlog_context_enable(D);
  cmlog(A, V_DEBUG, "ok");	/* visible */
  cmlog(B, V_DEBUG, "ok");	/* visible */
  cmlog(C, V_DEBUG, "error");	/* invisible */
  cmlog(D, V_DEBUG, "ok");	/* visible */
  cmlog(E, V_DEBUG, "error");	/* invisible */

  mlog(V_DEBUG, "-- MARK --");

  /* should activate C, E  */
  mlog_context_reset(C);
  cmlog(A, V_DEBUG, "ok");	/* visible */
  cmlog(B, V_DEBUG, "ok");	/* visible */
  cmlog(C, V_DEBUG, "ok");	/* visible */
  cmlog(D, V_DEBUG, "ok");	/* visible */
  cmlog(E, V_DEBUG, "ok");	/* visible */

  mlog(V_DEBUG, "-- MARK --");

  /* should deactivate C, E */
  mlog_context_disable(C);
  cmlog(A, V_DEBUG, "ok");	/* visible */
  cmlog(B, V_DEBUG, "ok");	/* visible */
  cmlog(C, V_DEBUG, "error");	/* invisible */
  cmlog(D, V_DEBUG, "ok");	/* visible */
  cmlog(E, V_DEBUG, "error");	/* invisible */

  mlog(V_DEBUG, "-- MARK --");

  mlog_context_destroy(A);
  mlog_context_destroy(B);
  mlog_context_destroy_recursive(C);
}

int main(int argc, char** argv)
{
  mtrace();
  mlog_set_level(V_DEBUG);
  do_test();
  muntrace();

  return 0;
}

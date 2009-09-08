#ifndef SPT_TEST_TIMEUTIL_H
#define SPT_TEST_TIMEUTIL_H

#include <sys/timex.h>
#include <errno.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define TIMEUTIL_ERRMSG_BUF_SIZE 256
#define time_value timeutil_te.time.tv_usec
#define time_elapsed_value timeutil_te.time.tv_usec - timeutil_st
#define time_value_type long int
#define time_value_format "%ld microsecond%s"
#define timeutil_is_singular(v) (v == 1)

#define macro_single_statement(x) do { x; } while ( 0 )

#define init_mark_variables()					\
  struct ntptimeval timeutil_te;				\
  long int timeutil_st = -1

#define update_time_value()						\
  macro_single_statement(						\
  int local_errno = 0;							\
  if ( (local_errno = ntp_gettime(&timeutil_te)) && local_errno > 0 ) { \
    char* errbuf = (char*) alloca(TIMEUTIL_ERRMSG_BUF_SIZE);		\
    strerror_r(local_errno, errbuf, TIMEUTIL_ERRMSG_BUF_SIZE);		\
    fprintf(stderr, "ntp_gettime: %s\n", errbuf);			\
    abort();								\
  } )

#define update_last_time_value() timeutil_st = time_value

#define mark_label(s)							\
  macro_single_statement(						\
  update_time_value();							\
  if ( timeutil_st != -1 )						\
    fprintf(stderr, "-- MARK%s%s ("time_value_format" elapsed)\n",	\
	    s ? ": " : "",						\
	    s ? s : "",							\
	    time_elapsed_value, timeutil_is_singular(time_elapsed_value) ? "" : "s");	\
  else fprintf(stderr, "-- MARK%s%s\n",					\
	       s ? ": " : "",						\
	       s ? s : "");						\
  update_time_value();							\
  update_last_time_value(); )

#define mark() mark_label(NULL)

#define begin(label)							\
  macro_single_statement(						\
  fprintf(stderr, "-- %s... ", label);					\
  update_time_value(); update_last_time_value(); )

#define beginf(fmt, ...)			\
  macro_single_statement(			\
  fprintf(stderr, "-- "fmt"... ", __VA_ARGS__);	\
  update_time_value(); update_last_time_value(); )


#define begin_nl(label)							\
  macro_single_statement(						\
  fprintf(stderr, "-- %s\n", label);					\
  update_time_value(); update_last_time_value(); )


#define end_label(s)							\
  macro_single_statement(						\
  update_time_value();							\
  fprintf(stderr, "%s: "time_value_format" elapsed\n",			\
	  s ? s : "done",						\
	  time_elapsed_value,						\
	  timeutil_is_singular(time_elapsed_value) ? "" : "s"); )

#define endf(fmt, ...)							\
  macro_single_statement(						\
  update_time_value();							\
  fprintf(stderr, fmt": "time_value_format" elapsed\n",			\
	  __VA_ARGS__,							\
	  time_elapsed_value,						\
	  timeutil_is_singular(time_elapsed_value) ? "" : "s"); )

#define end() end_label(NULL)

#ifdef __cplusplus
}
#endif

#endif	/* SPT_TEST_TIMEUTIL_H */

#ifndef SPT_TEST_TIMEUTIL_H
#define SPT_TEST_TIMEUTIL_H

#include <sys/timex.h>
#include <errno.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define time_value te.time.tv_usec
#define time_elapsed_value te.time.tv_usec - st
#define time_value_type long int
#define time_value_format "%ld microsecond%s"
#define is_sg(v) (v == 1)

#define init_mark_variables()			\
  struct ntptimeval te;				\
  long int st = -1

#define update_time_value()						\
  errno = 0; if ( ntp_gettime(&te) && errno > 0 ) { perror("ntp_gettime"); abort(); } \
  else

#define update_last_time_value() st = time_value

#define mark_label(s)							\
  errno = 0; if ( ntp_gettime(&te) && errno > 0 ) { perror("ntp_gettime"); abort(); } \
  if ( st != -1 )							\
    fprintf(stderr, "-- MARK%s%s ("time_value_format" elapsed)\n",	\
	    s ? ": " : "",						\
	    s ? s : "",							\
	    time_elapsed_value, is_sg(time_elapsed_value) ? "" : "s");	\
  else fprintf(stderr, "-- MARK%s%s\n",					\
	       s ? ": " : "",						\
	       s ? s : "");						\
  update_time_value();							\
  update_last_time_value()

#define mark() mark_label(NULL)

#define begin(label)							\
  fprintf(stderr, "-- %s... ", label);					\
  update_time_value(); update_last_time_value()

#define beginf(fmt, ...)			\
  fprintf(stderr, "-- "fmt"... ", __VA_ARGS__);	\
  update_time_value(); update_last_time_value()


#define begin_nl(label)							\
  fprintf(stderr, "-- %s\n", label);					\
  update_time_value(); update_last_time_value()


#define end_label(s)							\
  update_time_value();							\
  fprintf(stderr, "%s: "time_value_format" elapsed\n",			\
	  s ? s : "done",						\
	  time_elapsed_value,						\
	  is_sg(time_elapsed_value) ? "" : "s")

#define endf(fmt, ...)				\
  update_time_value();				\
  fprintf(stderr, fmt": "time_value_format" elapsed\n",			\
	  __VA_ARGS__,							\
	  time_elapsed_value,						\
	  is_sg(time_elapsed_value) ? "" : "s")

#define end() end_label(NULL)

#ifdef __cplusplus
}
#endif

#endif	/* SPT_TEST_TIMEUTIL_H */

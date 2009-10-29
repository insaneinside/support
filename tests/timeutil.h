#ifndef SPT_TEST_TIMEUTIL_H
#define SPT_TEST_TIMEUTIL_H

#include <sys/timex.h>
#include <sys/time.h>
#include <errno.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>


#ifdef __cplusplus
extern "C"
{
#endif

#define timeutil_time_value_type long long int

/* Let's write it once and not lose a zero somewhere */
#define TIMEUTIL_ONE_MILLION 1000000

/** Elapsed time, in microseconds. */
__inline__ timeutil_time_value_type
timeutil_elapsed(struct timeval* t1, struct timeval* t2)
{
  return TIMEUTIL_ONE_MILLION * ( t2->tv_sec - t1->tv_sec )
    + ( t2->tv_usec - t1->tv_usec );
}

#define TIMEUTIL_ERRMSG_BUF_SIZE 256
#define timeutil_time_value timeutil_te.time.tv_usec
#define timeutil_time_elapsed_value                                     \
  timeutil_elapsed(&(timeutil_prev_te->time), &(timeutil_next_te->time))
#define timeutil_time_value_format "%lld microsecond%s"
#define timeutil_is_singular(v) (v == 1)

#define macro_single_statement(x) do { x; } while ( 0 )

#define timeutil_init_mark_variables()                          \
  struct ntptimeval timeutil_te_a;				\
  struct ntptimeval timeutil_te_b;                              \
  memset(&timeutil_te_a, 0, sizeof(struct ntptimeval));         \
  memset(&timeutil_te_b, 0, sizeof(struct ntptimeval));         \
  struct ntptimeval* timeutil_next_te = &timeutil_te_a;         \
  struct ntptimeval* timeutil_prev_te = &timeutil_te_b

#define timeutil_update_time_value()                                    \
  macro_single_statement(                                               \
  int local_errno = 0;                                                  \
  errno = 0;                                                            \
  if ( (local_errno = ntp_gettime(timeutil_next_te)) > 0                \
       && local_errno == errno )                                        \
    {                                                                   \
  /* Allocate automatically-freed buffer for error string */            \
  char* timeutil_errbuf = (char*) alloca(TIMEUTIL_ERRMSG_BUF_SIZE);     \
  /* We zero the buffer first because strerror/strerror_r doesn't       \
     actually ever write anything to it for error TIME_ERROR in         \
     GNU libc 2.8.90 (contrary to what the library documentation        \
     guarantees). */                                                    \
  memset(timeutil_errbuf, 0, TIMEUTIL_ERRMSG_BUF_SIZE);                 \
									\
  if ( timeutil_errbuf && strerror_r(local_errno,                       \
				     timeutil_errbuf,                   \
				     TIMEUTIL_ERRMSG_BUF_SIZE) != NULL ) { \
    fprintf(stderr, "ERROR: ntp_gettime: %d (%s)\n",                    \
            local_errno, timeutil_errbuf);                              \
    abort();                                                            \
  }                                                                     \
    })

#define timeutil_update_last_time_value()                               \
  macro_single_statement(                                               \
  /* Save pointer to most recently-updated ntptimeval */                \
  struct ntptimeval* timeutil_tmp = timeutil_next_te;                   \
  /* Swap our storage pointers */                                       \
  timeutil_next_te = timeutil_prev_te;                                  \
  timeutil_prev_te = timeutil_tmp                                       \
									)

/** Produce a time "mark", followed by time-elapsed since last
 * timeutil output (if mark was previously used).
 *
 * @param s Label string, or NULL for no label.
 */
#define timeutil_mark_label(s)                                          \
  macro_single_statement(						\
  timeutil_update_time_value();						\
  if ( timeutil_prev_te->time.tv_sec != 0 )                             \
    fprintf(stderr, "-- MARK%s%s ("timeutil_time_value_format" elapsed)\n", \
	    s ? ": " : "",						\
	    s ? s : "",							\
	    timeutil_time_elapsed_value,                                \
            timeutil_is_singular(timeutil_time_elapsed_value) ? "" : "s"); \
  else fprintf(stderr, "-- MARK%s%s\n",					\
	       s ? ": " : "",						\
	       s ? s : "");						\
  timeutil_update_time_value();						\
  timeutil_update_last_time_value(); )

#define timeutil_mark() timeutil_mark_label(NULL)

#define timeutil_begin(label)						\
  macro_single_statement(						\
  fprintf(stderr, "-- %s... ", label);					\
  timeutil_update_time_value(); timeutil_update_last_time_value(); )

#define timeutil_beginf(fmt, ...)			\
  macro_single_statement(                               \
  fprintf(stderr, "-- "fmt"... ", __VA_ARGS__);		\
  timeutil_update_time_value(); timeutil_update_last_time_value(); )


#define timeutil_begin_nl(label)                                        \
  macro_single_statement(						\
  fprintf(stderr, "-- %s\n", label);					\
  timeutil_update_time_value(); timeutil_update_last_time_value(); )


#define timeutil_end_label(s)                                           \
  macro_single_statement(						\
  timeutil_update_time_value();						\
  fprintf(stderr, "%s: "timeutil_time_value_format" elapsed\n",         \
	  s ? s : "done",						\
	  timeutil_time_elapsed_value,                                  \
	  timeutil_is_singular(timeutil_time_elapsed_value) ? "" : "s"); )

#define timeutil_endf(fmt, ...)                                         \
  macro_single_statement(						\
  timeutil_update_time_value();						\
  fprintf(stderr, fmt": "timeutil_time_value_format" elapsed\n",        \
	  __VA_ARGS__,							\
	  timeutil_time_elapsed_value,                                  \
	  timeutil_is_singular(timeutil_time_elapsed_value) ? "" : "s"); )

#define timeutil_end() timeutil_end_label(NULL)

#ifdef __cplusplus
}
#endif

#endif	/* SPT_TEST_TIMEUTIL_H */

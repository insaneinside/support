#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include <support/mlog.h>

#define LIMLEVEL(l) (l >= MLOG_MIN_LOGLEVEL ? ( l <= MLOG_MAX_LOGLEVEL ? l : MLOG_MAX_LOGLEVEL ) : MLOG_MIN_LOGLEVEL)
#define LEVEL(spec)	(spec & MLOG_LOGLEVEL_MASK )
#define FLAGS(spec)	(spec & (~MLOG_LOGLEVEL_MASK))


/* Local variables follow.
 */

mlog_loglevel_t
mloglevel = MLOG_DEFAULT_LOGLEVEL;

int
mlog(const unsigned long spec, const char* fmt, ...)
{
  va_list ap;
  mlog_loglevel_t lvl = LEVEL(spec);

  if ( mloglevel < lvl )
    return 0;

  va_start(ap, fmt);
  return mlogv(spec, fmt, ap);
}

int
mlogv(const unsigned long spec, const char* fmt, va_list ap)
{
  mlog_loglevel_t lvl = LEVEL(spec);
  mlog_flags_t flags = FLAGS(spec);

  static char lhnl = 1;
  static mlog_loglevel_t last_loglevel = -1;

  static char* prefix = "FEWIDT";
  const char* pns = ": ";
  const char* mt = "";
  char* t;


  if ( mloglevel < MLOG_MIN_LOGLEVEL || mloglevel > MLOG_MAX_LOGLEVEL )
    {
      fprintf(stderr,
	      "%s: global mloglevel is out of bounds "
	      "(level = %d; [ min, max ] = [ %d, %d ])\n",
	      __func__, mloglevel,
	      MLOG_MIN_LOGLEVEL, MLOG_MAX_LOGLEVEL);
      abort();
    }

  if ( lhnl || last_loglevel != lvl )
    {
      if ( lvl != last_loglevel )
	{
	  if ( !lhnl )
	    fprintf(stderr,"\n");

	  fprintf(stderr, "[%c]", prefix[lvl]);
	}
      else
	fprintf(stderr, "   ");
      fprintf(stderr, " %s%s",
	      flags & F_PROGNAME ? va_arg(ap, char*) : mt,
	      flags & F_PROGNAME ? pns : mt);
    }
  else
    {
      if ( flags & F_PROGNAME )
	t = va_arg(ap, char*);	/* ignoring that argument */
    }

  if ( flags & F_MODNAME )
    {
      fprintf(stderr, "%s: ", va_arg(ap, char*));
    }
/* #ifdef DEBUG */
/*   fprintf(stderr, "s%:%d: in function %s: ", fn, line, func); */
/* #endif */

  vfprintf(stderr, fmt, ap);

  if ( flags & F_ERRNO )
    fprintf(stderr, ": %s", strerror(errno));

  if ( ! (flags & F_NONEWLINE) )
    fputc('\n', stderr);
  lhnl = ! (flags & F_NONEWLINE);

  va_end(ap);

  last_loglevel = lvl;
  return mloglevel;
}



mlog_loglevel_t
mlog_get_level()
{
  return mloglevel;
}

mlog_loglevel_t
mlog_set_level(const mlog_loglevel_t l)
{
  mloglevel = LIMLEVEL(l);
  return mloglevel;
}

#ifdef __cplusplus
}
#endif

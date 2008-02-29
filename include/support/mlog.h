/*!
  \file mlog.h
  \defgroup mlog mlog logging utility
 */
/*@{*/
#ifndef SUPPORT_MLOG_H
#define SUPPORT_MLOG_H

#ifdef __cplusplus
extern "C"
{
#endif

  /** \name mlog loglevels */
  /*@{*/
typedef enum
  {
    V_FATAL = 0,		/* 0000b */
    V_ERR,			/* 0001b */
    V_WARN,			/* 0010b */
    V_INFO,			/* 0011b */
    V_DEBUG,			/* 0100b */
    V_TELLMEYOURSECRETS		/* 0101b */
  } mlog_loglevel_t;
  /*@}*/

#define MLOG_MIN_LOGLEVEL V_FATAL
#define MLOG_MAX_LOGLEVEL V_TELLMEYOURSECRETS
#define MLOG_DEFAULT_LOGLEVEL V_WARN

#define MLOG_MAX_LOGLEVEL_HIGHEST_BIT ( 1 << 2 )
#define MLOG_LOGLEVEL_MASK 0xFF >> ( 8 - MLOG_MAX_LOGLEVEL_HIGHEST_BIT )


  /** \name mlog flags */
  /*@{*/
typedef enum
  {
    F_NONE	= 0,
    F_PROGNAME	= MLOG_MAX_LOGLEVEL_HIGHEST_BIT << 1,
    F_MODNAME	= MLOG_MAX_LOGLEVEL_HIGHEST_BIT << 2,
    F_NONEWLINE	= MLOG_MAX_LOGLEVEL_HIGHEST_BIT << 3,
    F_ERRNO	= MLOG_MAX_LOGLEVEL_HIGHEST_BIT << 4
  } mlog_flags_t;
  /*@}*/

extern int
mlog(const unsigned long spec, const char* fmt, ...);

mlog_loglevel_t
mlog_get_level();

mlog_loglevel_t
mlog_set_level(const mlog_loglevel_t);

typedef struct __mlog_context mlog_context_t;

mlog_context_t*
mlog_context_create(const char*, const char*);


#ifndef __cplusplus

#define mlog_incr_level() mlog_set_level(mlog_get_level()+1)
#define mlog_decr_level() mlog_set_level(mlog_get_level()-1)

#else
}

#define mlog_incr_level() mlog_set_level(static_cast<mlog_loglevel_t>(static_cast<int>(mlog_get_level())+1))
#define mlog_decr_level() mlog_set_level(static_cast<mlog_loglevel_t>(static_cast<int>(mlog_get_level())-1))

#endif	/* __cplusplus */

/*@}*/

#endif /* SUPPORT_MLOG_H */

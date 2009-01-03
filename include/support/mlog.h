#ifndef SUPPORT_MLOG_H
#define SUPPORT_MLOG_H	1

#include <stdarg.h>
#include <support/support-config.h>
#ifdef SPT_ENABLE_MLOG_CONTEXT
#include <support/mlog-context.h>
#endif

/** @defgroup mlog MLog: Lightweight Logging Utility
 *
 * The MLog function family provides an easy-to-use method for
 * configurable logging.  The @ref context utilities provide support
 * for component-specific log output.
 *
 *@{
 */

#ifdef __cplusplus
extern "C"
{
#endif

  /** Verbosity levels */
  typedef enum
    {
      /** The message indicates an unrecoverable fatal error.
       */
      V_FATAL = 0,		/* 0000b */
      /** The message indicates an error. */
      V_ERR,			/* 0001b */
      /** The message indicates a problem that the user should be
       *  warned about, but that does not prevent processing from
       *  continuing.
       */
      V_WARN,			/* 0010b */
      /** The message contains information that the user may be
       *  interested in.
       */
      V_INFO,			/* 0011b */
      /** The message contains debugging information.
       */
      V_DEBUG,			/* 0100b */
      /** The message contains more information than we really wanted
       * to know.
       */
      V_TELLMEYOURSECRETS	/* 0101b */
    } mlog_loglevel_t;

  /** @internal */
#define MLOG_MAX_LOGLEVEL_HIGHEST_BIT ( 1 << 2 )
  /** @internal */
#define MLOG_MIN_LOGLEVEL V_FATAL
  /** @internal */
#define MLOG_MAX_LOGLEVEL V_TELLMEYOURSECRETS
  /** @internal */
#define MLOG_DEFAULT_LOGLEVEL V_WARN
  /** @internal */
#define MLOG_LOGLEVEL_MASK 0xFF >> ( 8 - MLOG_MAX_LOGLEVEL_HIGHEST_BIT )


  /** Flags that modify the behaviour of mlog.
   */
  typedef enum
    {
      /** Print the program's name before the message.  The name
       * is supplied after the format argument.
       */
      F_PROGNAME	= MLOG_MAX_LOGLEVEL_HIGHEST_BIT << 1,

      /** Print a module or component name before the message.  The
       * name is supplied after the format argument and any program
       * name argument.
       *
       * @deprecated Use @ref context instead.
       */
      F_MODNAME	= MLOG_MAX_LOGLEVEL_HIGHEST_BIT << 2,
      /** Inhibit the newline at the end of the log message.
       */
      F_NONEWLINE	= MLOG_MAX_LOGLEVEL_HIGHEST_BIT << 3,
      /** Follow the message with a colon, a space, and the error
       *  message associated with the current value of @p errno.
       */
      F_ERRNO	= MLOG_MAX_LOGLEVEL_HIGHEST_BIT << 4
    } mlog_flags_t;


  /** Basic log interface.
   *
   * @param spec Bitwise inclusive OR of logging level and any desired
   * behaviour flags.
   *
   * @param fmt <code>printf</code>-style format string.
   *
   * @return The current logging level, or @c 0 if the logging level
   * is lower than the level in @p spec.
   *
   * @sa mlog_loglevel_t mlog_flags_t
   */
  int
  mlog(const unsigned long spec, const char* fmt, ...);

  /** Variadic back-end for MLog.
   */
  int mlogv(const unsigned long spec, const char* fmt, va_list ap);

  /** Get the current global logging level.
   */
  mlog_loglevel_t
  mlog_get_level();

  /** Set the global logging level.
   */
  mlog_loglevel_t
  mlog_set_level(const mlog_loglevel_t);

#ifndef __cplusplus

#define mlog_incr_level() mlog_set_level(mlog_get_level()+1)
#define mlog_decr_level() mlog_set_level(mlog_get_level()-1)

#else
}

#define mlog_incr_level() mlog_set_level(static_cast<mlog_loglevel_t>(static_cast<int>(mlog_get_level())+1))
#define mlog_decr_level() mlog_set_level(static_cast<mlog_loglevel_t>(static_cast<int>(mlog_get_level())-1))

#endif	/* __cplusplus */

/**@}*/

#endif /* SUPPORT_MLOG_H */

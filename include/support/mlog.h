#ifndef SUPPORT_MLOG_H
#define SUPPORT_MLOG_H	1

#include <support/support-config.h>
#include <stdarg.h>

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
      V_FATAL = 1,		/* 0000b */
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

  /** @defgroup context Contexts
   *
   * Component-centric logging.  Log contexts allow for 
   *
   *   - Runtime activation or deactivation of a given context
   *   - Hierarchical context manipulation
   *     - Implicit activation inheritance
   *     - Explicit overriding of inherited activation states
   *
   * @{
   */

  /** Opaque object interface to the logging context system.  Use the
   * following functions to manipulate a log context.
   */
  typedef struct __mlog_context mlog_context_t;

  /** Create a new logging context.
   *
   * @param parent @c NULL, or a context to assign as the parent context for the
   * context being created.  Unless the created context is explicitly
   * activated or deactivated with mlog_context_enable or
   * mlog_context_disable, it will inherit the parent's activation
   * state.
   *
   * @param name Symbolic name for the context.
   *
   * @param description Description of what the logging context is
   * used for.  Currently unused (may be @c NULL).
   */
  mlog_context_t*
  mlog_context_create(mlog_context_t* parent,
		      const char* name, const char* description);

  /** Destroy a logging context.
   *
   * The parent attribute of subcontexts (children) of the context
   * being destroyed will be set to NULL.
   *
   * @param context The context to destroy
   */
  void
  mlog_context_destroy(mlog_context_t* context);

  /** Recursively destroy a logging context and all subcontexts.
   *
   * @param context The context to destroy
   *
   * @sa mlog_context_destroy
   */
  void
  mlog_context_destroy_recursive(mlog_context_t* context);


  /** Explicitly enable a context.  Any child contexts that have not
   * been explicitly enabled or disabled will also be enabled.
   *
   * @param context The context to enable.
   */
  void
  mlog_context_enable(mlog_context_t* context);


  /** Explicitly disable a context.  Any child contexts that have not
   * been explicitly enabled or disabled will also be disabled.
   *
   * @param context The context to disable.
   */
  void
  mlog_context_disable(mlog_context_t* context);


  /** Reset a context to implicit (inherited) activation.  If the
   * context has a parent context, the parent context's activation
   * state will be inherited.  If the context has any child contexts,
   * the inherited state will be recursively propagated to those child
   * contexts which have not been explicitly activated or deactivated.
   *
   * @param context The context to reset.
   */
  void
  mlog_context_reset(mlog_context_t* context);


  /** Context-enabled version of mlog.
   *
   * @param context The context in which the message is to be logged.
   *
   * @copydetails mlog
   */
  int
  cmlog(const mlog_context_t* context, const unsigned long spec, const char* fmt, ...);

  /**@}*/

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

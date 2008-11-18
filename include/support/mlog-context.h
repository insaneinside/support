#ifndef MLOG_CONTEXT_H
#define MLOG_CONTEXT_H	1

#ifdef __cplusplus
extern "C"
{
#endif
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

#ifdef __cplusplus
}
#endif

#endif	/* MLOG_CONTEXT_H */

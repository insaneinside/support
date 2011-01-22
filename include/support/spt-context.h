/* -*- Mode: C; fill-column: 70 -*- */
/** @file support/spt-context.h
 *
 * Context-based logging facilities.
 */
#ifndef SPT_CONTEXT_H
#define SPT_CONTEXT_H	1

#include <unistd.h>		/* for ssize_t */
#include <stdint.h>

#include <support/support-config.h>
#include <support/mlog.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup context Log Contexts
 *
 * Log contexts provide a flexible logging framework that emphasizes
 * control for <em>both</em> the programmer <em>and</em> the end-user.
 * Log contexts allow for:
 *
 *   - Runtime activation or deactivation of a given context
 *   - Hierarchical context manipulation
 *     - Implicit activation inheritance
 *     - Explicit overriding of inherited activation states
 * @{
 */


  /** Opaque object interface to the logging context system.  Use the
   * following functions to manipulate a log context.
   */
  typedef struct __spt_context spt_context_t;

  typedef struct __spt_context_callbacks spt_context_callbacks_t;

  typedef mlog_loglevel_t spt_loglevel_t;
  /**@}*/
/**@}*/

#ifdef __cplusplus
}
#endif

#include <support/spt-context-handlers.h>
#include <support/private/spt-context.h>
#include <support/spt-context-spec.h>
/** @addtogroup context
 * @{
 */
#ifdef __cplusplus
extern "C"
{
#endif

  /* ******************************** */

  /**@name Creation/Destruction
   * @ingroup context
   * Resource management.
   *
   *@{
   */


  /** Allocate and initialize a new logging context.
   *
   * @param parent @c NULL, or a context to assign as the parent for the context
   * being created.  Unless the created context is explicitly activated or
   * deactivated with spt_context_enable or spt_context_disable, it will inherit
   * the parent's activation state.
   *
   * @param name Symbolic name for the context.
   *
   *
   * @if SPT_CONTEXT_ENABLE_DESCRIPTION
   *
   * @param description Description of what the logging context is used for.
   * Currently unused (may be @c NULL).
   *
   * @endif
   *
   * @return A pointer to the new context, or @c NULL if an error was
   * encountered.
   */
#ifdef SPT_CONTEXT_ENABLE_DESCRIPTION
  spt_context_t*
  spt_context_create(spt_context_t* parent,
		     const char* name,
		     const char* description);
#else
  spt_context_t*
  spt_context_create(spt_context_t* parent,
		     const char* name);
#endif

  /** Destroy a logging context.
   *
   * The parent attribute of subcontexts (children) of the context
   * being destroyed will be set to NULL.
   *
   * @param context The context to destroy
   */
  void
  spt_context_destroy(spt_context_t* context);

  /** Recursively destroy a logging context and all subcontexts.
   *
   * @param context The context to destroy
   *
   * @sa spt_context_destroy
   */
  void
  spt_context_destroy_recursive(spt_context_t* context);
  /**@}*/

  /* ******************************** */

  /** @name Context hierarchy manipulation and interrogation
   *  @ingroup context
   *
   * Contexts can be dynamically reparented.
   *
   *@{
   */

  /** Get the number of direct ancestors of a context.
   *
   * @param context Context for which to count children.
   *
   * @return Number of child contexts in the child-list of @p context.
   *
   * @see spt_context_get_num_ancestors
   */
  size_t
  spt_context_get_num_children(spt_context_t* context);


  /** Get the number of ancestors, both direct and indirect, of a
   * context.
   *
   * @param context Context for which to count the number of
   *   ancestors.
   *
   * @return Number of sub-contexts under @p context.
   *
   * @see spt_context_get_num_children
   */
  size_t
  spt_context_get_num_ancestors(spt_context_t* context);


  /** Reparent a context.  If it currently has a parent set, the
   *  parent and sibling links will be cleared first.
   *
   * @see spt_context_clear_parent
   *
   * @param context The log context to set parent links for.
   *
   * @param parent Parent context.
   */
  void
  spt_context_set_parent(spt_context_t* context, spt_context_t* parent);


  /** Clear a context's parent and sibling links.
   *
   * @param context The context to detach from its parent's hierarchy.
   */
  void
  spt_context_clear_parent(spt_context_t* context);


  /** Callback (delegate) type used with spt_context_each_child. */
  typedef uint8_t (spt_context_user_callback_t)(spt_context_t* context, void* userdata);

  /** Call a function on each of a context's child nodes.
   *
   * @param context Context containing target child contexts.
   *
   * @param callback Pointer to the function to call on each child
   *                 context.
   *
   * @param userdata Extra data-pointer to pass to @p callback.  May be
   *                 @c NULL.
   *
   */
  uint8_t
  spt_context_each_child(const spt_context_t* context,
                         spt_context_user_callback_t* callback,
                         void* userdata);
  /**@}*/

  /* ******************************** */

  /** @name Activation
   *
   * Use these functions to explicitly enable or disable a context, or
   * reset it to its inherited state.
   *
   * @{
   */

  /** Explicitly enable a context.  Any child contexts that have not
   * been explicitly enabled or disabled will also be enabled.
   *
   * @param context The context to enable.
   */
  void
  spt_context_enable(spt_context_t* context);


  /** Explicitly disable a context.  Any child contexts that have not
   * been explicitly enabled or disabled will also be disabled.
   *
   * @param context The context to disable.
   */
  void
  spt_context_disable(spt_context_t* context);

  /** Reset a context to implicit (inherited) activation.  If the
   * context has a parent context, the parent context's activation
   * state will be inherited.  If the context has any child contexts,
   * the inherited state will be recursively propagated to those child
   * contexts which have not been explicitly activated or deactivated.
   *
   * @param context The context to reset.
   */
  void
  spt_context_reset(spt_context_t* context);
  /**@}*/

  /** @name Logging
   *
   * Yeah, we do that too.
   *@{
   */
  /** Interface macro for logging messages in a given context.
   *
   * @param cxt The context in which to log.
   *
   * @param ... Additional arguments are eventually passed to mlog.
   *
   * @see cmlog_real
   * @see mlog
   */
#define cmlog(cxt, ...) if ( spt_context_active(cxt) ) cmlog_real(cxt, __VA_ARGS__)

  /** Alias for cmlog */
#define spt_logv cmlog

  /** Context-enabled version of mlog.
   *
   * @warning Do not call this function directly; use the cmlog macro instead.
   *
   * @param context The context in which the message is to be logged.
   *
   * @copydetails mlog
   */
  int
  cmlog_real(const spt_context_t* context, const unsigned long spec, const char* fmt, ...);
  /**@}*/
  /**@}*/
#ifdef __cplusplus
}
#endif

#endif	/* SPT_CONTEXT_H */

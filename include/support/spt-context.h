/* -*- Mode: C; fill-column: 80 */
#ifndef SPT_CONTEXT_H
#define SPT_CONTEXT_H	1

#include <support/support-config.h>

/** @defgroup context Contexts
 *
 * Contexts provide a flexible logging and output framework that
 * emphasizes both programmer and end-user control.  Log contexts
 * allow for:
 *
 *   - Runtime activation or deactivation of a given context
 *   - Hierarchical context manipulation
 *     - Implicit activation inheritance
 *     - Explicit overriding of inherited activation states
 */

#ifdef __cplusplus
extern "C"
{
#endif

  /** @ingroup context
   *@{
   */
  /** Opaque object interface to the logging context system.  Use the
   * following functions to manipulate a log context.
   */
  typedef struct __spt_context spt_context_t;

  /** Opaque interface to the output handler objects for spt_context.
   */
  typedef struct __spt_context_handler spt_context_handler_t;
  /**@}*/

#ifdef __cplusplus
}
#endif

#include <support/private/spt-context.h>
#include <support/mlog.h>

#ifdef __cplusplus
extern "C"
{
#endif
  /** @name Output handlers
   *  @ingroup context
   *
   *
   * @{
   */

  /**@name Handler function prototypes
   *@{
   */
  /** Data-output function prototype.
   *
   * <strong>Requirements:</strong> Any implementations of this
   * function should
   *   - Be completely thread-safe.
   *   - Format or interpret the data only to the extent needed for
   *     output.
   *   - Write the data to the handler's output as appropriate.
   *
   * Implementations should NOT
   *   - Modify structures pointed to by the function arguments
   *   - Interpret or reformat the content of the data.
   *   - Check if the context is active.  If the context is not
   *     active, the function will not be called.
   *
   *
   * @param context A reference to the context in (for) which the
   * handler function has been called.
   *
   * @param data Pointer to a character buffer containing output data.
   *
   * @param length Number of bytes to be written.
   *
   * @return Number of bytes written, or &lt; 0 if an error was
   * encountered.
   */
  typedef int (*spt_context_write_func_t)(const spt_context_t* context,
					  const char* data,
					  const ssize_t length);


  /** Message format function
   */
  typedef int (*spt_context_format_func_t)(const spt_context_t* context,
					   const spt_loglevel_t level,
					   const char* message);
  /**@}*/


  typedef struct __spt_context_handler
  {
#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
    uint32_t magic;
#endif
    spt_context_write_func_t write;
    spt_context_format_func_t format;
  } spt_context_handler_t;
  /**@}*/


  /**@name Creation/Destruction
   *
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
   * @param description Description of what the logging context is used for.
   * Currently unused (may be @c NULL).
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

  /** Use a string value to enable or disable multiple contexts.
   * Parses the passed string, and searches for similarly-named
   * contexts under the given root context.
   *
   * @param spec A string specifying the contexts to enable or
   * disable. In <a href="http://www.rfc-editor.org/std/std68.txt">ABNF</a>
   * notation:
   * <code>
   * spec		= single_spec *(","  single_spec)
   *
   * single_spec	= state_flag identifier ; Enable or disable a single context.
   *
   * state_flag		= "+" / "-"
   *
   * identifier		= context_name *("." context_name)
   *
   * context_name	= &lt;any CHAR excluding "." and ","&gt;
   * </code>
   *
   * To enable a single context: <code>+context_name</code>
   *
   * To enable a certain (single) child context with a non-unique
   * name: <code>+parent_name.child_name</code>
   *
   *
   * @return The number of individual contexts enabled or disabled, or
   * a negative error code if an error was encountered.
   */
  int
  spt_context_parse_spec(/*spt_context_t* root, */const char* spec);

  /**@}*/


  /** Interface macro for logging messages in a given context.
   *
   * @param cxt The context in which to log.
   *
   * @param ... Additional arguments are eventually passed to mlog.
   *
   * @see cmlog_real
   * @see mlog
   */
#define cmlog(cxt, ...) if ( spt_context_active(cxt) ) cxt->handler->log(cxt, __VA_ARGS__)

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

#ifdef __cplusplus
}
#endif

#endif	/* SPT_CONTEXT_H */

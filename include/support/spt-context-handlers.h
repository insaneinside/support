/* -*- Mode: C; fill-column: 70 -*- */
/** @file support/spt-context-handlers.h
 *
 * Output handler definitions for log contexts.
 */
#ifndef support_spt_context_handlers_h
#define support_spt_context_handlers_h 1

#ifdef __cplusplus
extern "C"
{
#endif

  /** @defgroup context_handlers Output handlers
   *  @ingroup context
   *
   * @brief Modular output handling for log contexts.
   * @{
   */
  /** Opaque interface to the output handler objects for spt_context.
   */
  typedef struct __spt_context_handler spt_context_handler_t;

  /**@name Handler function prototypes
   *@{
   */
  /** Data-output function prototype.
   *
   * @requirements
   * Any implementations of this should
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


  /** Log message format function prototype.
   *
   * @requirements
   * Any implementations of this should
   *   - Be completely thread-safe
   *
   */
  typedef int (*spt_context_format_func_t)(const spt_context_t* context,
					   const spt_loglevel_t level,
					   const char* message,
					   void* output_dest);
  /**@}*/


  struct __spt_context_handler
  {
#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
    uint32_t magic;
#endif

    spt_context_write_func_t write;
    spt_context_format_func_t format;
  };
  /**@}*/
  /**@}*/

#ifdef __cplusplus
}
#endif

#endif	/* defined(support_spt_context_handlers_h) */

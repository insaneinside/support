/* -*- Mode: C; fill-column: 70 -*- */
/** @file support/spt-context-callbacks.h
 *
 * Functions and structures for runtime notification of changes in
 * context state.
 */
#ifndef support_spt_context_callbacks_h
#define support_spt_context_callbacks_h 1

#include <support/support-config.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef SPT_CONTEXT_ENABLE_CALLBACKS
  /** Callback container structure for log contexts.  This provides a
   * concise way to specify and store the collection of callbacks for
   * one or more contexts.
   *
   * @note Like a context's state, its callbacks are also inherited by
   *   its children; however, unlike its state, explicitly setting a
   *   set of callbacks on a child will <em>not</em> disable the
   *   inherited callbacks. \par
   *    To disable inherited callbacks for a subtree starting at
   *   <code>cxt</code>, use
   *   @code
   * spt_context_callbacks_set_inheritance(cxt, SPT_CONTEXT_CALLBACKS_NO_INHERITANCE);
   *   @endcode
   *
   * @ingroup context
   */
  typedef struct __spt_context_callbacks
  {
    /** @name Initialization and destruction
     *@{
     */
    /** Pointer-to-function called immediately after the context is
     *	created.
     */
    void (*post_create)(struct __spt_context* context, void* user_data);

    /** Pointer-to-function called immediately before the context is
     * destroyed.
     */
    void (*pre_destroy)(struct __spt_context* context, void* user_data);
    /**@}*/

    /** @name Hierarchy changes
     *@{
     */
    /** Pointer-to-function called when a new sibling has been added.  */
    void (*post_sibling_add)(struct __spt_context* this_context, struct __spt_context* sibling, void* user_data);


    /** Pointer-to-function called before a sibling is removed.  */
    void (*pre_sibling_remove)(struct __spt_context* this_context, struct __spt_context* sibling, void* user_data);


    /** Pointer-to-function called when a new child has been added. */
    void (*post_child_add)(struct __spt_context* this_context, struct __spt_context* child);


    /** Pointer-to-function called before a child is removed. */
    void (*pre_child_remove)(struct __spt_context* this_context, struct __spt_context* child);
  /**@}*/
  } spt_context_callbacks_t;
#endif  /* defined(SPT_CONTEXT_ENABLE_CALLBACKS) */

#endif	/* support_spt_context_callbacks_h */

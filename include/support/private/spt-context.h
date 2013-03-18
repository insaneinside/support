/** @file support/private/spt-context.h
 * @internal
 */
#ifndef SUPPORT_PRIVATE_SPT_CONTEXT_H
#define SUPPORT_PRIVATE_SPT_CONTEXT_H

#include <support/support-config.h>

#ifdef __cplusplus
extern "C"
{
#endif

  /** @addtogroup context
   *@{
   */
  /** Flags that detail the state of a log context.
   * @internal
   */
  enum spt_context_flags
    {
      /** Indicates which of the two policies -- implicit or explicit --
       * currently governs the context.  If set, the explicit policy is
       * being used.
       */
      SPT_CONTEXT_POLICY	= 1 << 0,

      /** Indicates the state of the explicit policy.  If set, the
       * context is active under this policy.
       */
      SPT_CONTEXT_EXPLICIT_STATE	= 1 << 1,

      /** Indicates the (in)active state of the implicit policy.  If
       * set, the context is active under this policy.
       */
      SPT_CONTEXT_IMPLICIT_STATE	= 1 << 2,

      /** If set, the name of a context and any parent context(s) will
       * be hidden when messages are logged to that context.
       */
      SPT_CONTEXT_HIDE_NAME = 1 << 3,

      /** If set, a context will not be enabled through implicit
       *	state-inheritance.
       */
      SPT_CONTEXT_NO_IMPLICIT_STATE = 1 << 4
    };

  /** Log context data structure.
   *  @internal
   */
  struct __spt_context
  {
#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
    /** Magic number. */
    uint32_t magic;
#endif

    /** Context ID */
    unsigned long int id;

    /** Symbolic name */
    char* name;

    /** @internal Fully-scoped name (the context-identifier). */
    char* full_name;


#ifdef SPT_CONTEXT_ENABLE_DESCRIPTION
    /** Description string, in case the user asks for a list of available contexts. */
    char* description;
#endif

#ifdef SPT_CONTEXT_ENABLE_OUTPUT_HANDLERS
    /** Output handler for this context.
     */
    spt_context_handler_t* output_handler;
#endif

    /** State flags.
     * @see spt_context_flags
     */
    unsigned long int flags;

    /**@name Context relations
     *@{
     */

    /** The context of which this is a subcontext.
     */
    struct __spt_context* parent;

    /** List of child contexts -- first child */
    struct __spt_context* first_child;

    /** List of child contexts -- last child */
    struct __spt_context* last_child;

    /** Next sibling context */
    struct __spt_context* next_sibling;

    /** Previous sibling context. */
    struct __spt_context* prev_sibling;
    /**@}*/
  };

  /**@}*/

  /** @ingroup context_spec */
  struct __spt_context_parse_spec
  {
#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
    uint32_t magic;    /**< Magic number */
#endif

    /** Value of flags (below) will override value of flags in matching
     * contexts for bits set in this mask.
     */
    unsigned long int mask;

    /** IOR flags field of any matching context with this value.
     */
    unsigned long int flags;

    /** Copy of the single_spec input string.
     */
    char* input;
 
    /** Number of elements in name_array.
     */
    size_t name_array_length;

    /** Context name hierarchy.  This parse spec matches any context
     *  whose full name ends with the elements in name_array.
     */
    char** name_array;

    /** Pointer to the next parse spec in the list (if any).  */
    struct __spt_context_parse_spec* next;
  };

  /** Determine if a context is activated.
   *
   * @param cxt Pointer to the context to examine
   *
   * @return @c 1 if the context is active, @c 0 if the context is
   * inactive.
   */
#define spt_context_active(cxt) ( SPT_IS_CONTEXT(cxt) ? spt_context_state(cxt->flags) : 0 )

  /** Extract the activation state of a context from its flags variable.
   *
   * @internal
   *
   * @param flags Context's `flags' variable.
   *
   * @return @c 1 if the context is active, @c 0 if the context is
   * inactive.
   */
#define spt_context_state(flags)		\
  ( flags & SPT_CONTEXT_POLICY			\
    ? flags & SPT_CONTEXT_EXPLICIT_STATE	\
    : flags & SPT_CONTEXT_IMPLICIT_STATE )

  /** Name separator string used when building the full name of
   * non-top-level contexts.
   * @internal
   */
  #define SPT_CONTEXT_NAME_SEPARATOR "."
  /** Length of SPT_CONTEXT_NAME_SEPARATOR.
   * @internal
   */
  #define SPT_CONTEXT_NAME_SEPARATOR_LENGTH 1

  #ifdef SPT_ENABLE_CONSISTENCY_CHECKS
    /** Magic number for context structures.  This is useful during
     *  debugging.
     * @internal
     */
    #define SPT_CONTEXT_MAGIC  ( ( 'M' << 24 ) + ( 'C' << 16 ) + ( 'X' << 8 ) + 'T' )

    /** Magic number for context parse-spec structures.  This is useful during
     *  debugging.
     * @internal
     */
    #define SPT_CONTEXT_PARSE_SPEC_MAGIC ( ( 'S' << 24 ) + ( 'P' << 16 ) + ( 'E' << 8 ) + 'C' )

    /** Check if the argument is a well-initialized log context.
     * @internal
     */
    #define SPT_IS_CONTEXT(cxt)						\
      ( cxt && ((spt_context_t*) cxt)->magic == SPT_CONTEXT_MAGIC )

    /** Check if the argument is a well-initialized context parse-spec.
     * @internal
     */
    #define SPT_IS_CONTEXT_PARSE_SPEC(pspec)				\
      ( pspec && ((spt_context_parse_spec_t*) pspec)->magic == SPT_CONTEXT_PARSE_SPEC_MAGIC )

    #define SPT_CONTEXT_HAS_CHILDREN(cxt) ( SPT_IS_CONTEXT(cxt) && cxt->first_child != NULL )
  #else
    #define SPT_IS_CONTEXT(cxt) (cxt)
    #define SPT_IS_CONTEXT_PARSE_SPEC(pspec) (pspec)
    #define SPT_CONTEXT_HAS_CHILDREN(cxt) (cxt->first_child != NULL )
  #endif

  /**@}*/

#ifdef __cplusplus
}
#endif

#endif	/* SUPPORT_PRIVATE_SPT_CONTEXT_H */

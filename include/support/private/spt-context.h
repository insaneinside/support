#ifndef SUPPORT_PRIVATE_SPT_CONTEXT_H
#define SUPPORT_PRIVATE_SPT_CONTEXT_H

#include <support/support-config.h>
#include <support/dllist.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** Flags that detail the state of a log context.
 */
enum spt_context_flags
  {
    /** Indicates which of the two policies -- implicit or explicit --
     * currently governs the context.  If set, the explicit policy is
     * active.
     */
    SPT_CONTEXT_POLICY	= 1 << 0,

    /** Indicates the (in)active state of the explicit policy.
     */
    SPT_CONTEXT_EXPLICIT_STATE	= 1 << 1,

    /** Indicates the (in)active state of the implicit policy.
     */
    SPT_CONTEXT_IMPLICIT_STATE	= 1 << 2,

    /** If set, the name of a context and any parent context(s) will
     * be hidden when messages are logged to that context.
     */
    SPT_CONTEXT_HIDE_NAME = 1 << 3
};

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

  /** @internal Fully-scoped name */
  char* full_name;

#ifdef SPT_CONTEXT_ENABLE_DESCRIPTION
  /** Description string, in case the user asks for a list of available contexts */
  char* description;
#endif

  /** Output handler for this context.
   */
  spt_context_handler_t output_handler;

  /** State flags.
   * @see spt_context_flags
   */
  unsigned long int flags;

  /** List of child contexts */
  dllist_t* children;

  /** If not NULL, the context of which this is a subcontext.
   */
  struct __spt_context* parent;
};

/** Determine the state -- active or inactive -- of a context.
 *
 * @param cxt A pointer to the context
 */
#define spt_context_active(cxt) ( SPT_IS_CONTEXT(cxt) ? spt_context_state(cxt->flags) : 0 )

/** @copybrief spt_context_active
 *
 * @param flags Context's `flags' variable.
 */
#define spt_context_state(flags)		\
  ( flags & SPT_CONTEXT_POLICY			\
    ? flags & SPT_CONTEXT_EXPLICIT_STATE	\
    : flags & SPT_CONTEXT_IMPLICIT_STATE )

#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
#define SPT_CONTEXT_MAGIC  ( ( 'M' << 3 ) + ( 'C' << 2 ) + ( 'X' << 1 ) + 'T' )
#define SPT_IS_CONTEXT(cxt) \
  ( cxt && ((spt_context_t*) cxt)->magic == SPT_CONTEXT_MAGIC )
#else
#define SPT_IS_CONTEXT(cxt) (cxt)
#endif

#ifdef __cplusplus
}
#endif

#endif	/* SUPPORT_PRIVATE_SPT_pCONTEXT_H */

#ifndef SUPPORT_PRIVATE_MLOG_CONTEXT_H
#define SUPPORT_PRIVATE_MLOG_CONTEXT_H

#include <support/dllist.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** Flags that detail the state of a log context.
 */
enum mlog_context_flags
  {
    /** Indicates which of the two policies -- implicit or explicit --
     * currently governs the context.  If set, the explicit policy is
     * active.
     */
    MLOG_CONTEXT_POLICY	= 1 << 0,

    /** Indicates the (in)active state of the explicit policy.
     */
    MLOG_CONTEXT_EXPLICIT_STATE	= 1 << 1,

    /** Indicates the (in)active state of the implicit policy.
     */
    MLOG_CONTEXT_IMPLICIT_STATE	= 1 << 2

};

struct __mlog_context
{
  /** Magic number. */
  uint32_t magic;

  /** Context ID */
  unsigned long int id;

  /** Symbolic name */
  char* name;

  /** Fully-scoped name */
  char* full_name;

  /** Description string, in case the user asks for a list of available contexts */
  char* description;

  /** Hash of @v name, to speed up symbolic lookups.
   */
  /* hash_t name_hash; */

  /* /\** Non-zero if the context is active.
   *  *\/
   * int active; */

  /** State flags.
   * @see mlog_context_flags
   */
  unsigned long int flags;

  /** List of child contexts */
  dllist_t* children;
  /* struct __mlog_context* children;
   * unsigned int num_children; */

  /** If not NULL, the context of which this is a subcontext.
   */
  struct __mlog_context* parent;
};

/** Determine the state -- active or inactive -- of a context.
 *
 * @param cxt A pointer to the context
 */
#define mlog_context_active(cxt) mlog_context_state(cxt->flags)

/** @copybrief mlog_context_active
 *
 * @param flags Context's `flags' variable.
 */
#define mlog_context_state(flags)		\
  ( flags & MLOG_CONTEXT_POLICY			\
    ? flags & MLOG_CONTEXT_EXPLICIT_STATE	\
    : flags & MLOG_CONTEXT_IMPLICIT_STATE )

#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
#define CONTEXT_MAGIC  ( ( 'M' << 3 ) + ( 'C' << 2 ) + ( 'X' << 1 ) + 'T' )
#define MLOG_IS_CONTEXT(cxt) \
  ( cxt && ((mlog_context_t*) cxt)->magic == CONTEXT_MAGIC )
#else
#define MLOG_IS_CONTEXT(cxt) 1
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#endif	/* SUPPORT_PRIVATE_MLOG_CONTEXT_H */

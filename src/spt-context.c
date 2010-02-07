#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

#ifndef NDEBUG
#include <assert.h>
#else
#define assert(expression)
#endif  /* NDEBUG */

#include <support/support-config.h>
#include <support/spt-context.h>
#include <support/macro.h>

#define SPT_LOCK_EXCLUSIVE(target)
#define SPT_UNLOCK_EXCLUSIVE(target)

#define CMLOG_FORMAT "[%s] %s"
#define CMLOG_FORMAT_NONAME "%s"
static unsigned long int context_id_base = 0;

#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
#include <assert.h>
#endif

#define LEVEL(cmlog_flags)	(cmlog_flags & MLOG_LOGLEVEL_MASK )

#ifdef SPT_CONTEXT_ENABLE_DESCRIPTION
#define CONTEXT_ALLOC_SIZES(parent,name,description)			\
  size_t name_alloc_size = strlen(name) + 1;				\
  size_t fullname_alloc_size = ( parent && ! ( parent->flags & SPT_CONTEXT_HIDE_NAME ) ) \
    ? ( strlen(parent->full_name) + SPT_CONTEXT_NAME_SEPARATOR_LENGTH + name_alloc_size ) \
    : 0; /* will be using name pointer */				\
  size_t description_alloc_size = description ? strlen(description) + 1 : 0; \
  size_t alloc_size                                                    \
    = sizeof(spt_context_t)                                            \
    + name_alloc_size                                                  \
    + fullname_alloc_size                                              \
    + description_alloc_size
#else
#define CONTEXT_ALLOC_SIZES(parent,name)				\
  size_t name_alloc_size = strlen(name) + 1;				\
  size_t fullname_alloc_size = ( parent && ! ( parent->flags & SPT_CONTEXT_HIDE_NAME ) ) \
    ? ( strlen(parent->full_name) + SPT_CONTEXT_NAME_SEPARATOR_LENGTH + name_alloc_size ) \
    : 0; /* will be using name pointer */				\
  size_t alloc_size                                                    \
    = sizeof(spt_context_t)                                            \
    + name_alloc_size                                                  \
    + fullname_alloc_size
#endif

uint8_t
spt_context_each_child(const spt_context_t* context,
                       spt_context_user_callback_t* callback,
                       void* userdata)
{
  uint8_t times_called = 0;

  if ( SPT_IS_CONTEXT(context) && context->first_child )
    {
      spt_context_t* node = NULL;
      spt_context_t* next = context->first_child;;

      while ( next )
	{
          node = next;
	  next = node->next_sibling;

          callback(node, userdata);
          times_called++;
	}
    }

  return times_called;
}

/** Append a child node to a context.
 *
 * @param cxt Context to append @p child to.
 *
 * @param child Context to append to @p cxt.
 *
 * @internal
 */
__inline__ void
context_append_child(spt_context_t* cxt, spt_context_t* child)
{
  /* Do we need to initialize the first_child list pointer, or just
   * append after last_child?
   */
  if ( cxt->first_child )
    {
      cxt->last_child->next_sibling = child;
      child->prev_sibling = cxt->last_child;
    }
  else
    /* First child appended. */
    cxt->first_child = child;

  /* Reset the last_child pointer, and the new child's next_sibling
   * and parent pointers.
   */
  cxt->last_child = child;
  cxt->last_child->next_sibling = NULL;
  cxt->last_child->parent = cxt;
}

/** Remove a context from its list of siblings.
 *
 * Doesn't modify the context's parent object.
 *
 * @internal
 */
__inline__ uint8_t
context_unlink_parent_and_siblings(spt_context_t* cxt)
{
  if ( cxt->prev_sibling != NULL )
    cxt->prev_sibling->next_sibling = cxt->next_sibling;
  if ( cxt->next_sibling != NULL )
    cxt->next_sibling->prev_sibling = cxt->prev_sibling;

  cxt->prev_sibling = NULL;
  cxt->next_sibling = NULL;
  cxt->parent = NULL;
  return 0;
}

/** Remove a child node from a context's list of child contexts.
 *
 * @internal
 */
__inline__ uint8_t
context_remove_child(spt_context_t* cxt, spt_context_t* child)
{
  assert( SPT_CONTEXT_HAS_CHILDREN(cxt) );

  /* If it's the first or last child entry, we need to update this
   * context's {first|last}_child links.
   */
  if ( child == cxt->first_child )
    cxt->first_child = child->next_sibling;
  else if ( child == cxt->last_child )
    cxt->last_child = child->prev_sibling;

  /* Unlink the child from this context and its siblings. */
  context_unlink_parent_and_siblings(child);
  return 0;
}

/** Build the full name that should be assigned to a context.
 *
 * @param context Context to build the full name for.
 *
 * @param alloc_size Number of bytes allocated for the @c full_name
 * string.
 *
 */
static char*
context_build_full_name(const spt_context_t* context, size_t alloc_size)
{
  if ( alloc_size <= 0 )
    {
      if ( context->flags & SPT_CONTEXT_HIDE_NAME )
        return NULL;
      else
        return context->name;
    }
  size_t left = alloc_size;
  char* out = NULL;
  char* to = NULL;
  char* lastto = NULL;

  /* If this is not a subcontext, or the parent's name is hidden, just
   * use the immediate name.
   */
  if ( context->parent == NULL || context->parent->flags & SPT_CONTEXT_HIDE_NAME )
    return context->name;

  out = context->full_name;
  to = out;
  lastto = out;

  to = stpncpy(to, context->parent->full_name, left);
  left -= (size_t) (to - lastto);
  lastto = to;
  to = stpncpy(to, SPT_CONTEXT_NAME_SEPARATOR, left);
  left -= (size_t) (to - lastto);
  lastto = to;
  to = stpncpy(to, context->name, left);
  left -= (size_t) (to - lastto);

  /* `to' should point to the nul byte at the end of the string. */
#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
  if ( left != 1 )
    {
      fprintf(stderr, "ERROR: In %s: output buffer has incorrect size (%u left) while building name for context #%lu\n", __func__, (unsigned int) left, context->id);
      abort();
    }
#endif
  return out;
}

/* ----------------------------------------------------------------
 * Context creation and destruction
 */

/** Destroy a single context, without touching its child contexts.
 *
 * @param context The context to destroy.
 *
 * @internal
 */
static void
context_destroy_single(spt_context_t* context)
{
  if ( SPT_IS_CONTEXT(context) )
    {
#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
      context->magic = 0;
#endif	/* SPT_ENABLE_CONSISTENCY_CHECKS */
      free(context);
    }

}

/* /\** Call spt_context_destroy on a node's data.
 *  *\/
 * static int
 * _fe_destroy_context(dllist_t* node, const void* udata)
 * {
 *   if ( node && SPT_IS_CONTEXT(node->data) )
 *     {
 *       spt_context_destroy((spt_context_t*) (node->data));
 *       node->data = NULL;
 *     }
 *   return 1;
 * } */

/** Call spt_context_destroy_recursive on a node's data.
 */
static uint8_t
_fe_destroy_context_recursive(spt_context_t* context,
			      void* udata __attribute__ (( unused )) )
{
  if ( SPT_CONTEXT_HAS_CHILDREN(context) )
    spt_context_destroy_recursive(context);
  else
    context_destroy_single(context);

  return 1;
}

/** Set the context's parent to NULL.
 *
 * @internal
 */
static uint8_t
_fe_unparent_context(spt_context_t* cxt,
		     void* udata __attribute__ (( unused )) )
{
  cxt->parent = NULL;
  return 1;
}

#define assign_and_advance(dest,type,size,source,size_counter)	\
  dest = (type *) source;					\
  source += (ptrdiff_t) size;                                   \
  size_counter -= size

#ifdef SPT_CONTEXT_ENABLE_DESCRIPTION
spt_context_t*
spt_context_create(spt_context_t* parent,
                   const char* name,
                   const char* description)
#else
  spt_context_t*
  spt_context_create(spt_context_t* parent,
                     const char* name)
#endif
{
  unsigned char* buf = NULL;
  spt_context_t* cxt = NULL;

  /* Declare allocation size variables */
#ifdef SPT_CONTEXT_ENABLE_DESCRIPTION
  CONTEXT_ALLOC_SIZES(parent, name, description);
#else
  CONTEXT_ALLOC_SIZES(parent, name);
#endif

#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
  size_t orig_alloc_size = alloc_size;
#endif  /* SPT_ENABLE_CONSISTENCY_CHECKS */

  if ( ! name /*|| ! description*/ )
    return NULL;

  /* Allocate the memory */
  if ( ! ( buf = (unsigned char*) malloc(alloc_size) ) )
    {
      perror("malloc");
      return NULL;
    }
  memset(buf, 0, alloc_size);

  /* Assign pointers */
  assign_and_advance(cxt, spt_context_t, sizeof(spt_context_t),
                     buf, alloc_size);
  assign_and_advance(cxt->name, char, name_alloc_size,
                     buf, alloc_size);
  assign_and_advance(cxt->full_name, char, fullname_alloc_size,
                     buf, alloc_size);
#ifdef SPT_CONTEXT_ENABLE_DESCRIPTION

#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
  assert(alloc_size == description_alloc_size);
#endif  /* SPT_ENABLE_CONSISTENCY_CHECKS */

  assign_and_advance(cxt->description, char, description_alloc_size,
                     buf, alloc_size);
#endif

#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
  assert(buf == (unsigned char*) cxt + orig_alloc_size );
  assert(alloc_size == 0);
#endif  /* SPT_ENABLE_CONSISTENCY_CHECKS */

  /* Set object vars and copy strings */
  SPT_LOCK_EXCLUSIVE(context_id_base);
  cxt->id = context_id_base++;
  SPT_UNLOCK_EXCLUSIVE(context_id_base);

#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
  cxt->magic = SPT_CONTEXT_MAGIC;
#endif

  strncpy(cxt->name, name, name_alloc_size);

#ifdef SPT_CONTEXT_ENABLE_DESCRIPTION
  if ( description )
    strncpy(cxt->description, description, description_alloc_size);
#endif

  /* If parent node given, append the node to the parent's child list
   * and reset the node (reset ensures properly-inherited activation
   * state).
   */
  if ( parent )
    {
      context_append_child(parent, cxt);
      spt_context_reset(cxt);
    }

  cxt->full_name = context_build_full_name(cxt, fullname_alloc_size);

  return cxt;
}

void
spt_context_destroy(spt_context_t* context)
{
  if ( ! context )
    return;

  /* Free list of child contexts */
  if ( SPT_CONTEXT_HAS_CHILDREN(context) )
    spt_context_each_child(context, &_fe_unparent_context, NULL);

  if ( context->parent )
    {
#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
      assert( SPT_CONTEXT_HAS_CHILDREN(context->parent) );
#endif  /* SPT_ENABLE_CONSISTENCY_CHECKS */

      context_remove_child(context->parent, context);
    }
  context_destroy_single(context);
}

void
spt_context_destroy_recursive(spt_context_t* context)
{
  if ( ! context )
    return;

  if ( SPT_CONTEXT_HAS_CHILDREN(context) )
    {
      spt_context_each_child(context, &_fe_destroy_context_recursive, NULL);
    }

  context_destroy_single(context);
}

/* ----------------------------------------------------------------
 * Context (de)activation and policy management.
 */

static void context_inherit_state(spt_context_t* context);

/** Refresh inherited state data by calling context_inherit_state on
 *  each list element.
 *
 * @internal
 */
static uint8_t
_fe_inherit_state(spt_context_t* context,
		  void* udata __attribute__ (( unused )) )
{
  context_inherit_state(context);
  return 1;
}

/** Refresh a context's implicit (inherited) state from the parent
 *  context.
 *
 * @internal
 */
static void
context_inherit_state(spt_context_t* context)
{
  if ( context->parent )
    {
      context->flags &= ~SPT_CONTEXT_POLICY; /* set implicit policy */

      if ( spt_context_active(context->parent) )
	context->flags |= SPT_CONTEXT_IMPLICIT_STATE;
      else
	context->flags &= ~SPT_CONTEXT_IMPLICIT_STATE;
    }

  if ( SPT_CONTEXT_HAS_CHILDREN(context) )
    spt_context_each_child(context, &_fe_inherit_state, NULL);
}


void
spt_context_enable(spt_context_t* context/*, const unsigned int recursive*/)
{
  context->flags |= SPT_CONTEXT_POLICY; /* set policy explicit */
  context->flags |= SPT_CONTEXT_EXPLICIT_STATE; /* enable */

  /* Update child contexts */
  if ( SPT_CONTEXT_HAS_CHILDREN(context) )
    spt_context_each_child(context, &_fe_inherit_state, NULL);
}


void
spt_context_disable(spt_context_t* context)
{
  context->flags |= SPT_CONTEXT_POLICY; /* set policy explicit */
  context->flags &= ~SPT_CONTEXT_EXPLICIT_STATE; /* disable */

  /* Update child contexts */
  if ( SPT_CONTEXT_HAS_CHILDREN(context) )
    spt_context_each_child(context, &_fe_inherit_state, NULL);
}


void
spt_context_reset(spt_context_t* context)
{
  /* Unset policy bit (set to implicit)   */
  context->flags &= ~SPT_CONTEXT_POLICY;

  /* Refresh the inherited state  */
  context_inherit_state(context);
}

#include <support/mlog.h>

int
cmlog_real(const spt_context_t* context, const unsigned long spec, const char* fmt, ...)
{
  va_list ap;
  mlog_loglevel_t lvl = LEVEL(spec);
  char* cfmt = NULL;
  ssize_t ncfmt = -1;
  int r = -1;
  static char* last_full_name = NULL;

  /* Don't do anything if the log context is inactive. */
  if ( ! spt_context_active(context) )
    return 0;

  /* Don't do anything if the global log level is less than the
   * message's log level.
   */
  if ( mlog_get_level() < lvl )
    return 0;
  const char* asfmt = CMLOG_FORMAT;

  /* If the context has the HIDE_NAME flag set, use the appropriate printf
   * format.
   */
  if ( context->flags & SPT_CONTEXT_HIDE_NAME )
    asfmt = CMLOG_FORMAT_NONAME;

  va_start(ap, fmt);

  ncfmt = asprintf(&cfmt, CMLOG_FORMAT, context->full_name, fmt);
  if ( ncfmt < 0 )
    return -1;


  r = mlogv(spec, cfmt, ap);
  last_full_name = context->full_name;
  free(cfmt);
  return r;
}

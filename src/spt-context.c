#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

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
  size_t alloc_size							\
  = sizeof(spt_context_t)						\
    + name_alloc_size							\
    + fullname_alloc_size						\
    + description_alloc_size;
#else
#define CONTEXT_ALLOC_SIZES(parent,name)				\
  size_t name_alloc_size = strlen(name) + 1;				\
  size_t fullname_alloc_size = ( parent && ! ( parent->flags & SPT_CONTEXT_HIDE_NAME ) ) \
    ? ( strlen(parent->full_name) + SPT_CONTEXT_NAME_SEPARATOR_LENGTH + name_alloc_size ) \
    : 0; /* will be using name pointer */				\
  size_t alloc_size							\
  = sizeof(spt_context_t)						\
    + name_alloc_size							\
    + fullname_alloc_size
#endif

/** Build the full name that should be assigned to a context.
 *
 * @param context Context to build the full name for.
 *
 * @param alloc_size 
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
static int
_fe_destroy_context_recursive(dllist_t* node,
			      const void* udata __attribute__ (( unused )) )
{
  if ( node )
    {
      spt_context_t* cxt = (spt_context_t*) (node->data);
      if ( cxt->children )
	spt_context_destroy_recursive(cxt);
      else
	context_destroy_single(cxt);
      node->data = NULL;
    }
  return 1;
}

/** Set the context's parent to NULL.
 */
static int
_fe_unparent_context(dllist_t* node,
		     const void* udata __attribute__ (( unused )) )
{
  if ( node )/* && node->data ) */
    {
      spt_context_t* context = (spt_context_t*) (node->data);
      context->parent = NULL;
    }
  return 1;
}

#define assign_and_advance(dest,type,size,source,size_counter)	\
  dest = (type *) source;					\
  source += (ptrdiff_t) size;				\
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

  if ( parent )
    {
      cxt->parent = parent;
      parent->children = dllist_append(parent->children, cxt);
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

  if ( context->children )
    {
      dllist_foreach(context->children, _fe_unparent_context, NULL);
      dllist_free(context->children);
    }

  if ( context->parent && context->parent->children )
    context->parent->children = dllist_remove(context->parent->children, context);
  context_destroy_single(context);
}

void
spt_context_destroy_recursive(spt_context_t* context)
{
  if ( ! context )
    return;

  if ( context->children )
    {
      dllist_foreach(context->children, _fe_destroy_context_recursive, NULL);
      dllist_free(context->children);
    }

  context_destroy_single(context);
}

/* ----------------------------------------------------------------
 * Context (de)activation and policy management.
 */

static void context_inherit_state(spt_context_t* context);

/* /\** Reset -- to implicit (inherited) -- the context's state policy.
 *  *\/
 * static int
 * _fe_reset_policy(dllist_t* node, const void* udata)
 * {
 *   spt_context_t* context = NULL;
 *   if ( node && node->data )
 *     {
 *       context = (spt_context_t*) (node->data);
 *       spt_context_reset(context);
 *     }
 *   return 1;
 * } */

/** Refresh inherited state data by calling context_inherit_state on
 *  each list element.
 */
static int
_fe_inherit_state(dllist_t* node,
		  const void* udata __attribute__ (( unused )) )
{
  spt_context_t* context = NULL;
  if ( node && node->data )
    {
      context = (spt_context_t*) (node->data);
      context_inherit_state(context);
    }
  return 1;
}

/** Refresh a context's implicit (inherited) state from the parent
 *  context.
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

  if ( context->children )
    dllist_foreach(context->children, _fe_inherit_state, NULL);
}


void
spt_context_enable(spt_context_t* context/*, const unsigned int recursive*/)
{
  context->flags |= SPT_CONTEXT_POLICY; /* set policy explicit */
  context->flags |= SPT_CONTEXT_EXPLICIT_STATE; /* enable */

  /* Update child contexts */
  if ( context->children )
    dllist_foreach(context->children, _fe_inherit_state, NULL);
}


void
spt_context_disable(spt_context_t* context)
{
  context->flags |= SPT_CONTEXT_POLICY; /* set policy explicit */
  context->flags &= ~SPT_CONTEXT_EXPLICIT_STATE; /* disable */

  /* Update child contexts */
  if ( context->children )
    dllist_foreach(context->children, (dllist_func) _fe_inherit_state, NULL);
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

  if ( ! spt_context_active(context) )
    return 0;

  if ( mlog_get_level() < lvl )
    return 0;
  const char* asfmt = CMLOG_FORMAT;
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

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <support/mlog.h>
#include <support/hash_table.h>
#include <support/macro.h>

#define CONTEXT_NAME_SEPARATOR ":"
#define CONTEXT_NAME_SEPARATOR_LENGTH 1
#define CMLOG_FORMAT "[%s] %s"
static __id_t context_id_base = 0;

#define LEVEL(spec)	(spec & MLOG_LOGLEVEL_MASK )

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
  /** Context ID */
  __id_t id;

  /** Magic number. */
  uint32_t magic;

  /** Symbolic name */
  char* name;

  /** Fully-scoped name */
  char* full_name;

  /** Description string, in case the user asks for a list of available contexts */
  char* description;

  /** Hash of @v name, to speed up symbolic lookups.
   */
  hash_t name_hash;

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

#define CONTEXT_MAGIC  ( ( 'M' << 3 ) + ( 'C' << 2 ) + ( 'X' << 1 ) + 'T' )
#define MLOG_IS_CONTEXT(cxt) \
  ( cxt && ((mlog_context_t*) cxt)->magic == CONTEXT_MAGIC )
/* *((uint32_t*) cxt + offsetof(mlog_context_t, magic)) == CONTEXT_MAGIC ) */

/** Determine the state -- active or inactive -- of a context.
 *
 * @param flags Value of the context's `flags' variable.
 */
inline unsigned long int
context_state(unsigned long int flags)
{
  return flags & MLOG_CONTEXT_POLICY ?
    flags & MLOG_CONTEXT_EXPLICIT_STATE : flags & MLOG_CONTEXT_IMPLICIT_STATE;
}


/** Build the full name that should be assigned to a context.
 */
static char*
context_build_full_name(const mlog_context_t* context)
{
  size_t left = -1;
  char* out = NULL;
  char* to = NULL;
  char* lastto = NULL;

  /* If this is not a subcontext, just use the immediate name. */
  if ( ! context->parent )
    return context->name;/* strdup(context->name); */

  /* Calculate full_name buffer size. */
  left =
    strlen(context->parent->full_name)
    + CONTEXT_NAME_SEPARATOR_LENGTH
    + strlen(context->name) + 1;

  out = (char*) malloc(sizeof(char) * left);
  if ( !out )
    {
      perror("malloc");
      return NULL;
    }
  to = out;
  lastto = out;

  to = stpncpy(to, context->parent->full_name, left);
  left -= to - lastto;
  lastto = to;
  to = stpncpy(to, CONTEXT_NAME_SEPARATOR, left);
  left -= to - lastto;
  lastto = to;
  to = stpncpy(to, context->name, left);
  left -= to - lastto;

  /* `to' should point to the nul byte at the end of the string. */
  if ( left != 1 )
    mlog(V_DEBUG, "In %s: output buffer has incorrect size (%d left).", __func__, left);

  return out;
}

/* ----------------------------------------------------------------
 * Context creation and destruction
 */

static void
context_destroy_single(mlog_context_t* context)
{
  if ( MLOG_IS_CONTEXT(context) )
    {
      context->magic = 0;

      mlog(V_DEBUG | F_NONEWLINE, "%s: destroying \"%s\"... ", __func__, context->name);

      /* if ( context->name ) */

      /* full_name may just be a copy of name pointer */
      if ( context->full_name && context->full_name != context->name )
	free(context->full_name);
      if ( context->name )
	free(context->name);
      if ( context->description )
	free(context->description);
      free(context);

      mlog(V_DEBUG, "done.");
    }

}

/* /\** Call mlog_context_destroy on a node's data.
 *  *\/
 * static int
 * _fe_destroy_context(dllist_t* node, const void* udata)
 * {
 *   if ( node && MLOG_IS_CONTEXT(node->data) )
 *     {
 *       mlog_context_destroy((mlog_context_t*) (node->data));
 *       node->data = NULL;
 *     }
 *   return 1;
 * } */

/** Call mlog_context_destroy_recursive on a node's data.
 */
static int
_fe_destroy_context_recursive(dllist_t* node, const void* udata)
{
  if ( node )
    {
      mlog_context_t* cxt = (mlog_context_t*) (node->data);
      if ( cxt->children )
	mlog_context_destroy_recursive(cxt);
      else
	context_destroy_single(cxt);
      node->data = NULL;
    }
  return 1;
}

/** Set the context's parent to NULL.
 */
static int
_fe_unparent_context(dllist_t* node, const void* udata)
{
  if ( node )/* && node->data ) */
    {
      mlog_context_t* context = (mlog_context_t*) (node->data);
      context->parent = NULL;
    }
  return 1;
}

mlog_context_t*
mlog_context_create(mlog_context_t* parent,
		    const char* name, const char* description)
{
  mlog_context_t* o = NULL;

  if ( ! name || ! description )
    return NULL;

  if ( ! ( o = (mlog_context_t*) malloc(sizeof(mlog_context_t)) ) )
    {
      perror("malloc");
      return NULL;
    }

  memset(o, 0, sizeof(mlog_context_t));

  o->id = context_id_base++;
  o->magic = CONTEXT_MAGIC;
  o->name = strdup(name);
  o->description = strdup(description);

  if ( parent )
    {
      o->parent = parent;
      parent->children = dllist_append(parent->children, o);
    }

  o->full_name = context_build_full_name(o);

  if ( !o->full_name )
    {
      free(o->name);
      free(o->description);
      free(o);
      return NULL;
    }

  return o;
}

void
mlog_context_destroy(mlog_context_t* context)
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
mlog_context_destroy_recursive(mlog_context_t* context)
{
  if ( ! context )
    return;

  mlog(V_DEBUG, "%s: destroying \"%s\"", __func__, context->name);
  if ( context->children )
    {
      dllist_foreach(context->children, _fe_destroy_context_recursive, NULL);
      dllist_free(context->children);
      mlog(V_DEBUG, "done.");
    }

  context_destroy_single(context);
  mlog(V_DEBUG, "%s: done.", __func__);
}

/* ----------------------------------------------------------------
 * Context (de)activation and policy management.
 */

static void context_inherit_state(mlog_context_t* context);

/* /\** Reset -- to implicit (inherited) -- the context's state policy.
 *  *\/
 * static int
 * _fe_reset_policy(dllist_t* node, const void* udata)
 * {
 *   mlog_context_t* context = NULL;
 *   if ( node && node->data )
 *     {
 *       context = (mlog_context_t*) (node->data);
 *       mlog_context_reset(context);
 *     }
 *   return 1;
 * } */

/** Refresh inherited state data by calling context_inherit_state on
 *  each list element.
 */
static int
_fe_inherit_state(dllist_t* node, const void* udata)
{
  mlog_context_t* context = NULL;
  if ( node && node->data )
    {
      context = (mlog_context_t*) (node->data);
      context_inherit_state(context);
    }
  return 1;
}

/** Refresh a context's implicit (inherited) state from the parent
 *  context.
 */
static void
context_inherit_state(mlog_context_t* context)
{
  if ( context->parent )
    {
      if ( context_state(context->parent->flags) )
	context->flags |= MLOG_CONTEXT_IMPLICIT_STATE;
      else
	context->flags &= ~MLOG_CONTEXT_IMPLICIT_STATE;
    }

  if ( context->children )
    dllist_foreach(context->children, _fe_inherit_state, NULL);
}


void
mlog_context_enable(mlog_context_t* context/*, const unsigned int recursive*/)
{
  context->flags |= MLOG_CONTEXT_POLICY; /* set policy explicit */
  context->flags |= MLOG_CONTEXT_EXPLICIT_STATE; /* enable */

  /* Update child contexts */
  if ( context->children )
    dllist_foreach(context->children, _fe_inherit_state, NULL);
}


void
mlog_context_disable(mlog_context_t* context)
{
  context->flags |= MLOG_CONTEXT_POLICY; /* set policy explicit */
  context->flags &= ~MLOG_CONTEXT_EXPLICIT_STATE; /* disable */

  /* Update child contexts */
  if ( context->children )
    dllist_foreach(context->children, (dllist_func) _fe_inherit_state, NULL);
}


void
mlog_context_reset(mlog_context_t* context)
{
  /* Unset policy bit (set to implicit)   */
  context->flags &= ~MLOG_CONTEXT_POLICY;

  /* Refresh the inherited state  */
  context_inherit_state(context);
}

/* ----------------------------------------------------------------
 * Context-based logging
 */
int
cmlog(const mlog_context_t* context, const unsigned long spec, const char* fmt, ...)
{
  va_list ap;
  mlog_loglevel_t lvl = LEVEL(spec);
  char* cfmt = NULL;
  size_t ncfmt = -1;
  int r = -1;

  if ( ! context_state(context->flags) )
    return 0;

  if ( mlog_get_level() < lvl )
    return 0;

  va_start(ap, fmt);

  ncfmt = asprintf(&cfmt, CMLOG_FORMAT, context->full_name, fmt);
  if ( ncfmt < 0 )
    return -1;


  r = mlogv(spec, cfmt, ap);
  free(cfmt);
  return r;
}
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

#include <support/mlog.h>
#include <support/macro.h>

#define CONTEXT_NAME_SEPARATOR "."
#define CONTEXT_NAME_SEPARATOR_LENGTH 1
#define CMLOG_FORMAT "[%s] %s"
#define CMLOG_FORMAT_NONAME "%s"
static unsigned long int context_id_base = 0;
static dllist_t* parse_spec_list = NULL;

#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
#include <assert.h>
#define PARSE_SPEC_MAGIC ( ( 'S' << 3 ) + ( 'P' << 2 ) + ( 'E' << 1 ) + 'C' )
#endif

struct parse_spec
{
#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
  uint32_t magic;
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

  size_t name_array_length;
  char** name_array;
};

#define LEVEL(cmlog_flags)	(cmlog_flags & MLOG_LOGLEVEL_MASK )

/** Build the full name that should be assigned to a context.
 */
static char*
context_build_full_name(const mlog_context_t* context)
{
  size_t left = -1;
  char* out = NULL;
  char* to = NULL;
  char* lastto = NULL;

  /* If this is not a subcontext, or the parent's name is hidden, just
   * use the immediate name.
   */
  if ( context->parent == NULL || context->parent->flags & MLOG_CONTEXT_HIDE_NAME )
    return context->name;

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
#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
  if ( left != 1 )
    mlog(V_DEBUG, "In %s: output buffer has incorrect size (%d left).", __func__, left);
#endif
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

      /* full_name may just be a copy of name pointer */
      if ( context->full_name && context->full_name != context->name )
	free(context->full_name);
      if ( context->name )
	free(context->name);
      if ( context->description )
	free(context->description);
      free(context);
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

static int
_apply_pspec(const struct parse_spec* ps, mlog_context_t* cxt)
{
#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
  assert(cxt->magic == CONTEXT_MAGIC);
  assert(ps->magic == PARSE_SPEC_MAGIC);
#endif
  /* printf("%p: ", cxt);
   * _print_pspec(ps); */
  int i;
  const mlog_context_t* cc = cxt;
  for ( i = ps->name_array_length - 1; i > -1 && cc != NULL; --i )
    {
      if ( strcmp(cc->name, ps->name_array[i]) )
	return 0;
      cc = cc->parent;
    }
  cxt->flags = (cxt->flags & ~(ps->mask)) | (ps->flags & ps->mask);
  /* if ( ps->flags & MLOG_CONTEXT_EXPLICIT_STATE )
   *   mlog_context_enable(cxt);
   * else
   *   mlog_context_disable(cxt); */

  return 1;
}

static int
_fe_apply_pspecs(dllist_t* node, const void* udata)
{
  struct parse_spec* ps = (struct parse_spec*) node->data;
  mlog_context_t* cxt = (mlog_context_t*) udata;
  if ( _apply_pspec(ps, cxt) )
    {
      free(ps);
      parse_spec_list = dllist_remove_node(parse_spec_list, node);
      return 0;
    }
  else
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
#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
  o->magic = CONTEXT_MAGIC;
#endif
  o->name = strdup(name);
  o->description = strdup(description);

  if ( parent )
    {
      o->parent = parent;
      parent->children = dllist_append(parent->children, o);
      mlog_context_reset(o);
    }

  o->full_name = context_build_full_name(o);

  if ( !o->full_name )
    {
      free(o->name);
      free(o->description);
      free(o);
      return NULL;
    }

  if ( parse_spec_list )
    dllist_foreach(parse_spec_list, &_fe_apply_pspecs, o);

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
      if ( mlog_context_state(context->parent->flags) )
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



#define assign_and_advance(dest,type,size,source,size_counter)	\
  dest = (type *) source;					\
  source += (ptrdiff_t) size;				\
  size_counter -= size

static struct parse_spec*
_parse_single_spec(const char* _spec, const size_t _length)
{
  if ( _length < 2 || ( *_spec != '+' && *_spec != '-' ) )
    return NULL;

  size_t num_elems = 1;

  /* Count the number of name elements. */
  {
    const char* search = _spec + 1;
    while ( (search = strchr(search, '.')) != NULL )
      {
	num_elems++;
	search++;
      }
  }

  ssize_t n
    = sizeof(struct parse_spec)
    + (num_elems * sizeof(char*))
    + ( _length + 1 );

  char* buf = (char*) malloc(n);
  memset(buf, 0, n);

  struct parse_spec* ps = NULL;
  assign_and_advance(ps, struct parse_spec, sizeof(struct parse_spec), buf, n);
  assign_and_advance(ps->name_array, char*, num_elems * sizeof(char*), buf, n);
  assign_and_advance(ps->input, char, _length + 1, buf, n);
  memcpy(ps->input, _spec, _length + 1);
#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
  /* should have used up all the allocated memory  */
  assert(n == 0);
  ps->magic = PARSE_SPEC_MAGIC;
#endif
  ps->name_array_length = num_elems;

  /* Loop through again and grab name element strings  */
  unsigned int i = 0;
  char* search = ps->input + 1;
  while ( search < ps->input + _length )
    {
      buf = search;
      search = strchrnul(buf, '.');
      *search = '\0';
      ps->name_array[i] = buf;
      i++, search++;
    }

  if ( *(ps->input) == '+' )
    {
      ps->flags
	= MLOG_CONTEXT_POLICY	      /* explicit policy */
	| MLOG_CONTEXT_EXPLICIT_STATE /* set to enabled */;
      ps->mask = ps->flags;
    }
  else if ( *(ps->input) == '-' )
    {
      ps->flags = MLOG_CONTEXT_POLICY;
      ps->mask
	= MLOG_CONTEXT_POLICY	      /* explicit policy */
	| MLOG_CONTEXT_EXPLICIT_STATE /* copy the 'disabled' value */;
    }
  else
    abort();
  return ps;
}

int
mlog_context_parse_spec(const char* __ispec)
{
  if ( !__ispec )
    return -1;

  size_t count = 0;
  char* spec = strdup(__ispec);
  const size_t len = strlen(spec);
  if ( len < 2 )
    return 0;
  char* sp = spec;
  char* end = spec + len;
  char* sg_spec_end = NULL;
  struct parse_spec* pspec;

  while ( sp <= end )
    {
      /* Find the end of this single_spec. */
      sg_spec_end = strchr(sp, ',');
      if ( !sg_spec_end )
	sg_spec_end = end;

      *sg_spec_end = '\0';
      pspec = _parse_single_spec(sp, sg_spec_end - sp);
      if ( pspec )
	{
#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
	  assert(pspec->magic == PARSE_SPEC_MAGIC);
#endif
	  parse_spec_list = dllist_append(parse_spec_list, pspec);
	  count++;
	}
      sp = sg_spec_end + 1;
    }
  /* free(spec); */
  return count;
}

int
cmlog_real(const mlog_context_t* context, const unsigned long spec, const char* fmt, ...)
{
  va_list ap;
  mlog_loglevel_t lvl = LEVEL(spec);
  char* cfmt = NULL;
  size_t ncfmt = -1;
  int r = -1;

  if ( ! mlog_context_active(context) )
    return 0;

  if ( mlog_get_level() < lvl )
    return 0;
  const char* asfmt = CMLOG_FORMAT;
  if ( context->flags & MLOG_CONTEXT_HIDE_NAME )
    asfmt = CMLOG_FORMAT_NONAME;

  va_start(ap, fmt);

  ncfmt = asprintf(&cfmt, CMLOG_FORMAT, context->full_name, fmt);
  if ( ncfmt < 0 )
    return -1;


  r = mlogv(spec, cfmt, ap);
  free(cfmt);
  return r;
}

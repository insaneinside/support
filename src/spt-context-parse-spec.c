#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

#include <support/spt-context.h>
#include <support/dllist.h>

#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
#include <assert.h>
#endif

struct pspec_applydata
{
  dllist_t* pspec_list;
  spt_context_t* context;
};

/* Foreward declarations */
static uint8_t
_fe_context_apply_single_spec(spt_context_t* context, void* udata);

/* ****************************************************************
 * Utility functions
 */
/* _apply_pspec could be inline, but references static function _fe_context_apply_single_spec. */
static int
_apply_pspec(const spt_context_parse_spec_t* ps, spt_context_t* cxt)
{
#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
  assert(SPT_IS_CONTEXT(cxt));
  assert(SPT_IS_CONTEXT_PARSE_SPEC(ps));
#endif
  uint8_t apply_here = 1;
  int i;
  const spt_context_t* cc = cxt;
  for ( i = (signed) ps->name_array_length - 1; i > -1 && cc != NULL; --i, cc = cc->parent )
    {
      if ( strcmp(cc->name, ps->name_array[i]) )
	{
	  /* fprintf(stderr, "not applying pspec %p to context %s\n", ps, cc->full_name); */
	  apply_here = 0;
	  break;
	}
      /* fprintf(stderr, "pspec %p matched \"%s\" on \"%s\"\n", ps, cc->name, ps->name_array[i]); */
    }
  if ( apply_here )
    {
      /* fprintf(stderr, "applying pspec %p to context %s\n", ps, cxt->full_name); */
      cxt->flags = (cxt->flags & ~(ps->mask)) | (ps->flags & ps->mask);
      if ( ps->flags & SPT_CONTEXT_EXPLICIT_STATE )
      	spt_context_enable(cxt);
      else
      	spt_context_disable(cxt);
    }
  spt_context_each_child(cxt, &_fe_context_apply_single_spec, (void*) ps);

  return 1;
}

#define assign_and_advance(dest,type,size,source,size_counter)	\
  dest = (type *) source;					\
  source += (ptrdiff_t) size;					\
  size_counter -= size

__attribute__ (( __always_inline__ ))
__inline__ spt_context_parse_spec_t*
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

  size_t n
    = sizeof(spt_context_parse_spec_t)
    + (num_elems * sizeof(char*))
    + ( _length + 1 );

  char* buf = (char*) malloc(n);
  memset(buf, 0, n);

  spt_context_parse_spec_t* ps = NULL;
  assign_and_advance(ps, spt_context_parse_spec_t, sizeof(spt_context_parse_spec_t), buf, n);
  assign_and_advance(ps->name_array, char*, (size_t) ( num_elems * sizeof(char*) ), buf, n);
  assign_and_advance(ps->input, char, _length + 1, buf, n);
  memcpy(ps->input, _spec, _length + 1);
#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
  /* should have used up all the allocated memory  */
  assert(n == 0);
  ps->magic = SPT_CONTEXT_PARSE_SPEC_MAGIC;
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
	= SPT_CONTEXT_POLICY	      /* explicit policy */
	| SPT_CONTEXT_EXPLICIT_STATE /* set to enabled */;
      ps->mask = ps->flags;
    }
  else if ( *(ps->input) == '-' )
    {
      ps->flags = SPT_CONTEXT_POLICY;
      ps->mask
	= SPT_CONTEXT_POLICY	      /* explicit policy */
	| SPT_CONTEXT_EXPLICIT_STATE /* copy the 'disabled' value */;
    }
  else
    abort();
  return ps;
}

/* ****************************************************************
 * Callbacks
 */
static int
_fe_destroy_parse_spec(dllist_t* node, const void* udata __attribute__(( unused )) )
{
  free(node->data);
  return 1;
}

static uint8_t
_fe_context_apply_single_spec(spt_context_t* context, void* udata)
{
  spt_context_parse_spec_t* ps = (spt_context_parse_spec_t*) udata;
  _apply_pspec(ps, context);
  return 1;
}


static int
_fe_apply_pspecs(dllist_t* node, const void* udata)
{
  spt_context_parse_spec_t* ps = (spt_context_parse_spec_t*) node->data;
  struct pspec_applydata* pad = (struct pspec_applydata*) udata;
  spt_context_t* cxt = pad->context;

  if ( _apply_pspec(ps, cxt) )
    {
      pad->pspec_list = dllist_remove_node(pad->pspec_list, node);
    }

  return 1;
}

/* ****************************************************************
 * Public API
 */

void
spt_context_apply_parse_specs(spt_context_t* context,
				 dllist_t* pspec_list)
{
  dllist_t* use_list = dllist_copy(pspec_list);
  struct pspec_applydata pad;
  pad.pspec_list = use_list;
  pad.context = context;

  dllist_foreach(use_list, &_fe_apply_pspecs, &pad);

  if ( pad.pspec_list )
    dllist_free(pad.pspec_list);
}

dllist_t*
spt_context_parse_specs(const char* __ispec)
{
  if ( !__ispec )
    return NULL;

  size_t count = 0;
  char* spec = strdupa(__ispec);
  const size_t len = strlen(spec);
  if ( len < 2 )
    return NULL;
  char* sp = spec;
  char* end = spec + len;
  char* sg_spec_end = NULL;
  spt_context_parse_spec_t* pspec = NULL;
  dllist_t* out = NULL;

  while ( sp <= end )
    {
      /* Find the end of this single_spec. */
      sg_spec_end = strchr(sp, ',');
      if ( !sg_spec_end )
	sg_spec_end = end;

      *sg_spec_end = '\0';
      pspec = _parse_single_spec(sp, (size_t) (sg_spec_end - sp) );
      if ( pspec )
	{
#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
	  assert(SPT_IS_CONTEXT_PARSE_SPEC(pspec));
#endif
	  out = dllist_append(out, pspec);
	  count++;
	}
      sp = sg_spec_end + 1;
    }

  return out;
}

void
spt_context_parse_spec_destroy(spt_context_parse_spec_t* pspec)
{
  free(pspec);
}


void
spt_context_parse_spec_destroy_list(dllist_t* pspec_list)
{
  dllist_foreach(pspec_list, &_fe_destroy_parse_spec, NULL);
  dllist_free(pspec_list);
}


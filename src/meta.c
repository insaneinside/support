#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
/* #include <setjmp.h> */
/* #include <obstack.h> */

#include <support/meta.h>
#include <support/mlog.h>

#ifndef NULL
#define NULL ((void*) 0)
#endif

#if 0
  typedef union
  {
    void* ptr;
    short int sint;
    unsigned short int usint;
    long int lint;
    unsigned long int ulint;
    float flt;
    double dbl;
    long double ldbl;
    integer_t lt_int;
    uinteger_t lt_uint;
    /*     scalar_t lt_scalar; */
    string_t lt_string;
    /*     meta_type_t* custom; */
  } meta_union_t;
#endif	/* 0 */

static void*
mdcf_duplicate(meta_type_t* c, void* src)
{
  void* o = malloc(c->info->size);
  memcpy(o, src, c->info->size);
  return o;
}

static void*
mdcf_copy(meta_type_t* c, void* dest, void* src)
{  
  assert(c != NULL);
  assert(c->info != NULL);
  assert(dest != NULL);
  assert(src != NULL);

  memcpy(dest, src, c->info->size);

  return dest;
}

static void*
mdcf_create(meta_type_t* c)
{
  return memset(malloc(c->info->size), 0, c->info->size);
}

static void
mdcf_destroy(meta_type_t* c, void* p)
{
  free(p);
}

meta_type_funcs_t
default_custom_funcs =
  {
    .duplicate	= &mdcf_duplicate,
    .copy	= &mdcf_copy,
    .create	= &mdcf_create,
    .destroy	= &mdcf_destroy
  };

static meta_value_type_t
next_type_id = VALUE_TYPE_MAX_INDEX + 1;

static meta_type_t* custom_types[BUFSIZ];
static unsigned int num_custom = 0;

static
meta_type_info_t builtin_type_infos[] =
  {
    /* VALUE_TYPE_NONE */
    {
      .size = 0,
      .passby = PASS_BY_VALUE,
      .name = "None",
      .sdesc = "Empty type",
      .ldesc = "Empty type, for use in placeholders."
    },

    /* VALUE_TYPE_PTR */
    {
      .size = sizeof(void*),
      .passby = PASS_BY_VALUE,
      .name = "Pointer",
      .sdesc = "Generic pointer",
      .ldesc = "One hell of an inefficient way to store a generic pointer."
    },

    /* VALUE_TYPE_SHORT_INT */
    {
      .size = sizeof(short int),
      .passby = PASS_BY_VALUE,
      .name = "Short integer",
      .sdesc = NULL,
      .ldesc = NULL
    },

    /* VALUE_TYPE_UNSIGNED_SHORT_INT */
    {
      .size = sizeof(unsigned short int),
      .passby = PASS_BY_VALUE,
      .name = "Unsigned short integer",
      .sdesc = NULL,
      .ldesc = NULL
    },

    /* VALUE_TYPE_LONG_INT */
    {
      .size = sizeof(long int),
      .passby = PASS_BY_VALUE,
      .name = "Long integer",
      .sdesc = NULL,
      .ldesc = NULL
    },

    /* VALUE_TYPE_UNSIGNED_LONG_INT */
    {
      .size = sizeof(unsigned long int),
      .passby = PASS_BY_VALUE,
      .name = "Unsigned long integer",
      .sdesc = NULL,
      .ldesc = NULL
    },

    /* VALUE_TYPE_FLOAT */
    {
      .size = sizeof(float),
      .passby = PASS_BY_VALUE,
      .name = "Float",
      .sdesc = NULL,
      .ldesc = NULL
    },

    {
      .size = sizeof(double),
      .passby = PASS_BY_VALUE,
      .name = "Double",
      .sdesc = NULL,
      .ldesc = NULL
    },

    {
      .size = sizeof(long double),
      .passby = PASS_BY_VALUE,
      .name = "Long double",
      .sdesc = NULL,
      .ldesc = NULL
    }
  };

static
meta_type_t builtin_types[] =
  {
    {
      .type_index = VALUE_TYPE_NONE,
      .info = &(builtin_type_infos[VALUE_TYPE_NONE]),
      .funcs = &default_custom_funcs
    },
    {
      .type_index = VALUE_TYPE_PTR,
      .info = &(builtin_type_infos[VALUE_TYPE_PTR]),
      .funcs = &default_custom_funcs
    },
    {
      .type_index = VALUE_TYPE_SHORT_INT,
      .info = &(builtin_type_infos[VALUE_TYPE_SHORT_INT]),
      .funcs = &default_custom_funcs
    },
    {
      .type_index = VALUE_TYPE_UNSIGNED_SHORT_INT,
      .info = &(builtin_type_infos[VALUE_TYPE_UNSIGNED_SHORT_INT]),
      .funcs = &default_custom_funcs
    },
    {
      .type_index = VALUE_TYPE_LONG_INT,
      .info = &(builtin_type_infos[VALUE_TYPE_LONG_INT]),
      .funcs = &default_custom_funcs
    },
    {
      .type_index = VALUE_TYPE_UNSIGNED_LONG_INT,
      .info = &(builtin_type_infos[VALUE_TYPE_UNSIGNED_LONG_INT]),
      .funcs = &default_custom_funcs
    },
    {
      .type_index = VALUE_TYPE_FLOAT,
      .info = &(builtin_type_infos[VALUE_TYPE_FLOAT]),
      .funcs = &default_custom_funcs
    },
    {
      .type_index = VALUE_TYPE_DOUBLE,
      .info = &(builtin_type_infos[VALUE_TYPE_DOUBLE]),
      .funcs = &default_custom_funcs
    },
    {
      .type_index = VALUE_TYPE_LONG_DOUBLE,
      .info = &(builtin_type_infos[VALUE_TYPE_LONG_DOUBLE]),
      .funcs = &default_custom_funcs
    }
  };

static size_t type_sizes[] =
  {
    0,				/* NONE */
    sizeof(short int),
    sizeof(unsigned short int),
    sizeof(long int),
    sizeof(unsigned long int),
    sizeof(float),
    sizeof(double),
    sizeof(long double),
    
    sizeof(integer_t),
    sizeof(uinteger_t),
/*     sizeof(scalar_t), */
    sizeof(string_t),
    0,
  };

/** Given an absolute type index, map it to a custom type index.
 */
inline unsigned int
meta_get_custom_index(meta_value_type_t type_index)
{
  return type_index - VALUE_TYPE_MAX_INDEX - 1;
}

meta_type_t*
meta_find_type_by_index(meta_value_type_t index)
{
  if ( meta_type_is_custom(index) > 0 )
    return custom_types[meta_get_custom_index(index)];
  else
    return &(builtin_types[index]);
}

inline size_t
meta_value_type_size(meta_value_type_t tp)
{
  if ( tp >= next_type_id )
    {
      mlog(V_FATAL | F_MODNAME, "no such type (0x%X) registered, application developer error.", __func__);
      abort();
    }
  if ( tp <= VALUE_TYPE_MAX_INDEX )
    return type_sizes[tp];
  else
    return custom_types[meta_get_custom_index(tp)]->info->size;
}


meta_value_type_t
meta_register_type(meta_type_t* ct)
{
  custom_types[num_custom++] = ct;
  ct->type_index = next_type_id++;
  assert(next_type_id == num_custom + VALUE_TYPE_MAX_INDEX + 1);
  mlog(V_DEBUG, "Registered type 0x%02x, \"%s\"", ct->type_index, ct->info->name);

  return ct->type_index;
}

int
meta_type_is_custom(meta_value_type_t tp)
{
  return tp > VALUE_TYPE_MAX_INDEX ? ( tp < next_type_id  ? tp : -1) : 0;
}

meta_t*
meta_new(meta_value_type_t tp, ...)
{
  meta_t* o = NULL;
  va_list ap;

  if ( meta_type_is_custom(tp) )
    mlog(V_DEBUG, "meta_new called for custom type 0x%02x", tp);

  if ( (o = (meta_t*) malloc(sizeof(meta_t)) ) == NULL )
    {
      mlog(V_ERR, "failed to allocate memory for new meta_t");
      return NULL;
    }
    
  o->name = NULL;
  o->type = meta_find_type_by_index(tp);
  o->refcount = 0;
  o->allocated_size = 0;
  o->valptr = NULL;

  va_start(ap, tp);
  if ( ! meta_vset(o, tp, ap) )
    {
      free(o);
      o = NULL;
    }
  va_end(ap);

  return o;
}

meta_t*
meta_set(meta_t* meta, meta_value_type_t tp, ...)
{
  va_list ap;
  va_start(ap, tp);
  if ( ! meta_vset(meta, tp, ap) )
    {
      va_end(ap);
      return NULL;
    }
  va_end(ap);
  return meta;    
}


int
meta_allocate_as_needed(meta_t* meta)
{
  if ( meta->allocated_size != meta->type->info->size )
    {
      if ( !realloc(&(meta->valptr), meta->type->info->size  ) )
	{
	  mlog(V_ERR | F_ERRNO, "failed to allocate memory for meta_t value data");
	  return 0;
	}
      else
	meta->allocated_size = meta->type->info->size;
    }

  return 1;
}

meta_t*
meta_vset(meta_t* meta, meta_value_type_t tp, va_list ap)
{
  meta_type_t* type = NULL;
  void* p = NULL;

  /* Make sure the type exists */
  if ( ( type = meta_find_type_by_index(tp) ) == NULL )
    {
      mlog(V_ERR, "requested type 0x%02x not found.", tp);
      return NULL;
    }

  /* Set the type. */
  meta->type = type;

  /* Create a new instance of the type using the user-supplied function */
  meta->valptr = type->funcs->create(type);
  p = va_arg(ap, void*);

  /* Copy the value. */
  switch ( type->info->passby )
    {
    case PASS_BY_VALUE:
      /** \warning: CRUDE HACK  */
      type->funcs->copy(type, meta->valptr, &p);
      break;
    case PASS_BY_POINTER:
      type->funcs->copy(type, meta->valptr, p);
      break;

    default:
      abort();
      break;
    }


  return meta;
}

int
meta_get_refcount(meta_t* m)
{
  return m->refcount;
}

const char*
meta_get_name(meta_t* m)
{
  return (const char*) m->name;
}

int
meta_set_name(meta_t* m, const char* n)
{
  if ( m->name )
    free(m->name);

  m->name = strdup(n);
  return 0;
}

int
meta_reserve(meta_t* m)
{
  m->refcount++;
  return m->refcount;
}

int
meta_release(meta_t* m)
{
  assert(m != NULL);
  unsigned int ctp;
  meta_type_funcs_t* uf = NULL;

  if ( m->refcount > 1 )
    {
      m->refcount--;
      return m->refcount;
    }
  else
    {
      if ( m->name )
	free(m->name);
      if ( ( ctp = meta_type_is_custom(m->type->type_index) ) > 0 )
	{
	  ctp = meta_get_custom_index(m->type->type_index);
	  if ( custom_types[ctp]->funcs
	       && custom_types[ctp]->funcs->copy )
	    uf = custom_types[ctp]->funcs;
	  else
	    uf = &default_custom_funcs;
	  
	  uf->destroy(custom_types[ctp], m->valptr);
	}
      else
	{
	  if ( m->valptr )
	    free(m->valptr);
	}

      free(m);
      return 0;
    }
}

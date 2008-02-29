#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <alloca.h>
#include <math.h>
#include <unistd.h>
#include <mcheck.h>

#include <support/meta.h>
#include <support/mlog.h>
#include <support/vector.h>

#define CUSTOM_TYPE svec_t
#define CUSTOM_TYPE_NAME __STRING(svec_t)
#define CUSTOM_INITIAL_VALUE SV_NEW(1, 2, 3)

static scalar_t*
svec_duplicate(meta_type_t* c, scalar_t* src)
{
  scalar_t* o = (scalar_t*) malloc(sizeof(svec_t));
  SV_COPY((o), (src));
  return o;
}

static scalar_t*
svec_copy(meta_type_t* c, scalar_t* dest, scalar_t* src)
{
  SV_COPY((dest), (src));
  return dest;
}

static scalar_t*
svec_create(meta_type_t* c)
{
  scalar_t* o = (scalar_t*) malloc(sizeof(svec_t));
  SV_SET((o), 0, 0, 0);
  return o;
}

static void
svec_destroy(meta_type_t* c, scalar_t* s)
{
  free(s);
  return;
}

/* static */
/* meta_custom_funcs_t svec_funcs = */
/*   { */
/*     .duplicate		= &svec_duplicate, */
/*     .copy		= svec_copy, */
/*     .create		= svec_create, */
/*     .destroy		= svec_destroy */
/*   }; */

static
meta_type_funcs_t svec_funcs =
  {
    .duplicate	= (meta_func_duplicate_t) &svec_duplicate,
    .copy	= (meta_func_copy_t) &svec_copy,
    .create	= (meta_func_create_t) &svec_create,
    .destroy	= (meta_func_destroy_t) &svec_destroy
  };

static
meta_type_info_t svec_info =
  {
    .size	= sizeof(svec_t),
    .passby	= PASS_BY_POINTER,
    .name	= "svec_t",
    .sdesc	= "simple vector",
    .ldesc	= "A three-element scalar array."
  };

static
meta_type_t svec_custom =
  {
    .info = &svec_info,
    .funcs = &svec_funcs
  };

int
main(int argc, char** argv)
{
  mtrace();
  meta_t* m = NULL;
  meta_value_type_t ctype;	/* custom type id */
  CUSTOM_TYPE co = CUSTOM_INITIAL_VALUE;

  mlog_set_level(V_DEBUG);


  if ( argc > 1 )
    {
      m = meta_new(VALUE_TYPE_PTR, (string_t) strdup(argv[1]));

      fprintf(stderr, "metavariable value: \"%s\"\n",
	      meta_value(m, string_t));

      meta_release(m);
    }
  
  /* m = meta_new(VALUE_TYPE_INTEGER, 66);
   * fprintf(stderr, "metavariable value: %ld\n",
   * 	  meta_value(m, integer_t)); 
  meta_release(m);*/
  m = meta_new(VALUE_TYPE_FLOAT, M_PI);
  assert(S_EQ(meta_value(m, float),M_PI));
  fprintf(stderr, "metavariable value: %f\n",
	  meta_value(m, float));
  
  m = meta_new(VALUE_TYPE_LONG_DOUBLE, M_PIl);
  assert(S_EQ(meta_value(m, double),M_PIl));
  fprintf(stderr, "metavariable value: %.32f\n",
	  meta_value(m, double));
  meta_release(m);


  fprintf(stderr, "testing meta_register_type for type %s...\n", CUSTOM_TYPE_NAME);
  ctype = meta_register_type(&svec_custom);
  assert(ctype > 0);

  m = meta_new(ctype, co);

  fprintf(stderr, "metavariable value: %s\n",
	  SV_STR(meta_value(m, svec_t)));

  assert(meta_type_is_custom(meta_typeindex(m)) > 0);

  meta_release(m);
  
  muntrace();

  return 0;
}

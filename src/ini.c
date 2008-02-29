#include <malloc.h>
#include <support/ini.h>

/******************************************************************/
/* Local structs and functions                                    */

struct __ini
{
  dllist_t* sections;
};


struct __ini_section
{
  char* name;
  dllist_t* values;
};

static int
_fe_destroy_section(dllist_t* node, const void* udata)
{
  ini_section_destroy((ini_section_t*) node->data);
  return 1;			/* don't stop */
}

static int
_fe_destroy_value(dllist_t* node, const void* udata)
{
  meta_release((meta_t*) node->data);
  return 1;			/* don't stop */
}

/******************************************************************/

ini_t*
ini_create()
{
  return (ini_t*) malloc(sizeof(ini_t));
}



ini_section_t*
ini_section_create()
{
  return (ini_section_t*) malloc(sizeof(ini_section_t));
}


void
ini_destroy(ini_t* i)
{
  dllist_foreach(i->sections, _fe_destroy_section, NULL);
  dllist_free(i->sections);
  free(i);
}

void
ini_section_destroy(ini_section_t* s)
{
  dllist_foreach(s->values, _fe_destroy_value, NULL);
  dllist_free(s->values);
  free(s);
}


/******************************************************************/

const char*
ini_section_get_label(const ini_section_t* s)
{
  return s->name;
}


/******************************************************************/

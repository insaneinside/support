//#define DEBUG_VLIST
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>

#include "vlist.h"
#include "macro.h"

#define UNSET "<unset>"

//#define PREALLOC 128

typedef struct
{
#ifdef DEBUG_VLIST
  char id[4];
#endif

  void* base;			/* should equal address of this struct.  */

  size_t elsz;			/* element size (bytes) */

  size_t allocd;		/* bytes allocated */

/*unsigned int u_els_allocd;*/ 	/* num. elements that a user should assume allocated */

  unsigned int els_allocd; 	/* number of elements actually allocated --
				 * should be allocd/elsz!
				 */

  void* lst;			/* list start */
} vlmanager_t;


inline
vlmanager_t*
vlm(void** l)
{
#ifdef DEBUG_VLIST
  assert(l != NULL);
#endif

#ifdef DEBUG_VLIST
  vlmanager_t* m;
  m = (vlmanager_t*) ( l - sizeof(vlmanager_t) );

  assert(m = m->base);
  assert(m->elsz == sizeof(void*));
  assert(m->els_allocd == ( m->allocd / m->elsz ));
  assert(!strcmp(m->id, "LST"));
  return m;
#else
  return (vlmanager_t*) ( l - sizeof(vlmanager_t) );
#endif
}


/** Returns the number of items in a vlist.
 *
 * @param list a vlist
 */
inline
size_t
vlist_size(void** l)
{
  assert(l != NULL);
  return vlm(l)->els_allocd;
}


inline
void**
vlist_resize(void** l, size_t newsz)
{
#ifdef DEBUG_VLIST
  puts(__PRETTY_FUNCTION__);
#endif

  assert(l != NULL);
  
  void** o;
  vlmanager_t* m;
  int elt, olt;

  m = vlm(l);

  if ( m ->els_allocd == newsz )
    return l;

  size_t oldsz = m->els_allocd;
  olt = m->allocd;
  elt = m->elsz * newsz;

  printf("realloc()... ");
  m = realloc(m, sizeof(vlmanager_t) + elt);
  puts("done.");

  if ( m )
    {
      /*m->u_els_allocd = sz;*/
      m->base = m;
      m->lst = (void**) m + sizeof(vlmanager_t);
      m->allocd = elt;
      m->els_allocd = newsz;
      if ( elt > olt )
	memset(m->lst + olt, 0, elt - olt - 1);

      return m->lst;
    }
  else
    abort();
}


/** Returns a pointer to an empty list.
 */
void**
vlist_new(/*size_t elsz, size_t prealloc*/)
{
  void** o;
  vlmanager_t* m;
  const size_t elsz = sizeof(void*);
  const size_t prealloc = 0;
  int elt = elsz * prealloc;
  m = (vlmanager_t*) malloc(sizeof(vlmanager_t) + elt);
  o = (void**) ( (void**) m + sizeof(vlmanager_t) );

#ifdef DEBUG_VLIST
  strcpy(m->id, "LST");
#endif

  m->base = m;
  m->elsz = elsz;
  m->els_allocd = prealloc;
  /*m->u_els_allocd = 0;*/
  m->allocd = elt;
  m->lst = o;

  memset(o, 0, elt);

  return o;
}

/** Frees the pointer array of a list
 *
 * @param list the array to be freed
 */
void
vlist_free (void** l)
{
  assert(l != NULL);
  free(vlm(l));
  //free(l);
}
  

/** Adds an additional element to the end of a list, but does not
 * initialize it.
 *
 * @param list a vector list
 *
 * @return the new start of the list
 */
inline
void**
vlist_add(void** l)
{
  return vlist_resize(l, vlist_size(l) + 1 );
}

/** Appends an item to a vector list
 *
 * @param list a vector list
 * @param ptr the data pointer to append
 */
void**
vlist_append (void** l, void* p)
{
  assert(l != NULL);
  size_t i = vlist_size(l);
  l = vlist_add(l);
  l = vlist_resize(l, i+1);
  l[i] = p;

  return l;
}


/*
 * Inserts "ptr" into the list at "pos", starting from zero. If
 * "pos" is negative, counts backwards from the end of the list
 */
void**
vlist_insert(void** list, void* ptr, int pos)
{
  int i;
  int sz = vlist_size(list);
  void** base;

  base = list - 1;

  if ( pos < 0 )
    pos = sz + pos;
  else if ( pos > sz )
    return list;

  for ( i = sz; i >= pos; i-- )
    list[i+1] = list[i];

  list[i] = ptr;

  return list;
}


#if 0
/*
 * Replaces the pointer at "pos" with "ptr".
 */
void
vlist_set(void** list, const void *ptr, int pos)
{
  int sz = vlist_size(list);

  if ( pos > (sz-1) )
    return ;

  if ( pos < 0 )
    pos = sz - 1;

  list[pos] = ptr;

  return ;
}
#endif

/*
 * Removes (writes over) the pointer at "pos". Does
 * NOT free() whatever memory it points to.
 */
void**
vlist_delete_pos(void** list, int pos)
{
  int i,sz;
  void **nl;

  sz=vlist_size(list);

  nl = realloc(list, (sizeof(void *) * sz) - sizeof(void *));
  if(!nl) {
    fprintf(stderr,"%s: realloc() failed.\n", __func__);
    return list;
  }
  else
    list = nl;

  for(i=pos;i<sz-1;i++)
    list[i]=list[i+1];

  list[sz-1]=NULL;

  return list;
}


/* Tries to find ptr in list, and removes ptr from list
 * if found.
 */
void**
vlist_delete (void** list, void *ptr)
{
  int i, sz;
  sz = vlist_size(list);
  for ( i = 0; i < sz; i++) {
    if (list[i] == ptr)
      return (vlist_delete_pos(list, i));
  }

  return list;
}


/*
 * Appends one list to the end of another.
 */
void**
vlist_append_list(void** dest, void** src)
{
  unsigned int i;
  size_t szs,szd;
  szs = vlist_size(src);
  szd = vlist_size(dest);
  //dest = vlist_resize(dest, szs+szd);
  //memcpy(dest + szd * sizeof(void**), src, szs * sizeof(void**) );
  for( i = 0; i < szs; i++)
    {
      dest = vlist_add(dest);
      dest[szd+i] = src[i];
      /*vlist_set(dest, src[i], -1);*/
      //dest = vlist_append(dest, vlist_get(src, i));
    }

  return dest;
}


#if 0
/*
 * Returns the pointer at "pos". Easily done manually, but the
 * point is to relieve the programmer of manually messing with
 * arrays.
 */
void**
vlist_get(void **list, int pos)
{
  int sz = vlist_size(list);

  if( pos < 0 )
    {
      return list[sz-pos];
    }
  else if( pos < sz )
    {
      return list[pos];
    }
  else
    {
      abort();
      return NULL;
    }
}
#endif

/** Executes func for each element of list, passing userdata as
 * the second argument
 *
 * @param list a vector list
 * @param func a function to execute on each element of list
 * @param userdata data to be passed to func
 */
void
vlist_foreach(void** list, vlist_foreachfunc func, const void *userdata)
{
  int i, sz;
  sz = vlist_size(list);
  for(i = 0; i < sz; i++)
    func(list[i], userdata);

  return;
}

void
vlist_foreach_reverse(void** list, vlist_foreachfunc func, const void *userdata)
{
  int i, sz;
  sz = vlist_size(list);
  for(i = sz - 1; i >= 0; i--)
    func(list[i], userdata);

  return;
}


void**
vlist_find(void** list, vlist_findfunc func, const void *userdata)
{
  void *ret;
  int i, sz;
  sz = vlist_size(list);

  ret = NULL;

  for(i = 0; i < sz; i++)
    if( (ret = func(list[i], userdata)) != NULL)
      break;

  return ret;
}

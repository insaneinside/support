#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "vlist_old.h"

#define UNSET "<unset>"

/** Returns a pointer to an empty list.
 */
void **vlist_new(void) {
  void **list;

  list = (void ** ) malloc(sizeof(void *));

  list[0]=NULL;

  return list;
}

/** Frees the pointer array of a list
 *
 * @param list the array to be freed
 */
void vlist_free ( void **list ) {
  free(list);
}
  


/** Returns the number of items in a vlist.
 *
 * @param list a vector list
 */
int vlist_size(void ** list) {
  if(!list)
    return 0;

  int i;
  for ( i = 0; list[i]; i++) ;

  return i;
}

/** Appends an item to a vector list
 *
 * @param list a vector list
 * @param ptr the data pointer to append
 */
void *vlist_append (void **list, void *ptr) {
  list = vlist_add(list);
  vlist_set(list, ptr, -1);
  return list;
}


/** Adds an additional element to the end of a list, but does not
 * initialize it.
 *
 * @param list a vector list
 *
 * @return the new start of the list
 */
void *vlist_add(void **list) {
  void **nl;
  int sz=vlist_size(list);

  nl=realloc(list,(sz+2)*sizeof(void *));
  if(!nl) {
    fprintf(stderr,"%s: realloc() failed\n", __func__);
    return list;
  }
  else
    list = nl;

  list[sz]=UNSET;
  list[sz+1]=NULL;

  return list;
}

/*
 * Inserts "ptr" into the list at "pos", starting from zero. If
 * "pos" is negative, counts backwards from the end of the list
 */
void *vlist_insert(void **list, void *ptr, int pos) {
  int i;
  int sz = vlist_size(list);
  void **nl;

  if(pos<0)
    pos = sz + pos;
  else if(pos>sz)
    return list;

  nl=realloc(list,(sizeof(void *) * sz)+sizeof(void *));
  if(!nl) {
    fprintf(stderr,"%s: realloc() failed\n",__func__);
    return list;
  }
  else
    list = nl;

  for(i=sz;i>=pos;i--)
    list[i+1]=list[i];

  list[i]=ptr;

  return list;
}


/*
 * Replaces the pointer at "pos" with "ptr".
 */
void vlist_set(void **list, void *ptr, int pos) {
  int sz = vlist_size(list);

  if(pos>(sz-1))
    return ;

  if(pos<0)
    pos=sz-1;

  list[pos]=ptr;

  return ;
}

/*
 * Removes (writes over) the pointer at "pos". Does
 * NOT free() whatever memory it points to.
 */
void *vlist_delete_pos(void **list, int pos) {
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
void *vlist_delete (void **list, void *ptr) {
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
void *vlist_append_list(void **dest, void **src) {
  int i, sz;
  
  sz = vlist_size(src);
  for(i=0; i < sz; i++) {
    dest = vlist_append(dest, vlist_get(src, i));
  }

  return dest;
}

/*
 * Returns the pointer at "pos". Easily done manually, but the
 * point is to relieve the programmer of manually messing with
 * arrays.
 */
void *vlist_get(void **list, int pos) {
  int sz = vlist_size(list);

  if(pos<0) {
    return list[sz+pos];
  }
  else if(pos < sz) {
      return list[pos];
  }
  else {
    return NULL;
  }
}


/** Executes func for each element of list, passing userdata as
 * the second argument
 *
 * @param list a vector list
 * @param func a function to execute on each element of list
 * @param userdata data to be passed to func
 */
void vlist_foreach(void **list, vlist_foreachfunc func, const void *userdata) {
  int i, sz;
  sz = vlist_size(list);
  for(i = 0; i < sz; i++)
    func(list[i], userdata);

  return;
}

void *vlist_find(void **list, vlist_findfunc func, const void *userdata) {
  void *ret;
  int i, sz;
  sz = vlist_size(list);

  ret = NULL;

  for(i = 0; i < sz; i++)
    if( (ret = func(list[i], userdata)) != NULL)
      break;

  return ret;
}

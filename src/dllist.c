#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <support/support-config.h>
#include <support/dllist.h>

/* local functions */
void
dllist_free_node(dllist_t* node)
{
  if ( node )
    {
#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
      node->magic = 0;
#endif
      free(node);
    }
}

static int
_fe_list_free(dllist_t* node, const void* userdata)
{
  dllist_free_node(node);
  return 1;
}

struct sortdata_t
{
  dllist_t** src;
  dllist_t** dest;
  dllist_cmpfunc* cmp;
};

static int
_fe_insert_sorted(dllist_t* node, const void* udata)
{
  struct sortdata_t* sd = (struct sortdata_t*) udata;
  *(sd->src) = dllist_unlink(*(sd->src), node);
  *(sd->dest) = dllist_insert_node_sorted(*(sd->dest), node, *(sd->cmp));
  return 1;
}


dllist_t*
dllist_alloc()
{
  dllist_t* out;
  out = (dllist_t* ) malloc(sizeof(dllist_t));
  if ( ! out )
    return NULL;

  memset(out, 0, sizeof(dllist_t));
#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
  out->magic = DLLIST_MAGIC;
#endif
  return out;
}

dllist_t*
dllist_node(void* data)
{
  dllist_t* out = NULL;;
  out = dllist_alloc();
  if ( out )
    out->data = data;
  return out;
}

dllist_t*
dllist_append(dllist_t* list, void* data)
{
  if ( DLLIST_IS_NODE(list) )
    {
      dllist_t* last = NULL;

      last = dllist_last(list);

      if ( last )
	{
	  last->next = dllist_node(data);
	  last->next->next = NULL;
	  last->next->prev = last;
	}
      else
	return NULL;
    }
  else
    {
      list = dllist_node(data);	/* new list */
    }


  return list;
}


dllist_t*
dllist_prepend(dllist_t* list, void* data)
{
  dllist_t* first;

  if ( DLLIST_IS_NODE(list) )
    {
      first = dllist_first(list);

      first->prev = dllist_node(data);
      first->prev->next = first;
      first->prev->prev = NULL;

      return first->prev;		/* new node is now start of list */
    }
  else
    return dllist_node(data);	/* new list */
}


dllist_t*
dllist_insert(dllist_t* list, void* data, int pos)
{
  return dllist_insert_node(list, dllist_node(data), pos);
}

dllist_t*
dllist_insert_node(dllist_t* list, dllist_t* node, int pos)
{
  dllist_t* pre;
  pre = dllist_at(list, pos-1);

  node->prev = pre;
  if ( pre )
    {				/* pre could be NULL, position -1 */
      node->next = pre->next;
      pre->next = node;
    }
  else				/* inserting at start of list */
    node->next = dllist_first(list);

  if ( node->next )	       /* could be inserting at end of list */
    node->next->prev = node;

  if ( !node->prev )
    return node;
  else
    return list;
}

dllist_t*
dllist_unlink(dllist_t* list, dllist_t* node)
{
  dllist_t* ns;
  if ( ( ns = dllist_first(list) ) == node )
    ns = node->next;

  if ( node->next )
    node->next->prev = node->prev;

  if ( node->prev )
    node->prev->next = node->next;

  node->next = NULL;
  node->prev = NULL;

  return ns;
}

dllist_t*
dllist_remove_node(dllist_t* list, dllist_t* node)
{
  list = dllist_unlink(list, node);
  free(node);
  return list;
}

dllist_t*
dllist_remove(dllist_t* list, const void* data)
{
  dllist_t* node = dllist_find(list, data);
  if ( DLLIST_IS_NODE(node) )
    return dllist_remove_node(list, node);
  else
    return list;
}



dllist_t*
dllist_find(dllist_t* list, const void* data)
{
  dllist_t* cnode;
  for ( cnode=dllist_first(list);
	cnode != NULL;
	cnode = cnode->next )
    {
      if ( cnode->data == data )
	return cnode;
    }

  return NULL;
}

dllist_t*
dllist_find_user(dllist_t* list, dllist_func f, const void* udata)
{
  dllist_t* node = list;

  do
    {
      if ( !f(node, udata) )
	return node;
      node = node->next;
    }
  while ( node );

  return NULL;
}

dllist_t*
dllist_at(dllist_t* list, int pos)
{
  dllist_t* node;
  int i;

  if ( pos < 0 )
    return NULL;

  for ( node = dllist_first(list), i = 0;
	node != NULL && i < pos;
	node = node->next, i++ )
    ;

  return node;
}


int
dllist_getpos(dllist_t* node)
{
  dllist_t* cur = node;
  int p = 0;

  while ( cur->prev )
    {
      cur = cur->prev;
      p++;
    }

  return p;
}


dllist_t*
dllist_first(dllist_t* node)
{
  if ( !DLLIST_IS_NODE(node) )
    return NULL;

  while ( node->prev )
    node = node->prev;

  return node;
}

dllist_t*
dllist_last(dllist_t* node)
{
  if ( !DLLIST_IS_NODE(node) )
    return NULL;

  while ( node && node->next )
    node = node->next;

  return node;
}

int
dllist_size(dllist_t* list)
{
  dllist_t* node;
  int i = 0;
  if ( !DLLIST_IS_NODE(list) )
    return 0;

  node = dllist_first(list);

  do
    {
      i++;
      node = node->next;
    }
  while ( node != NULL );

  return i;
}

void
dllist_free(dllist_t* list)
{
  if ( ! DLLIST_IS_NODE(list) )
    return ;

  dllist_foreach(list, &_fe_list_free, NULL);
}


void
dllist_foreach(dllist_t* list, dllist_func foreachfunc, const void* userdata)
{
  if ( ! DLLIST_IS_NODE(list) )
    return;

  dllist_t* node, *next;
  node = list;


  do
    {
      /* Save the pointer to the next node _before_ we call
       * the user function, in case it removes the current
       * node from the list.
       */
      next = dllist_next(node);

      if ( !foreachfunc(node, userdata) )
	break;

      node = next;
    }
  while ( DLLIST_IS_NODE(node) );

  return;
}

/** Inserts `node' between `a' and `a->prev'.  `a->prev' may be NULL, but both
 * `node' and `a' must point to valid dllist structures.
 *
 */
void
dllist_insert_before(dllist_t* node, dllist_t* a)
{
  node->next = a;
  node->prev = a->prev;
  a->prev = node;
  if ( node->prev )
    node->prev->next = node;
}

/** Inserts `node' between `a' and `a->next'.  `a->next' may be NULL,
 * but both `node' and `a' must point to valid dllist structures.
 *
 */
void
dllist_insert_after(dllist_t* node, dllist_t* a)
{
  node->prev = a;
  node->next = a->next;
  a->next = node;
  if ( node->next )
    node->next->prev = node;
}


dllist_t*
dllist_insert_sorted(dllist_t* list, void* data, dllist_cmpfunc cf)
{
  return dllist_insert_node_sorted(list, dllist_node(data), cf);
}

dllist_t*
dllist_insert_node_sorted(dllist_t* list, dllist_t* node, dllist_cmpfunc cf)
{
  dllist_t* cur;
  dllist_t* ln = NULL;
  int cr = 0, lr = 0;
  cur = list;

  if ( !DLLIST_IS_NODE(list) )
    return node;

  do
    {
      cr = cf(node->data, cur->data);
      if ( cr == 0 )
	{
	  dllist_insert_after(node, cur);
	  break;
	}
      else if ( cr < 0 )
	{
	  if ( ( lr > 0 && ln == cur->prev ) || cur->prev == NULL )
	    {
	      dllist_insert_before(node, cur);
	      break;
	    }
	  else
	    {
	      ln = cur;
	      lr = cr;
	      cur = cur->prev;
	    }
	}
      else if ( cr > 0 )
	{
	  if ( ( lr < 0 && ln == cur->next ) || cur->next == NULL )
	    {
	      dllist_insert_after(node, cur);
	      break;
	    }
	  else
	    {
	      ln = cur;
	      lr = cr;
	      cur = cur->next;
	    }
	}
    }
  while ( cur );

  return dllist_first(list);
}

dllist_t*
dllist_sort(dllist_t* list, dllist_cmpfunc cmp)
{
  dllist_t* out = NULL;
  struct sortdata_t sd;
  sd.src = &list;
  sd.dest = &out;
  sd.cmp = &cmp;

  dllist_foreach(list, _fe_insert_sorted, &sd);

  return out;
}

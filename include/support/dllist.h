#ifndef SUPPORT_DLLIST_H
#define SUPPORT_DLLIST_H

#include <support/support-config.h>
#include <stdint.h>

/** @defgroup dllist Double-linked lists
 *@{
 */


#ifdef __cplusplus
extern "C"
{
#endif

  /* typedef struct _dllist dllist_t;  */

  typedef struct _dllist
  {
#ifdef SPT_ENABLE_CONSISTENCY_CHECKS
    uint32_t magic;
#endif
    void *data;

    struct _dllist* next;
    struct _dllist* prev;
  } dllist_t;

#define dllist_next(node) ((*node).next)
#define dllist_prev(node) ((*node).prev)

#ifdef SPT_ENABLE_CONSISTENCY_CHECKS

#define DLLIST_MAGIC  ( ( 'D' << 24 ) + ( 'L' << 16 ) + ( 'S' << 8 ) + 'T' )
#define DLLIST_IS_NODE(n) \
  ( n && *((uint32_t*) n) == DLLIST_MAGIC )

#else

#define DLLIST_IS_NODE(n) (n)

#endif	/* SPT_ENABLE_CONSISTENCY_CHECKS */

  typedef int (*dllist_func) (dllist_t* , const void*);

  typedef int (*dllist_cmpfunc) (void* a, void* b);


  /******************************************************************/
  /** \name Node Creation
   */
  /*@{*/

  /** Create an empty, unlinked node.
   */
  dllist_t* dllist_alloc();


  /** Allocate a node and initialize its data field.
   */
  dllist_t* dllist_node(void *data);

  /*@} */
  /******************************************************************/


  /******************************************************************/
  /** \name Augmentation
   */
  /*@{*/

  /** append data to the end of list.
   */
  dllist_t* dllist_append(dllist_t* list, void* data);


  /** Append a node to the end of the list.
   */
  dllist_t* dllist_append_node(dllist_t* list, dllist_t* node);

  /** Append another list to the end of the list.
   */
  dllist_t* dllist_append_list(dllist_t* list, dllist_t* second);


  /** prepend data to the start of list.
   */
  dllist_t* dllist_prepend(dllist_t* list, void* data);


  dllist_t* dllist_insert(dllist_t* list, void* data, int pos);

  dllist_t* dllist_insert_node(dllist_t* list, dllist_t* node, int pos);

  dllist_t* dllist_insert_sorted(dllist_t* list, void* data, dllist_cmpfunc sortfunc);

  dllist_t* dllist_insert_node_sorted(dllist_t* list, dllist_t* node, dllist_cmpfunc sortfunc);
  /*@}*/
  /******************************************************************/


  /******************************************************************/
  /** \name Object Removal
   */
  /*@{*/
  /** Free all nodes in @c list. Does \em not free user-data.
   */
  void dllist_free(dllist_t* list);


  /** Unlink node from list, without leaving a gap.
   *
   * \param	list	the list
   *
   * \param	node	the node to be unlinked
   *
   * \returns	a pointer the the beginning of list
   *
   */
  dllist_t* dllist_unlink(dllist_t* list, dllist_t* node); /* unlinks the node */
  dllist_t* dllist_remove_node(dllist_t* list, dllist_t* node); /* unlinks and destroys node */

  /** Find and remove first node where node->data == data.
   */
  dllist_t* dllist_remove(dllist_t* list, const void* data);

  /*@}*/
  /******************************************************************/


  /******************************************************************/
  /** \name Search Functions
   */
  /*@{*/

  /** find and return node where node->data == data.  If not found,
   *      return NULL.
   */
  dllist_t*
  dllist_find(dllist_t* list, const void *data); /* find node with data */

  /** Call the supplied function on each item in the list.  When func
   *      returns 0, return the current item.  if never found, return
   *      NULL.
   */
  dllist_t*
  dllist_find_user(dllist_t* list, dllist_func func, const void* usrdata);

  /*@}*/
  /******************************************************************/

  dllist_t* dllist_at(dllist_t* list, int pos); /* retrieves node at position */

  int dllist_getpos(dllist_t* node); /* finds node position in its list */

  dllist_t* dllist_first(dllist_t* node); /* find first node in list */
  dllist_t* dllist_last(dllist_t* node); /* find last node in list */

  int dllist_size(dllist_t* list); /* returns number of nodes in list */

  /* calls foreachfunc on each list item; returns when foreachfunc
   * returns zero.
   */
  void dllist_foreach(dllist_t* list,
		      dllist_func foreachfunc,
		      const void *userdata);

  dllist_t* dllist_sort(dllist_t* list, dllist_cmpfunc sortfunc);

  /** Create a copy of an entire list (and data pointers, of course).
   */
  dllist_t* dllist_copy(dllist_t* src);
  
  /** Create a copy of a single list node.
   */
  dllist_t* dllist_node_copy(dllist_t* node);

#ifdef __cplusplus
}
#endif

/**@}*/
#endif /* SUPPORT_DLLIST_H */

#ifndef NTREE_H
#define NTREE_H

typedef struct __ntree
{
  void* data;

  int nchildren;
  struct __ntree* parent;
  struct __ntree** child;
} ntree_t;



/* as used with ntree_foreach */
typedef int (*ntree_func) (ntree_t*node, void *userdata);


/* for use with ntree_find */
typedef ntree_t* (*ntree_findfunc) (ntree_t*node, void *userdata);

/* This compare function is a bit different from most:
 * the expected return value is an index for the child[]
 * component of b. It is used to determine where to place
 * a node.
 */
typedef unsigned int (*ntree_cmpfunc) (ntree_t*a, ntree_t*b);


/* convenience allocation functions */
ntree_t*
ntree_alloc();

ntree_t*
ntree_node(void *data);

/* add a new node */
ntree_t*
ntree_add(ntree_t*tree, void *data, int nchildren, ntree_cmpfunc cmpfunc);

/* ntree_splice -- add an existing node to a tree
 */
ntree_t*
ntree_splice(ntree_t*tree, ntree_t*node, ntree_cmpfunc cmpfunc);


/* execute foreachfunc on each node of the tree
 * while its return value is non-zero */
int ntree_foreach(ntree_t*tree,
		  ntree_func foreachfunc,
		  void *userdata);



ntree_t*ntree_root(ntree_t*node);	/* find the topmost node */



/* search will continue as long as the find function returns NULL */

ntree_t*
ntree_find(ntree_t* tree, ntree_findfunc func, void *userdata);

ntree_t*
ntree_foreach_find(ntree_t*tree, /* usu. called by ntree_find */
			  ntree_findfunc func,
			  void *userdata);
 


ntree_t*
ntree_airlayer(ntree_t* tree, ntree_t* node); /* de-parent node */


/* free all metadata. user should use ntree_foreach first to free
 * memory used by the data.
 */
void
ntree_free(ntree_t* tree);

int
ntree_free_func(ntree_t* node, void *userdata);
#endif	/* NTREE_H */

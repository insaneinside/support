#include <malloc.h>
#include <assert.h>

#include "ntree.h"

ntree_t*
ntree_alloc()
{
  ntree_t *out;
  out = (ntree_t *) malloc(sizeof(ntree_t));

  out->data = NULL;
  out->parent = NULL;
  out->child = NULL;

  return out;
}

ntree_t*
ntree_node(void *data)
{
  ntree_t *out = ntree_alloc();

  out->data = data;

  return out;
}

ntree_t*
ntree_add(ntree_t *tree, void *data, int nchildren, ntree_cmpfunc cmpfunc)
{
  int i;
  ntree_t *node;

  node = ntree_node(data);
  node->nchildren = nchildren;


  node->child = (ntree_t **) malloc(sizeof(ntree_t *) * node->nchildren);
  for(i=0; i < node->nchildren; i++) {
    node->child[i] = NULL;
  }

  return (ntree_splice(tree, node, cmpfunc));
}


ntree_t *ntree_splice(ntree_t *tree, ntree_t *node, ntree_cmpfunc cmpfunc) {
  int i;
  ntree_t *cur;

  if(!tree)			/* this will become the root node */
    return node;

  assert(tree->nchildren == node->nchildren);


  cur = ntree_root(tree);

  /* now we actually add it to the tree */
  while(1) {
    i = cmpfunc(node, cur);
    assert(i < node->nchildren);

    if(cur->child[i]) {
      cur = cur->child[i];
      continue;
    } else {
      cur->child[i] = node;
      node->parent = cur;
      break;
    }
  }

  return tree;
}

ntree_t *ntree_root(ntree_t *node) {
  if(!node)
    return NULL;

  while(node->parent != NULL)
    node = node->parent;

  return node;
}


int ntree_foreach(ntree_t *node, ntree_func foreachfunc, void *userdata) {
  /* We run foreachfunc on the children first so that if
   * it frees the node and the child pointer array, we won't
   * have any problems.
   */
  int i;
  for(i=0; i < node->nchildren; i++)
    if(node->child[i])
      if(!ntree_foreach(node->child[i], foreachfunc, userdata))
	return 0;


  if(!foreachfunc(node, userdata))
    return 0;

  return 1;
}


ntree_t *ntree_foreach_find(ntree_t *tree, ntree_findfunc func, void *userdata) {
  int i;
  ntree_t *ret = NULL;
  if((ret = func(tree, userdata)) != NULL)
    return ret;

  for(i=0; i < tree->nchildren; i++)
    if(tree->child[i])
      if((ret = ntree_foreach(tree->child[i], func, userdata)) != NULL)
	return ret;


  return NULL;
}  
  

ntree_t *ntree_airlayer(ntree_t *tree, ntree_t *node) {
  int i;
  tree = ntree_root(tree);

  if(node == tree)
    return node;

  assert(node->parent != NULL);

  for(i=0; i < node->parent->nchildren; i++)
    if(node->parent->child[i] == node) {
      node->parent->child[i] = NULL;
      node->parent = NULL;
      break;
    }

  assert(node->parent == NULL);


  return tree;
}


ntree_t *ntree_find(ntree_t *tree, ntree_findfunc func, void *userdata) {
  tree = ntree_root(tree);
  return (ntree_foreach_find(tree, func, userdata));
}


int ntree_free_func(ntree_t *node, void *userdata) {
  free(node->child);
  free(node);

  return 1;
}


void ntree_free(ntree_t *tree) {
  tree = ntree_root(tree);

  ntree_foreach(tree, &ntree_free_func, NULL);
}

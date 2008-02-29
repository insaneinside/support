#include <malloc.h>
#include <assert.h>

#include <support/dllist.h>
#include <support/hash_table.h>

static int
_fe_find_value(dllist_t* node, const void* hashptr)
{
  hash_pair_t *pair;
  pair = node->data;

  if ( pair->hash == *((hash_t *) hashptr) )
    return 0;
  else
    return -1;
}

static int
_fe_find_pair(dllist_t* node, const void* hashptr)
{
  hash_pair_t* pair;
  pair = node->data;

  if ( pair->hash == *((hash_t *) hashptr) )
    return 0;
  else
    return -1;
}

static int
_fe_free_pair(dllist_t* node, const void* nil)
{
  free(node->data);
  return 0;
}

hash_table_t*
hash_table_new(hash_makehash_func_t hashfunc)
{
  hash_table_t *out;
  assert(hashfunc != NULL);
  out = (hash_table_t *) malloc(sizeof(hash_table_t));

  out->hashfunc = hashfunc;
  out->kvpairs = NULL;

  return out;
}


void*
hash_table_get_value(hash_table_t* table, void* key)
{
  hash_t hash;
  assert(table != NULL);
  hash = table->hashfunc(key);
  return ((hash_pair_t*) dllist_find_user(table->kvpairs, &_fe_find_value, &hash)->data)->value;
}


void*
hash_table_get_pair(hash_table_t *table, void *key)
{
  hash_t hash = table->hashfunc(key);
  return (hash_pair_t*) dllist_find_user(table->kvpairs, &_fe_find_pair, &hash)->data;
}
  

int
hash_table_add(hash_table_t *table, void *key, void *value)
{
  hash_pair_t *pair;
  hash_t hash = table->hashfunc(key);

  assert(table != NULL);
  assert(table->kvpairs != NULL);

  /* if the key is already in the table, overwrite the current value */
  if ( ( pair = hash_table_get_pair(table, key) ) != NULL )
    {
      pair->value = value;
      return HASH_VALUE_UPDATED;
      
    }
  else
    {			/* add it to the table */
      pair = (hash_pair_t *) malloc(sizeof(hash_pair_t));
      pair->value = value;
      pair->hash = hash;
      
      table->kvpairs = dllist_append(table->kvpairs, pair);
      return HASH_PAIR_ADDED;
    }
}
    
int
hash_table_del(hash_table_t *table, void *key)
{
  hash_pair_t *pair;

  assert(table != NULL);

  if ( ( pair = hash_table_get_pair(table, key) ) == NULL )
    return HASH_FAILURE;
  else
    {
      table->kvpairs = dllist_remove(table->kvpairs, pair);
      free(pair);

      return HASH_SUCCESS;
    }
}

void
hash_table_free(hash_table_t *table)
{
  if (!table)
    return ;

  dllist_foreach(table->kvpairs, &_fe_free_pair, NULL);
  free(table);
}

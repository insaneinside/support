#ifndef SUPPORT_HASH_TABLE_H
#define SUPPORT_HASH_TABLE_H

#include <limits.h>
#include <support/dllist.h>

#ifndef hash_t
typedef unsigned long int hash_t;
#endif

#define HASH_MAX ULONG_MAX


typedef enum
  {
    HASH_FAILURE = -1,
    HASH_SUCCESS = 0,
    HASH_VALUE_UPDATED,
    HASH_PAIR_ADDED,
    HASH_PAIR_REMOVED
  } hash_return_t;


/* take a key and return a hash unique to that key
 * this function is always required by a hash table.
 */
typedef hash_t (*hash_makehash_func_t) (void *key);


/* duplicate a key
 * this function is always required by a hash table.
 */
/*typedef void* (*hash_dupekey_func_t) (void *key);*/


typedef struct
{
  hash_t hash;

  /*void *key;*/
  void *value;
} hash_pair_t;


typedef struct
{
  hash_makehash_func_t hashfunc;
  /*hash_makekey_func_t datafunc;*/

  dllist_t* kvpairs;
} hash_table_t;


hash_table_t*
hash_table_new(hash_makehash_func_t hashfunc/*, hash_makedata_func_t datafunc*/);

void
hash_table_free(hash_table_t *table); /* free a table and all memory used by it */

void*
hash_table_get_value(hash_table_t *table, void *key); /* return value corresponding to key */

void*
hash_table_get_pair(hash_table_t *table, void *key); /* return kv_pair structure pointer corresponding to key */

hash_return_t
hash_table_add(hash_table_t *table, void *key, void *value);	/* add a key/value pair to the table */

hash_return_t
hash_table_del(hash_table_t *table, void *key); /* remove a key/value pair from the table */


#endif /* SUPPORT_HASH_TABLE_H */

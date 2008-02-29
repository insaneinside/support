#ifndef V_LIST_H
#define V_LIST_H

typedef void  (*vlist_foreachfunc) ( void *ptr, const void *userdata );
typedef void* (*vlist_findfunc)    ( void *ptr, const void *userdata );

void **	vlist_new	( void );
void	vlist_free	( void **list );

int	vlist_size	( void **list );

void *	vlist_append	( void **list, void *ptr );

void * 	vlist_add	( void **list );
void *	vlist_insert	( void **list, void *ptr, int pos );


void	vlist_set	( void **list, void *ptr, int pos );
void *	vlist_delete_pos( void **list, int pos );
void *	vlist_delete	( void **list, void *ptr );
void *	vlist_append_list( void **dest, void **src );
void *  vlist_get	( void **list, int pos );

void	vlist_foreach	( void **list, vlist_foreachfunc func, const void *userdata );
void *	vlist_find	( void **list, vlist_findfunc func, const void *userdata );


#endif /* V_LIST_H */

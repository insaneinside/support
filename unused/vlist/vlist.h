#ifndef V_LIST_H
#define V_LIST_H

#ifdef __cplusplus
extern "C"
{
#endif


typedef void  (*vlist_foreachfunc) ( void** ptr, const void *userdata );
typedef void* (*vlist_findfunc)    ( void** ptr, const void *userdata );

  /* BROKEN!
#define vlist_type(_Tp) _Tp**
#define vlt(_Tp) _Tp**

#define vlist_new(_Tp, pre) (vlt(_Tp)) _vlist_new(sizeof(_Tp), pre) */

#define vlist_cast(v) (void**) v

void**	vlist_new	( /*size_t elsz, size_t prealloc */);
void	vlist_free	( void** list );

size_t	vlist_size	( void** list );

inline void**	vlist_resize	( void** list, size_t size );


void**	vlist_append	( void** list, void *ptr );

void**	vlist_add	( void** list );
void**	vlist_insert	( void** list, void *ptr, int pos );


void	vlist_set	( void** list, const void *ptr, int pos );
void**	vlist_delete_pos( void** list, int pos );
void**	vlist_delete	( void** list, void *ptr );
void**	vlist_append_list( void** dest, void **src );
void**	vlist_get	( void** list, int pos );

void	vlist_foreach	( void** list, vlist_foreachfunc func, const void *userdata );
void	vlist_foreach_reverse	( void **list, vlist_foreachfunc func, const void *userdata );
void**	vlist_find	( void** list, vlist_findfunc func, const void *userdata );


#ifdef __cplusplus
}
#endif

#endif /* V_LIST_H */

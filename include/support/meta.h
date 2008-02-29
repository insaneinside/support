#ifndef SUPPORT_META_H
#define SUPPORT_META_H
/** \defgroup meta Metavariables */
/*@{*/


/* needed for the declaration of meta_vset */
#include <stdarg.h>

#include <support/logical-types.h>


#ifdef __cplusplus
extern "C"
{
#endif

  /** Enumeration of builtin types.
   */
  typedef enum
    {
      /* Language (C) types */
      VALUE_TYPE_NONE = 0,
      VALUE_TYPE_PTR,
      VALUE_TYPE_SHORT_INT,
      VALUE_TYPE_UNSIGNED_SHORT_INT,
      VALUE_TYPE_LONG_INT,
      VALUE_TYPE_UNSIGNED_LONG_INT,
      VALUE_TYPE_FLOAT,
      VALUE_TYPE_DOUBLE,
      VALUE_TYPE_LONG_DOUBLE,

      /* Logical types */
      /* VALUE_TYPE_INTEGER,
       * VALUE_TYPE_UNSIGNED_INTEGER,
       * /\*       VALUE_TYPE_SCALAR, *\/
       * VALUE_TYPE_STRING */
    } meta_value_type_t;

  /** Maximum value for a meta_value_type_t corresponding to a
   *  built-in type.
   */
#define VALUE_TYPE_MAX_INDEX VALUE_TYPE_LONG_DOUBLE


  /******************************************************************/
  /** \name Type interface
   */
  /*@{*/

  /** \internal
   */
  typedef struct __meta_type meta_type_t;

  /** \internal
   */
  typedef struct __meta_type_info meta_type_info_t;

  /** Type definition for custom object-replication functions.
   *
   * \param	custom	Pointer to the custom type of the object to be
   *			duplicated.
   * 
   */
  typedef	void* (*meta_func_duplicate_t)	( meta_type_t* type,
						  void* src );

  /** Type definition for custom object-xeroxication functions.
   */
  typedef	void* (*meta_func_copy_t)	( meta_type_t* type,
						  void* dest,
						  void* src );

  /** Type definition for custom object-creation functions.
   */
  typedef	void* (*meta_func_create_t)	( meta_type_t* type);

  /** Type definition for custom object-destruction functions.
   */
  typedef	void  (*meta_func_destroy_t)	( meta_type_t*, void* ptr );
  


  /** How the user should pass values to the \c meta_ interface functions when
   *  setting a meta.
   */
  typedef enum
    {
      PASS_BY_VALUE,
      PASS_BY_POINTER
    } meta_type_passby_t;

  /** Holds pointers to user-supplied functions for various operations
   *  on a certain type.  If a member is \c NULL, the corresponding
   *  builtin function will be used.
   */
  typedef struct
  {
    /** Duplicate an object.
     */
    meta_func_duplicate_t	duplicate;

    /** Copy the value of an object.
     */
    meta_func_copy_t		copy;

    /** Create a new, uninitialized object.
     */
    meta_func_create_t		create;

    /** Destroy an object.
     */
    meta_func_destroy_t		destroy;
  } meta_type_funcs_t;


  /** Structure containing information on the type interfaced by a \c
   *  meta_type_t.
   */
  struct __meta_type_info
  {
    /** Size, in bytes, of an instance of the underlying type.
     */
    size_t size;

    /** How the type should interfaced by the \c meta_ functions.
     */
    meta_type_passby_t passby;

    /** Name of the underlying object type.
     */
    string_t name;

    /** Short description of type.
     */
    string_t sdesc;

    /** Long description of type, or NULL.
     */
    string_t ldesc;
  };

  /** Container for a metavariable type definition.
   */
  struct __meta_type
  {
    /** \warning	meta_register_type will overwrite this
     * 			field; do not use it.
     */
    meta_value_type_t type_index;

    /** Pointer to a meta_type_info_t describing this type.
     */
    meta_type_info_t* info;

    /** Pointer to a meta_type_funcs_t for operations on this type.
     */
    meta_type_funcs_t* funcs;
  };


  /** Register a type.
   *
   * \param	tp	a pointer to the \c meta_custom_t to be registered.
   *
   * \returns	A \c meta_value_type_t corresponding to the new type.
   */
  meta_value_type_t
  meta_register_type(meta_type_t* tp);


  /** Determine if a metavalue type is custom.
   *
   * \param tp the \c meta_value_type_t to test
   *
   * \returns > 0 if \c tp is a custom type, 0 if it is not and < 0 if
   * errors were encountered.
   */
  int
  meta_type_is_custom(meta_value_type_t tp);

  /** Find a type definition object by its index.
   */
  meta_type_t*
  meta_find_type_by_index(meta_value_type_t index);

  /*@}*/
  /******************************************************************/

  /** \internal
   */
  typedef struct
  {
    /** Type definition.
     */
    meta_type_t* type;

    /** Optional identifier.
     */
    char* name;
    
    /** Pointer to memory allocated for the subvalue.
     */
    void* valptr;
    
    /** Size of the memory region pointed to by \c valptr.
     */
    size_t allocated_size;

    /** Number of external objects currently referencing this object. */
    unsigned int refcount;
  } meta_t;

  /** Create an empty metavariable.
   */
  meta_t* meta_create();

  /** Alias for meta_release.
   */
#define meta_destroy(m) meta_release(m)



  /** Create and initialize a metavariable.
   */
  meta_t* meta_new(meta_value_type_t, ...);


  /** Set a meta variadically.
   */
  meta_t* meta_set(meta_t* meta, meta_value_type_t tp, ...);


  /** \bug This function possibly misuses va_arg...
   */
  meta_t* meta_vset(meta_t* meta, meta_value_type_t tp, va_list ap);



  /** Mark a metavariable as in-use. Increments the internal reference
   *  count.
   */
  int meta_reserve(meta_t*);

  int meta_release(meta_t*);

  const char* meta_get_name(meta_t*);
  int meta_get_refcount(meta_t*);

  int meta_set_name(meta_t*, const char*);

#define meta_type(m) m->type
#define meta_typeindex(m) m->type->type_index

#ifndef __cplusplus
#define meta_value(m,tp) *((tp*) m->valptr)
#else
#define meta_value(m,tp) *(reinterpret_cast<tp*>(m->valptr))
}
#endif


/*@}*/
#endif /* SUPPORT_META_H */

/**@file
 *Reference-counted object class and supporting functions.
 *
 */
#ifndef RefCountedObject_hh
#define RefCountedObject_hh 1

#include <stdexcept>
#include <cstdint>

/** Base class with reference counter, for use with intrusive pointer
 *  types.  Functions for use with boost::intrusive_ptr are included.
 *
 * When @em storing any sort of reference to a heap-allocated,
 * <code>%RefCountedObject</code>-derived object, use
 * <code>boost::intrusive_ptr&lt;DerivedType&gt;</code>.  Note that the
 * intrusive pointer type is only necessary when storing, as opposed to using,
 * <code>%RefCountedObject</code>-derived objects.
 *
 * @sa intrusive_ptr_add_ref, intrusive_ptr_release
 */
struct RefCountedObject
{
  /** Constructor. Initializes the object's reference count to zero.
   */
  inline RefCountedObject()
    : refCount ( 0 )
  {
  }

  /** Destructor.
   *
   * @throws runtime_error if object's reference count is non-zero.
   */
  inline virtual ~RefCountedObject()
  {
    if ( refCount > 0 )
      throw std::runtime_error("In RefCountedObject::~RefCountedObject(): destructor called with refCount > 0");
    else if ( refCount < 0 )
      throw std::runtime_error("In RefCountedObject::~RefCountedObject(): destructor called with refCount < 0");
  }

  ssize_t refCount;
};

/** Increments the reference count of a RefCountedObject.
 *
 * @warning This function is provided for integration with boost::intrusive_ptr,
 * and is not intended to be called directly.
 */
inline void
intrusive_ptr_add_ref(RefCountedObject* __rco)
{
  /* assert(__rco != NULL);
   * assert(__rco->refCount >= 0); */
  __rco->refCount++;
}

/** Decrements the reference count of a RefCountedObject, and deletes the object
 * if its reference count is zero.
 *
 * @warning This function is provided for integration with boost::intrusive_ptr,
 * and is not intended to be called directly.
 */
inline void
intrusive_ptr_release(RefCountedObject* __rco)
{
  /* assert(__rco != NULL); */
  /* assert(__rco->refCount > 0); */ // <= not needed?
  __rco->refCount--;

  if ( __rco->refCount == 0 )
    delete __rco;
}

#include <boost/intrusive_ptr.hpp>

/** Quick-n-dirty alias for boost::intrusive_ptr, in lieu of GCC's eventual
 *  implementation of C++0x template aliases.
 *
 * @todo Using #defines for aliasing symbols isn't a very good idea in C++,
 * since it fscks up namespacing scoping just about everywhere.  Is there a
 * better way to fake this?
 */
#define Ref boost::intrusive_ptr

/* template < typename _T >
 * class Ref
 *   : public boost::intrusive_ptr<_T>
 * {
 * public:
 *   inline Ref(_T* __ptr)
 *     : boost::intrusive_ptr<_T>::intrusive_ptr(__ptr)
 *   {
 *   }
 *   
 * }; */

#endif	/* RefCountedObject_hh */

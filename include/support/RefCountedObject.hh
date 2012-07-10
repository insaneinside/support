/**@file
 * 
 * Reference-counted object class and supporting structures and
 * functions for use with boost::intrusive_ptr.
 *
 */
#ifndef support_RefCountedObject_hh
#define support_RefCountedObject_hh 1

#include <stdexcept>
#include <cstdint>


namespace spt
{
  /** Base class with reference counter, for use with intrusive pointer
   *  types.  Functions for use with boost::intrusive_ptr are included.
   *
   * When @em storing any sort of reference to a heap-allocated,
   * <code>%RefCountedObject</code>-derived object, use
   * <code>boost::intrusive_ptr&lt;DerivedType&gt;</code>.  Note that the
   * intrusive pointer type is only necessary when storing, as opposed to using,
   * <code>%RefCountedObject</code>-derived objects.
   *
   * @sa RefTraits, intrusive_ptr_add_ref, intrusive_ptr_release
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
    inline virtual ~RefCountedObject() throw ( std::runtime_error )
    {
      if ( refCount > 0 )
	throw std::runtime_error("In RefCountedObject::~RefCountedObject(): destructor called with refCount > 0");
      else if ( refCount < 0 )
	throw std::runtime_error("In RefCountedObject::~RefCountedObject(): destructor called with refCount < 0");
    }

    ssize_t refCount;
  };
}

/** Increments the reference count of a RefCountedObject.
 *
 * @warning This function is provided for integration with boost::intrusive_ptr,
 * and is not intended to be called directly.
 */
inline void
intrusive_ptr_add_ref(spt::RefCountedObject* __rco)
{
  __rco->refCount++;
}

/** Decrements the reference count of a RefCountedObject, and deletes the object
 * if its reference count is zero.
 *
 * @warning This function is provided for integration with boost::intrusive_ptr,
 * and is not intended to be called directly.
 */
inline void
intrusive_ptr_release(spt::RefCountedObject* __rco)
{
  __rco->refCount--;

  if ( __rco->refCount == 0 )
    delete __rco;
}

#include <boost/intrusive_ptr.hpp>


namespace spt
{
  /** Object-reference type-traits struct.  This is used to hide the
   * specific types used for referencing objects from the object
   * implementations.
   */
  template < typename _T >
  struct RefTraits
  {
    /** Type used when storing a reference to an object.  We use a
     *  tracked-pointer because stored references are not   */
    typedef boost::intrusive_ptr<_T> stored_ref;
    typedef const boost::intrusive_ptr<_T> const_stored_ref;

    /** Type used when passing or using -- but not storing -- node
     *  references.
     */
    typedef _T* passed_ref;
    typedef const _T* const_passed_ref;
  };
}

#endif	/* ! defined(support_RefCountedObject_hh) */

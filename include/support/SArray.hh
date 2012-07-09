#ifndef SArray_hh
#define SArray_hh 1

#include <cstddef>
#include <utility>

namespace spt
{
  /** Simple, statically-sized array with basic memory management
   *
   * @tparam _Tp Element type.
   */
  template < typename _Tp >
  struct SArray
  {
    typedef _Tp value_type;

    value_type* item;
    size_t size;


    static inline value_type*
    alloc(size_t count)
    {
      return static_cast<value_type*>(::operator new[](count * sizeof(_Tp)));
    }

    static inline void
    dealloc(value_type* ptr)
    {
      ::operator delete[](ptr);
    }

    inline SArray()
#if __cplusplus > 199711L || defined(__GXX_EXPERIMENTAL_CXX0X__)
      noexcept
#endif
      : item ( NULL ),
	size ( 0 )
    {
    }


    inline SArray(size_t s)
      : item ( s > 0 ? SArray::alloc(s) : NULL ),
	size ( s )
    {
    }


    inline ~SArray()
    {
      if ( item )
	{
	  dealloc(item);
	  item = NULL;
	}
    }


#if __cplusplus > 199711L || defined(__GXX_EXPERIMENTAL_CXX0X__)
    /** Move constructor. */
    inline SArray(SArray&& sa)
      : item ( std::move(sa.item) ),
	size ( std::move(sa.size) )
    {
      sa.item = NULL;
      sa.size = 0;
    }


    /**@name Deleted functions.
     *This class is non-copyable.
     *@{
     */
    SArray(const SArray&) = delete;
    SArray& operator=(const SArray&) = delete;
    /**@*/


    /** Rvalue assignment operator. */
    inline SArray&
    operator = (SArray&& sa)
    {
      item = std::move(sa.item);
      size = std::move(sa.size);

      sa.item = NULL;
      sa.size = 0;

      return *this;
    }
#endif

    inline _Tp&
    operator[](size_t n)
    {
      return item[n];
    }

    inline const _Tp&
    operator[](size_t n) const
    {
      return item[n];
    }
  };
}
#endif	/* defined(SArray_hh) */

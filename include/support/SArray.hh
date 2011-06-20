#ifndef SArray_hh
#define SArray_hh 1

#include <cstddef>
#include <utility>

namespace spt
{
  /** Simple, static array with basic memory management.
   */
  template < typename _Tp >
  struct SArray
  {
    _Tp* item;
    size_t size;

#if __cplusplus > 199711L || defined(__GXX_EXPERIMENTAL_CXX0X__)
    /**@name Deleted functions.
     *This class is non-copyable.
     *@{
     */
    SArray(const SArray&) = delete;
    SArray& operator=(const SArray&) = delete;
    /**@*/

    inline SArray()
      : item ( NULL ),
	size ( 0 )
    {
    }

    inline SArray(SArray&& sa)
      : item ( std::move(sa.item) ),
	size ( std::move(sa.size) )
    {
      sa.item = NULL;
      sa.size = 0;
    }
#endif

    inline SArray(SArray& sa)
      : item ( sa.item ),
	size ( sa.size )
    {
      sa.item = NULL;
      sa.size = 0;
    }


    inline SArray(size_t s)
      : item ( new _Tp[s] ),
	size ( s )
    {
    }

    inline ~SArray()
    {
      if ( item )
	delete[] item;
    }

#if __cplusplus > 199711L || defined(__GXX_EXPERIMENTAL_CXX0X__)
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

    inline SArray&
    operator = (SArray& sa)
    {
      item = std::move(sa.item);
      size = std::move(sa.size);

      sa.item = NULL;
      sa.size = 0;

      return *this;
    }


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

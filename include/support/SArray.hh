#ifndef SArray_hh
#define SArray_hh 1

#include <cstddef>
#include <utility>

template < typename _Tp >
struct SArray
{
  _Tp* item;
  size_t size;

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
      delete item;
  }

  inline SArray&
  operator = (SArray&& sa)
  {
    item = sa.item;
    size = sa.size;

    sa.item = NULL;
    sa.size = 0;

    return *this;
  }

  inline _Tp&
  operator[](size_t n)
  {
    return item[n];
  }

};

#endif	/* defined(SArray_hh) */

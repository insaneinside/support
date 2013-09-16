#ifndef SArray_hh
#define SArray_hh 1

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <initializer_list>

namespace spt
{
  /** Simple array type with basic memory management
   *
   * @tparam _Tp Element type.
   */
  template < typename _Tp >
  struct SArray
  {
    typedef _Tp ValueType;

    ValueType* data;
    size_t size;
    size_t capacity;
    bool owns_data;

    /* STL-style accessors */
    inline ValueType& front() { return *data; }
    inline ValueType& back() { return *(data + size); }
    inline ValueType* begin() { return data; }
    inline ValueType* end() { return data + size; }

    inline const ValueType& front() const { return *data; }
    inline const ValueType& back() const { return *(data + size); }
    inline const ValueType* begin() const { return data; }
    inline const ValueType* end() const { return data + size; }


    ValueType& emplace(const ValueType&& v)
    { return emplace(const_cast<ValueType&&>(v)); }

    ValueType& emplace(ValueType&& v)
    { ensure_capacity(size + 1);
      static_assert(std::is_rvalue_reference<ValueType&&>::value, "Emplace argument resolves to non-Rvalue reference!");
      new ( data + size++ ) ValueType(std::move(v));
      return *(data + size - 1);
    }

    inline void
    ensure_capacity(size_t reqd_capacity)
    { if ( capacity < reqd_capacity )
	{
	  size_t old_capacity ( capacity );
	  capacity += 2 * (reqd_capacity - capacity);
	  data = static_cast<ValueType*>(realloc(data, capacity * sizeof(ValueType)));
	  memset(data + old_capacity, 0, sizeof(ValueType) * (capacity - old_capacity));
	} }


    static inline ValueType*
    alloc(size_t count)
    { return (count > 0 ? static_cast<ValueType*>(malloc(count * sizeof(_Tp))) : nullptr); }


    static inline void
    dealloc(ValueType* ptr)
    { if ( ptr ) free(ptr); }


    inline void
    clear()
    {
      if ( data && owns_data )
	{
	  for ( size_t i = 0; i < size; ++i )
	    data[i].~ValueType();
	  size = 0;
	}
    }

    inline SArray() noexcept
      : data ( nullptr ),
      size ( 0 ),
      capacity ( 0 ),
      owns_data ( true )
    {}


    inline SArray(size_t s)
    : data ( s > 0 ? SArray::alloc(s) : nullptr ),
      size ( 0 ),
      capacity ( s ),
      owns_data ( true )
    {}


    inline ~SArray()
    {
      if ( data && owns_data )
	{
	  clear();
	  dealloc(data);
	  data = nullptr;
	}
    }

    /** Move constructor. */
    inline SArray(SArray&& sa)
    : data ( sa.data ),
      size ( sa.size ),
      capacity ( sa.capacity ),
      owns_data ( sa.owns_data )
    {
      sa.data = nullptr;
      sa.owns_data = false;
    }

    /** Constructor for creating an SArray that references static data. */
    inline SArray(const std::initializer_list<ValueType>& _data)
      : SArray()
    {
      ensure_capacity(_data.size());
      for ( const ValueType& f : _data )
	emplace(std::move(f));
    }


    SArray(const SArray& s)
    : data ( s.data ),
      size ( s.size ),
      capacity ( s.capacity ),
      owns_data ( false )
    {}

    SArray& operator=(const SArray& s)
    {
      data = s.data;
      size = s.size;
      capacity = s.capacity;
      owns_data = false;
      return *this;
    }


    /** Rvalue assignment operator. */
    inline SArray&
    operator = (SArray&& sa)
    {
      data = sa.data;
      size = sa.size;
      capacity = sa.capacity;
      owns_data = sa.owns_data;

      sa.data = nullptr;
      sa.owns_data = false;

      return *this;
    }


    inline _Tp&
    operator[](size_t n)
    { return data[n]; }

    inline const _Tp&
    operator[](size_t n) const
    { return data[n]; }
  };
}

#endif	/* defined(SArray_hh) */

#ifndef Vec_hh
#define Vec_hh 1

#include <type_traits>
#include <support/scalar.h>
#include <cstdarg>
#include <cstring>
#include <stdexcept>

namespace spt
{
  /** A templatized implementation of Vector
   */
  template < size_t _N, typename _ValueType = scalar_t >
  class Vec
  {
  public:
    typedef  _ValueType value_type;
    typedef size_t size_type;

    static const size_type N = _N;

    inline
    Vec()
      : _M_val { 0 }
#ifdef SPT_VECT_CACHE_MAGNITUDE
        ,_M_mag_cached(0), _M_recalc_mag(false),
	_M_mag2_cached(0), _M_recalc_mag2(false)
#endif
    {
    }

    inline
    Vec(const Vec& v)
      : _M_val ( )
#ifdef SPT_VECT_CACHE_MAGNITUDE
        ,_M_mag_cached(v._M_mag_cached), _M_recalc_mag(v._M_recalc_mag),
	_M_mag2_cached(v._M_mag2_cached), _M_recalc_mag2(v._M_recalc_mag2)
#endif
    {
      for ( size_type i ( 0 ); i < _N; ++i )
	_M_val[i] = v._M_val[i];
    }


    template < typename... _S>
    Vec(_S... values)
      : _M_val { static_cast<value_type>(values)... }
#ifdef SPT_VECT_CACHE_MAGNITUDE
        ,_M_mag_cached(0), _M_recalc_mag(true), _M_mag2_cached(0), _M_recalc_mag2(true)
#endif
    {
    }

    template < typename _Tp = int >
    Vec(std::initializer_list<_Tp> values)
      : _M_val ( )
#ifdef SPT_VECT_CACHE_MAGNITUDE
        ,_M_mag_cached(0), _M_recalc_mag(true), _M_mag2_cached(0), _M_recalc_mag2(true)
#endif
    {
      size_type i ( 0 );
      for ( const _Tp* v ( values.begin() ); v != values.end(); ++v, ++i )
	_M_val[i] = static_cast<value_type>(*v);
    }


    /** Destructor. */
    inline virtual
    ~Vec() {}

    Vec&
    operator =(const Vec& other)
    {
      for ( size_type i ( 0 ); i < _N; ++i )
    	_M_val[i] = other._M_val[i];

#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = other._M_recalc_mag;
      _M_mag_cached = other._M_mag_cached;
      _M_recalc_mag2 = other._M_recalc_mag2;
      _M_mag2_cached = other._M_mag2_cached;
#endif
      
      return *this;
    }

    Vec&
    operator =(const value_type s[_N])
    {
      for ( size_type i ( 0 ); i < _N; ++i )
	_M_val[i] = s[i];
#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
      _M_recalc_mag2 = true;
#endif
      return *this;
    }

    __attribute__(( noinline ))	/* Variadic, so it's (apparently) not inlinable. */
    void
    set(value_type first, ...)
    {
      _M_val[0] = first;
      va_list ap;
      va_start(ap, first);
      for ( size_type i ( 1 ); i < _N; ++i )
	_M_val[i] = static_cast<value_type>(va_arg(ap, double));

      va_end(ap);
#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
      _M_recalc_mag2 = true;
#endif
    }

    inline void
    set(value_type nv[_N])
    {
      for ( size_type i ( 0 ); i < _N; ++i )
	_M_val[i] = nv[i];
#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
      _M_recalc_mag2 = true;
#endif
    }

    inline void
    set(const Vec& r)
    {
      memcpy(_M_val, r._M_val, _N * sizeof(value_type));
#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
      _M_recalc_mag2 = true;
#endif
    }

    inline void
    clear()
    {
      for ( size_type i ( 0 ); i < _N; ++i )
	_M_val[i] = 0;

#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_mag_cached = 0;
      _M_mag2_cached = 0;
      _M_recalc_mag = false;
      _M_recalc_mag2 = false;
#endif
    }


    value_type
    mag2() const
    {
#ifdef SPT_VECT_CACHE_MAGNITUDE
      if ( _M_recalc_mag2 )
	{
	  _M_mag2_cached = compute_mag2();
	  _M_recalc_mag2 = false;
	}
      return _M_mag2_cached;
#else
      return compute_mag2();
#endif
    }

    value_type
    mag() const
    {
#ifdef SPT_VECT_CACHE_MAGNITUDE
      if ( _M_recalc_mag )
	{
	  _M_mag_cached = compute_mag();
	  _M_recalc_mag = false;
	}
      return _M_mag_cached;
#else
      return compute_mag();
#endif
    }


    value_type
    compute_mag() const
    {
      return
#ifdef __MINGW32__
	S_SQRT(mag2());
#else
	_N == 2
	? S_HYPOT(_M_val[0], _M_val[1])
	: S_SQRT( mag2() );
#endif
    }

    value_type
    compute_mag2() const
    {
      return dot(*this);
    }

    /** Transform the vector to a unit vector, in-place.
     */
    void
    normalize()
    {
      value_type m(this->mag());

      if ( S_EQ(m, S_LITERAL(1.0)) ||
	   S_LT(m, S_SLOP) )
	return;

      for ( size_type i = 0; i < _N; ++i )
	_M_val[i] /= m;


#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_mag_cached = _M_mag2_cached = S_LITERAL(1.0);
      _M_recalc_mag = _M_recalc_mag2 = false;
#endif

      return;
    }

    Vec
    unit() const
    {
      value_type m(mag());

      if ( S_EQ(m, S_LITERAL(1.0)) ||
	   m < S_SLOP )
	return *this;

      Vec o;

      for ( size_type i = 0; i < _N; ++i )
	o._M_val[i] = _M_val[i] / m;

#ifdef USE_DEBUG
      assert(S_EQ(o.compute_mag(), S_LITERAL(1.0)));
#endif
#ifdef SPT_VECT_CACHE_MAGNITUDE
      o._M_mag_cached = o._M_mag2_cached = S_LITERAL(1.0);
      o._M_recalc_mag = o._M_recalc_mag2 = false;
#endif

      return o;
    }

    /** @bug Possible buffer overflow for template parameter _N
     *	greater than BUFSIZ.  Unlikely, but possible.
     */
    inline const char*
    to_s() const
    {
      unsigned int i;
      static char tbuf[BUFSIZ];
      static char buf[BUFSIZ];		// stupid, I know.  feel free to fix it. :P
      buf[0] = '\0';

      strcat(buf, "{ ");

      for ( i = 0; i < _N; i++ )
	{
	  sprintf(tbuf, "%.2f",static_cast<double>(_M_val[i]));
	  strcat(buf, tbuf);
	  if ( i < _N - 1 )
	    strcat(buf, ", ");
	}
      strcat(buf, " }");

      return buf;
    }

    inline value_type&
    operator[](const size_type vn)
    { 
      if ( vn < _N )
	return _M_val[vn];
      else
	throw std::out_of_range("Bad index in read-write operator [] !");
    }


    inline value_type
    operator[](const size_type vn) const
    {
      if ( vn < _N )
	return const_cast<value_type&>(_M_val[vn]);
      else
	throw std::out_of_range("Bad index in access operator [] !");
    }

    inline bool
    operator==(const Vec& r) const
    {
      if ( ( hasNaN() && ! r.hasNaN() )
	   || ( ! hasNaN() && r.hasNaN() ) )
	return false;

      for ( size_type i ( 0 ); i < _N; ++i )
	if ( S_NE(_M_val[i], r._M_val[i]) )
	  return false;

      return true;
    }


    inline bool
    operator!=(const Vec& r) const
    {
      return !operator==(r);
    }

    inline Vec
    operator+(const Vec& r) const
    {
      Vec out (*this);
      out += r;

      return out;
    }

    inline Vec&
    operator+=(const Vec& r)
    {
      for ( size_type i = 0; i < _N; ++i )
	_M_val[i] += r._M_val[i];

#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = _M_recalc_mag2 = true;
#endif
      return *this;
    }


    inline Vec
    operator-() const
    {
      Vec o ( *this );

      for ( size_type i = 0; i < _N; ++i )
	o._M_val[i] = -_M_val[i];

      return o;
    }



    inline Vec
    operator-(const Vec& r) const
    {
      Vec o ( *this );
      o -= r;

      return o;
    }


    inline Vec&
    operator-=(const Vec& r)
    {
      for ( size_type i = 0; i < _N; ++i )
	_M_val[i] -= r._M_val[i];

#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = _M_recalc_mag2 = true;
#endif
      return *this;
    }


    inline Vec
    operator*(value_type r) const
    {
      Vec out ( *this );
      out *= r;
      return out;
    }

    inline Vec&
    operator*=(value_type r)
    {
      for ( size_type i = 0; i < _N; ++i )
	_M_val[i] *= r;

#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = _M_recalc_mag2 = true;
#endif
      return *this;
    }



    inline Vec
    operator/(value_type r) const
    {
      Vec o ( *this );
      o /= r;

      return o;
    }

    inline Vec&
    operator/=(value_type r)
    {
      for ( size_type i = 0; i < _N; ++i )
	_M_val[i] /= r;

#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = _M_recalc_mag2 = true;
#endif
      return *this;
    }


    inline value_type
    dot(const Vec& r) const
    {
      value_type o(0);
      for ( size_type i = 0; i < _N; ++i )
	o += _M_val[i] * r._M_val[i];

      return o;
    }

    template < typename _VectorType >
    inline value_type
    dot(const _VectorType& other, scalar_t missingValue = S_LITERAL(0.0)) const
    {
      value_type o ( 0 );
      for ( size_type i ( 0 ); i < ( S_NONZERO(missingValue) ? std::max(N, _VectorType::N) : std::min(N, _VectorType::N) ); ++i )
	o += ( i < N ? _M_val[i] : missingValue ) * ( i < _VectorType::N ? other[i] : missingValue );
      return o;
    }


    inline const value_type*
    val() const
    {
      return const_cast<value_type*>(_M_val);
    }

    inline bool
    operator < (const Vec& v) const
    {
      bool
	hN ( hasNaN() ),
	vhN ( v.hasNaN() );

      if ( hN && ! vhN )
	return true;
      else if ( vhN && ! hN )
	return false;

      for ( size_type i ( 0 ); i < _N; ++i )
	if ( _M_val[i] < v._M_val[i] )
	  return true;
      return false;
    }

    bool isFinite() const
    {
      static_assert(std::is_floating_point<value_type>::value,
		    "Invalid use of member ‘isFinite’ on non-floating point Vec type");
      for ( size_type i ( 0 ); i < _N; ++i )
	if ( ! std::isfinite(_M_val[i]) )
	  return false;
      return true;
    }

    bool hasNaN() const
    {
      static_assert(std::is_floating_point<value_type>::value,
		    "Invalid use of member ‘hasNaN’ on non-floating point Vec type");
      for ( size_type i ( 0 ); i < _N; ++i )
	if ( std::isnan(_M_val[i]) )
	  return true;
      return false;
    }
      
    bool hasInfinite() const
    {
      static_assert(std::is_floating_point<value_type>::value,
		    "Invalid use of member ‘hasInfinite’ on non-floating point Vec type");
      for ( size_type i ( 0 ); i < _N; ++i )
	if ( std::fpclassify(_M_val[i]) == FP_INFINITE )
	  return true;
      return false;
    }

  protected:

    value_type _M_val[_N];

#ifdef SPT_VECT_CACHE_MAGNITUDE
    mutable value_type _M_mag_cached;
    mutable bool _M_recalc_mag;
    mutable value_type _M_mag2_cached;
    mutable bool _M_recalc_mag2;
#endif
  };

  template < size_t _N, typename _ValueType, typename _FactorType >
spt::Vec<_N, _ValueType>
operator*(const _FactorType s, const spt::Vec<_N, _ValueType>& v)
{
  static_assert(std::is_arithmetic<_FactorType>::value, "Invalid coefficient type for vector multiplication");
  return v * s;
}
}
#include <ostream>

/**  Shift-append operator for output of an spt::Vec<_N> to an STL stream.
 */
template < size_t _N, typename _ValueType >
std::basic_ostream<char>&
operator << (std::basic_ostream<char>& os, const spt::Vec<_N, _ValueType>& v)
{
  os << "{ " << v[0];
  for ( size_t i = 1; i < _N; ++i )
    os << ", " << v[1];
  return ( os << " }" );
}

#endif	/* Vec_hh */

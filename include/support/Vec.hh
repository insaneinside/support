#ifndef Vec_hh
#define Vec_hh 1

#include <support/scalar.h>
#include <cstdarg>
#include <cstring>
#include <stdexcept>

namespace spt
{
  /** A templatized implementation of Vector
   */
  template < size_t _N >
  class Vec
  {
  public:
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
    Vec(const Vec<_N>& v)
      : _M_val { 0 }
#ifdef SPT_VECT_CACHE_MAGNITUDE
        ,_M_mag_cached(v._M_mag_cached), _M_recalc_mag(v._M_recalc_mag),
	_M_mag2_cached(v._M_mag2_cached), _M_recalc_mag2(v._M_recalc_mag2)
#endif
    {
      for ( size_type i ( 0 ); i < _N; ++i )
	_M_val[i] = v._M_val[i];
    }


    inline
    Vec(const scalar_t v[])
      : _M_val { }
#ifdef SPT_VECT_CACHE_MAGNITUDE
        ,_M_mag_cached(0), _M_recalc_mag(true), _M_mag2_cached(0), _M_recalc_mag2(true)
#endif
    {
      for ( size_type i ( 0 ); i < _N; ++i )
	_M_val[i] = v[i];
    }


    __attribute__(( noinline ))	/* Variadic, so it's not inlinable. */
    Vec(scalar_t s0, ...)
      : _M_val { 0 }
#ifdef SPT_VECT_CACHE_MAGNITUDE
        ,_M_mag_cached(0), _M_recalc_mag(true), _M_mag2_cached(0), _M_recalc_mag2(true)
#endif
    {
      va_list ap;
      va_start(ap, s0);
      
      _M_val[0] = s0;
      for ( size_type i ( 1 ); i < _N; ++i )
	_M_val[i] = static_cast<scalar_t>(va_arg(ap, double));

      va_end(ap);
    }

    /** Destructor. */
    inline virtual
    ~Vec<_N>() {}

    Vec<_N>&
    operator =(const Vec<_N>& other)
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

    Vec<_N>&
    operator =(const scalar_t s[_N])
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
    set(scalar_t first, ...)
    {
      _M_val[0] = first;
      va_list ap;
      va_start(ap, first);
      for ( size_type i ( 1 ); i < _N; ++i )
	_M_val[i] = static_cast<scalar_t>(va_arg(ap, double));

      va_end(ap);
#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
      _M_recalc_mag2 = true;
#endif
    }

    inline void
    set(scalar_t nv[_N])
    {
      for ( size_type i ( 0 ); i < _N; ++i )
	_M_val[i] = nv[i];
#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
      _M_recalc_mag2 = true;
#endif
    }

    inline void
    set(const Vec<_N>& r)
    {
      memcpy(_M_val, r._M_val, _N * sizeof(scalar_t));
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


    scalar_t
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

    scalar_t
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


    scalar_t
    compute_mag() const
    {
      return S_SQRT( mag2() );
    }

    scalar_t
    compute_mag2() const
    {
      return dot(*this);
    }

    /** Transform the vector to a unit vector, in-place.
     */
    void
    normalize()
    {
      scalar_t m(this->mag());

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

    Vec<_N>
    unit() const
    {
      scalar_t m(mag());

      if ( S_EQ(m, S_LITERAL(1.0)) ||
	   m < S_SLOP )
	return *this;

      Vec<_N> o;

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

    inline scalar_t&
    operator[](const size_type vn)
    { 
      if ( vn < _N )
	return _M_val[vn];
      else
	throw std::out_of_range("Bad index in read-write operator [] !");
    }


    inline scalar_t
    operator[](const size_type vn) const
    {
      if ( vn < _N )
	return const_cast<scalar_t&>(_M_val[vn]);
      else
	throw std::out_of_range("Bad index in access operator [] !");
    }

    inline bool
    operator==(const Vec<_N>& r) const
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
    operator!=(const Vec<_N>& r) const
    {
      return !operator==(r);
    }

    inline Vec<_N>
    operator+(const Vec<_N>& r) const
    {
      Vec<_N> out (*this);
      out += r;

      return out;
    }

    inline Vec<_N>&
    operator+=(const Vec<_N>& r)
    {
      for ( size_type i = 0; i < _N; ++i )
	_M_val[i] += r._M_val[i];

#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = _M_recalc_mag2 = true;
#endif
      return *this;
    }


    inline Vec<_N>
    operator-() const
    {
      Vec<_N> o ( *this );

      for ( size_type i = 0; i < _N; ++i )
	o._M_val[i] = -_M_val[i];

      return o;
    }



    inline Vec<_N>
    operator-(const Vec<_N>& r) const
    {
      Vec<_N> o ( *this );
      o -= r;

      return o;
    }


    inline Vec<_N>&
    operator-=(const Vec<_N>& r)
    {
      for ( size_type i = 0; i < _N; ++i )
	_M_val[i] -= r._M_val[i];

#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = _M_recalc_mag2 = true;
#endif
      return *this;
    }


    inline Vec<_N>
    operator*(scalar_t r) const
    {
      Vec<_N> out ( *this );
      out *= r;
      return out;
    }

    inline Vec<_N>&
    operator*=(scalar_t r)
    {
      for ( size_type i = 0; i < _N; ++i )
	_M_val[i] *= r;

#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = _M_recalc_mag2 = true;
#endif
      return *this;
    }



    inline Vec<_N>
    operator/(scalar_t r) const
    {
      Vec<_N> o ( *this );
      o /= r;

      return o;
    }

    inline Vec<_N>&
    operator/=(scalar_t r)
    {
      for ( size_type i = 0; i < _N; ++i )
	_M_val[i] /= r;

#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = _M_recalc_mag2 = true;
#endif
      return *this;
    }


    inline scalar_t
    dot(const Vec<_N>& r) const
    {
      scalar_t o(0);
      for ( size_type i = 0; i < _N; ++i )
	o += _M_val[i] * r._M_val[i];

      return o;
    }

    template < typename _VectorType >
    inline scalar_t
    dot(const _VectorType& other, bool include_missing = false) const
    {
      scalar_t o ( 0 );
      for ( size_type i ( 0 ); i < ( include_missing ? std::max(N, _VectorType::N) : std::min(N, _VectorType::N) ); ++i )
	o += ( i < N ? _M_val[i] : S_LITERAL(1.0) ) * ( i < _VectorType::N ? other[i] : S_LITERAL(1.0) );
      return o;
    }


    inline const scalar_t*
    val() const
    {
      return const_cast<scalar_t*>(_M_val);
    }

    inline bool
    operator < (const Vec<_N>& v) const
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

    bool hasNaN() const
    {
      for ( size_type i ( 0 ); i < _N; ++i )
	if ( std::isnan(_M_val[i]) )
	  return true;
      return false;
    }
      

  protected:

    scalar_t _M_val[_N];

#ifdef SPT_VECT_CACHE_MAGNITUDE
    mutable scalar_t _M_mag_cached;
    mutable bool _M_recalc_mag;
    mutable scalar_t _M_mag2_cached;
    mutable bool _M_recalc_mag2;
#endif
  };

template < size_t _N >
spt::Vec<_N>
operator*(const scalar_t s, const spt::Vec<_N>& v)
{
  return v * s;
}
}
#include <ostream>

/**  Shift-append operator for output of an spt::Vec<_N> to an STL stream.
 */
template < size_t _N >
std::basic_ostream<char>&
operator << (std::basic_ostream<char>& os, const spt::Vec<_N>& v)
{
  os << "{ " << v[0];
  for ( size_t i = 1; i < _N; ++i )
    os << ", " << v[1];
  return ( os << " }" );
}

#endif	/* Vec_hh */

#ifndef SUPPORT_VECT_HH
#define SUPPORT_VECT_HH

#include <support/support-config.h>
#include <support/scalar.h>
#include <support/macro.h>
#include <support/strutils.h>
#include <support/vector.h>

#ifndef __cplusplus
#error this is a c++ header file.
#else

#include <cstdio>
#include <cstdarg>
#include <cfloat>
#include <fenv.h>
#include <stdexcept>
#include <cmath>
#include <support/Vec.hh>
#include <stdarg.h>

#define MSG_OORANGE "condition [-1 < vn < 3] not met"

namespace spt
{
  class Vector
  {
  public:
    typedef size_t size_type;
    static const size_type N = 3;

    inline Vector(Vector&& v)
      : _M_val ( )
#ifdef SPT_VECT_CACHE_MAGNITUDE
      ,_M_mag_cached ( std::move( v._M_mag_cached ) ),
	_M_mag2_cached ( std::move( v._M_mag2_cached ) ),
	_M_recalc_mag ( true ),
	_M_recalc_mag2 ( true )
#endif
    {
      SV_COPY(_M_val, v._M_val);
    }


    template < size_type _N >
    explicit Vector(const Vec<_N>& v)
      : _M_val ( ),
	_M_mag_cached ( ),
	_M_mag2_cached ( ),
	_M_recalc_mag ( true ),
	_M_recalc_mag2 ( true )
    {
      operator = (v);
    }

    template < size_type _N >
    inline Vector&
    operator =(const Vec<_N>& v)
    {
      for ( size_type i ( 0 ); i < N; ++i )
	_M_val[i] = i < _N ? v[i] : S_LITERAL( 0.0 );

#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
      _M_recalc_mag2 = true;
#endif
      return *this;
    }

    template < size_type _N>
    inline scalar_t
    dot(const Vec<_N>& other, bool include_missing = false ) const
    {
      scalar_t o ( 0 );
      for ( unsigned int i ( 0 ); i < ( include_missing ? std::max(N, _N) : std::min(N, _N) ); ++i )
	o += ( i < N ? _M_val[i] : S_LITERAL(1.0) ) * ( i < _N ? other[i] : S_LITERAL(1.0) );
      return o;
    }


    inline Vector()
      : _M_val { 0, 0, 0 }
#ifdef SPT_VECT_CACHE_MAGNITUDE
      ,_M_mag_cached(0), _M_mag2_cached ( 0 ), _M_recalc_mag(false), _M_recalc_mag2 ( false )
#endif
    {
    }


    inline Vector(const Vector& v)
#ifdef SPT_VECT_CACHE_MAGNITUDE
      : _M_mag_cached(v._M_mag_cached), _M_mag2_cached ( v._M_mag2_cached ), _M_recalc_mag(true), _M_recalc_mag2 ( true )
#endif
    {
      SV_COPY(_M_val, v._M_val);
    }

    inline Vector(scalar_t v0, scalar_t v1, scalar_t v2)
      : _M_val { v0, v1, v2 },
#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_mag_cached(0), _M_mag2_cached ( 0 ),  _M_recalc_mag(true), _M_recalc_mag2 ( true )
#endif
    {
    }



/*     Vector(scalar_t v[3])
 * #ifdef SPT_VECT_CACHE_MAGNITUDE
 *       : _M_mag_cached(0), _M_recalc_mag(true)
 * #endif
 *     {
 *       _M_val[0] = v[0];
 *       _M_val[1] = v[1];
 *       _M_val[2] = v[2];
 *     } */

    Vector(const svec_t& v)
#ifdef SPT_VECT_CACHE_MAGNITUDE
      : _M_mag_cached(0), _M_mag2_cached ( 0 ), _M_recalc_mag(true), _M_recalc_mag2 ( true )
#endif
    {
      SV_COPY(_M_val, v);
    }


    Vector(const svec_t* v)
#ifdef SPT_VECT_CACHE_MAGNITUDE
      : _M_mag_cached(0), _M_mag2_cached ( 0 ), _M_recalc_mag(true), _M_recalc_mag2 ( true )
#endif
    {
      SV_COPY(_M_val, *v);
    }

    virtual ~Vector() {}



    inline void
    set(const size_type vn, const scalar_t val)
    {
      if ( vn < 3 )
	_M_val[vn] = val;
      else
	throw std::out_of_range(MSG_OORANGE);
#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
      _M_recalc_mag2 = true;
#endif
    }


    inline void
    set(const scalar_t v0, const scalar_t v1, const scalar_t v2)
    {
      SV_SET(_M_val, v0, v1, v2);
#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
      _M_recalc_mag2 = true;
#endif
    }

    inline void
    set(const scalar_t nv[3])
    {
      SV_SET(_M_val, nv[0], nv[1], nv[2]);
#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
      _M_recalc_mag2 = true;
#endif
    }

    inline void
    set(const Vector& r)
    {
      *this = r;
#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
      _M_recalc_mag2 = true;
#endif
    }

    inline void
    clear()
    {
      SV_SET(_M_val, 0, 0, 0);

#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_mag_cached = 0;
      _M_recalc_mag = false;
      _M_mag2_cached = 0;
      _M_recalc_mag2 = false;
#endif
    }

    inline scalar_t
    mag2() const
    {
      if ( _M_recalc_mag2 )
	{
	  _M_mag2_cached = this->compute_mag2();
	  _M_recalc_mag2 = false;
	}
      return _M_mag2_cached;
    }

    inline scalar_t
    mag() const
    {
#ifdef SPT_VECT_CACHE_MAGNITUDE
      if ( _M_recalc_mag )
	{
	  _M_mag_cached = S_SQRT ( this->mag2() );
	  _M_recalc_mag = false;
	}
      return _M_mag_cached;
#else
      return this->compute_mag();
#endif
    }


    inline scalar_t
    compute_mag2() const
    {
      return
	( _M_val[0] * _M_val[0] ) +
	( _M_val[1] * _M_val[1] ) +
	( _M_val[2] * _M_val[2] );
    }

    inline void
    normalize()
    {
      operator /= (mag());


#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_mag_cached = S_LITERAL(1.0);
      _M_recalc_mag = false;
      _M_mag2_cached = S_LITERAL(1.0);
      _M_recalc_mag2 = false;
#endif

    }


    inline Vector
    unit() const
    {
      return ( (*this) / mag() );
    }
  

    inline const char*
    to_s() const
    {
      sprintf(_M_buf, "{ %.3f, %.3f, %.3f }",
	      static_cast<double>(_M_val[0]),
	      static_cast<double>(_M_val[1]),
	      static_cast<double>(_M_val[2]));

      return const_cast<char*>(_M_buf);
    }

    inline scalar_t&
    operator[](unsigned int vn)
    { 
      if ( vn < 3 )
	return _M_val[vn];
      else
	throw std::out_of_range(MSG_OORANGE);
    }


    inline scalar_t
    operator[](unsigned int vn) const
    {
      if ( vn < 3 )
	return const_cast<scalar_t&>(_M_val[vn]);
      else
	throw std::out_of_range(MSG_OORANGE);
    }

    inline bool
    operator==(const Vector& r) const
    {
      return
	S_EQ(_M_val[0], r._M_val[0]) &&
	S_EQ(_M_val[1], r._M_val[1]) &&
	S_EQ(_M_val[2], r._M_val[2]);
    }


    inline bool
    operator!=(const Vector& r) const
    {
      return !operator==(r);
    }




    inline Vector
    operator+(const Vector& r) const
    {
      return Vector( _M_val[0] + r._M_val[0],
		     _M_val[1] + r._M_val[1],
		     _M_val[2] + r._M_val[2] );
    }


    inline Vector
    operator-(const Vector& r) const
    {
      return Vector( _M_val[0] - r._M_val[0],
		     _M_val[1] - r._M_val[1],
		     _M_val[2] - r._M_val[2] );
    }


    // scalar constant product
    inline Vector
    operator*(const scalar_t r) const
    {
      return Vector( r * _M_val[0],
		     r * _M_val[1],
		     r * _M_val[2] );
    }

    // cross (vector) product
    inline Vector
    operator*(const Vector& r) const
    {
#ifdef USE_DEBUG
      if ( r.check_nan() )
	throw std::invalid_argument("Vector::operator*: right operand vector contains one or more NaN values");
#endif
  
      return Vector( ( _M_val[1] * r._M_val[2] ) - ( _M_val[2] * r._M_val[1] ),
		     ( _M_val[2] * r._M_val[0] ) - ( _M_val[0] * r._M_val[2] ),
		     ( _M_val[0] * r._M_val[1] ) - ( _M_val[1] * r._M_val[0] ) );
    }

    // dot (scalar) product
    inline scalar_t
    dot(const Vector& r) const
    {
      return ( (_M_val[0] * r._M_val[0]) +
	       (_M_val[1] * r._M_val[1]) +
	       (_M_val[2] * r._M_val[2]) );
    }


    inline Vector
    operator/(const scalar_t r) const
    {
      return Vector( _M_val[0] / r,
		     _M_val[1] / r,
		     _M_val[2] / r );

    }


    /* scalar_t
     * operator/(const Vector& r) const
     * {
     *   scalar_t v[1];//3];
     *   v[0] = _M_val[0] / r._M_val[0];
     *   //       v[1] = _M_val[1] / r._M_val[1];
     *   //       v[2] = _M_val[2] / r._M_val[2];
     * 
     *   return v[0];
     * } */
      

    // Assignment
    inline Vector&
    operator+=(const Vector& r)
    {
      _M_val[0] += r._M_val[0];
      _M_val[1] += r._M_val[1];
      _M_val[2] += r._M_val[2];

#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
      _M_recalc_mag2 = true;
#endif
      return *this;
    }

    inline Vector&
    operator-=(const Vector& r)
    {
      _M_val[0] -= r._M_val[0];
      _M_val[1] -= r._M_val[1];
      _M_val[2] -= r._M_val[2];

#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
      _M_recalc_mag2 = true;
#endif

      return *this;
    }

    inline Vector
    operator-() const
    {
      return Vector(-_M_val[0],
		    -_M_val[1],
		    -_M_val[2]);
    }

    // Cross product.
    inline Vector&
    operator*=(const Vector& r)
    {
      scalar_t ov[3];
      ov[0] = _M_val[0];
      ov[1] = _M_val[1];
      ov[2] = _M_val[2];

      set( ( ov[1] * r._M_val[2] ) - ( ov[2] * r._M_val[1] ),
	   ( ov[2] * r._M_val[0] ) - ( ov[0] * r._M_val[2] ),
	   ( ov[0] * r._M_val[1] ) - ( ov[1] * r._M_val[0] ) );

#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
      _M_recalc_mag2 = true;
#endif

      return *this;
    }

    inline Vector&
    operator*=(const scalar_t r)
    {
      _M_val[0] *= r;
      _M_val[1] *= r;
      _M_val[2] *= r;

#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
      _M_recalc_mag2 = true;
#endif

      return *this;
    }

    inline Vector&
    operator/=(const scalar_t r)
    {
      _M_val[0] /= r;
      _M_val[1] /= r;
      _M_val[2] /= r;

#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
      _M_recalc_mag2 = true;
#endif

      return *this;
    }
    
    inline bool
    operator < (const Vector& v) const
    {
      return
	( _M_val[0] < v._M_val[0]
	  ? true
	  : ( _M_val[1] < v._M_val[1]
	      ? true
	      : ( _M_val[2] < v._M_val[2]
		  ? true
		  : false
		  )
	      )
	  );	
    }

    inline bool
    check_nan() const
    {
      if ( std::isnan(_M_val[0]) ||
	   std::isnan(_M_val[1]) ||
	   std::isnan(_M_val[2]) )
	return true;
      return false;
    }

    inline const scalar_t*
    val() const
    {
      return const_cast<scalar_t*>(_M_val);
    }

  protected:
    svec_t _M_val;
    /* scalar_t _M_val[3]; */
    mutable char _M_buf[32];
#ifdef SPT_VECT_CACHE_MAGNITUDE
    mutable scalar_t _M_mag_cached;
    mutable scalar_t _M_mag2_cached;
    mutable bool _M_recalc_mag;
    mutable bool _M_recalc_mag2;
#endif
  };

}
inline spt::Vector
operator*(const scalar_t s, const spt::Vector& v)
  {
    return v * s;
  }

#include <ostream>

/**  Shift-append operator for output of an spt::Vector to an STL stream.
 */
inline std::basic_ostream<char>&
operator << (std::basic_ostream<char>& os, const spt::Vector& v)
{
  return ( os << "{ " << v[0] << ", " << v[1] << ", " << v[2] << " }" );
}

#endif // i'm c++, damnit!
#endif	// SUPPORT_VECT_HH

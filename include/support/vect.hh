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
#include <stdarg.h>

#define MSG_OORANGE "condition [-1 < vn < 3] not met"

namespace spt
{
#define vpos_t Vector		//typedef Vector vpos_t;
#define vdir_t Vector		//typedef Vector vdir_t;
#define vec_t Vector
  class Vector
  {
  public:
    typedef unsigned int size_type;

    inline Vector(Vector&& v)
      : _M_val ( ),
	_M_mag_cached ( std::move( v._M_mag_cached ) ),
	_M_mag2_cached ( std::move( v._M_mag2_cached ) ),
	_M_recalc_mag ( std::move( v._M_recalc_mag ) ),
	_M_recalc_mag2 ( std::move( v._M_recalc_mag2) )
    {
      SV_COPY(_M_val, v._M_val);
    }

    inline Vector()
#ifdef SPT_VECT_CACHE_MAGNITUDE
      : _M_val { 0, 0, 0 },
      _M_mag_cached(0), _M_mag2_cached ( 0 ), _M_recalc_mag(false), _M_recalc_mag2 ( false )
#endif
    {
    }


    inline Vector(const Vector& v)
#ifdef SPT_VECT_CACHE_MAGNITUDE
      : _M_mag_cached(v._M_mag_cached), _M_mag2_cached ( v._M_mag2_cached ), _M_recalc_mag(v._M_recalc_mag), _M_recalc_mag2 ( v._M_recalc_mag2 )
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
	SQ(_M_val[0]) +
	SQ(_M_val[1]) +
	SQ(_M_val[2]);
    }

    inline void
    normalize()
    {
      scalar_t m(this->mag());

      if ( S_EQ(m, S_LITERAL(1.0)) ||
	   S_LT(m, S_SLOP) )
	return;

      _M_val[0] /= m;
      _M_val[1] /= m;
      _M_val[2] /= m;


#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_mag_cached = S_LITERAL(1.0);
      _M_recalc_mag = false;
#endif

      return;
    }

    inline Vector
    unit()
    {
      scalar_t m(mag());

      if ( S_EQ(m, S_LITERAL(1.0)) || S_LT(m, S_SLOP) )
	return *this;

      Vector o(*this);

      o._M_val[0] /= m;
      o._M_val[1] /= m;
      o._M_val[2] /= m;

#ifdef USE_DEBUG
      assert(S_EQ(o.mag(), S_LITERAL(1.0)));
#endif

      return o;
    }


    inline Vector
    unit() const
    {
      scalar_t m = mag();

      if ( S_EQ(m, S_LITERAL(1.0)) ||
	   m < S_SLOP )
	return *this;

      Vector o = *this;

      o._M_val[0] /= m;
      o._M_val[1] /= m;
      o._M_val[2] /= m;

#ifdef USE_DEBUG
      assert(S_EQ(o.mag(), S_LITERAL(1.0)));
#endif
      return o;
    }
  

    inline const char*
    to_s() const
    {
      static char buf[64];		// stupid, I know.  feel free to fix it. :P
      sprintf(buf, "{ %.2f, %.2f, %.2f }",
	      static_cast<double>(_M_val[0]),
	      static_cast<double>(_M_val[1]),
	      static_cast<double>(_M_val[2]));

      return buf;
    }

    inline scalar_t&
    operator[](unsigned int vn)
    { 
      if ( vn < 3 )
	return _M_val[vn];
      else
	throw std::out_of_range(MSG_OORANGE);
    }


    inline scalar_t&
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
#ifdef SPT_VECT_CACHE_MAGNITUDE
      if ( ( !_M_recalc_mag && r._M_recalc_mag ) &&
	   S_EQ(_M_mag_cached, r._M_mag_cached) )
	return true;
#endif

      if ( S_EQ(_M_val[0], r._M_val[0]) &&
	   S_EQ(_M_val[1], r._M_val[1]) &&
	   S_EQ(_M_val[2], r._M_val[2]) )
	return true;

      return false;
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
#endif

      return *this;
    }

    inline Vector
    operator-()
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
#ifdef SPT_VECT_CACHE_MAGNITUDE
    mutable scalar_t _M_mag_cached;
    mutable scalar_t _M_mag2_cached;
    mutable bool _M_recalc_mag;
    mutable bool _M_recalc_mag2;
#endif
  };

  inline Vector
  operator*(const scalar_t s, const Vector& v)
  {
    return v * s;
  }
}
#endif // i'm c++, damnit!
#endif	// SUPPORT_VECT_HH

#ifndef SUPPORT_VECT_HH
#define SUPPORT_VECT_HH

#include <support/support-config.h>
#include <support/scalar.h>
#include <support/macro.h>
#include <support/strutils.h>


#ifndef __cplusplus
#error this is a c++ header file.
#else

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

    Vector()
#ifdef SPT_VECT_CACHE_MAGNITUDE
      : _M_mag_cached(0), _M_recalc_mag(false)
#endif
    {
    }


    Vector(const Vector& v)
#ifdef SPT_VECT_CACHE_MAGNITUDE
      : _M_mag_cached(v._M_mag_cached), _M_recalc_mag(v._M_recalc_mag)
#endif
    {
      SV_COPY(_M_val, v._M_val);
    }

    Vector(scalar_t v0, scalar_t v1, scalar_t v2)
#ifdef SPT_VECT_CACHE_MAGNITUDE
      : _M_mag_cached(0), _M_recalc_mag(true)
#endif
    {
      SV_SET(_M_val, v0, v1, v2);
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
      : _M_mag_cached(0), _M_recalc_mag(true)
#endif
    {
      SV_COPY(_M_val, v);
    }


    Vector(const svec_t* v)
#ifdef SPT_VECT_CACHE_MAGNITUDE
      : _M_mag_cached(0), _M_recalc_mag(true)
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
#endif
    }


    inline void
    set(const scalar_t v0, const scalar_t v1, const scalar_t v2)
    {
      SV_SET(_M_val, v0, v1, v2);
#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
#endif
    }

    inline void
    set(const scalar_t nv[3])
    {
      SV_SET(_M_val, nv[0], nv[1], nv[2]);
#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
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
#endif
    }

    inline scalar_t
    mag()
    {
#ifdef SPT_VECT_CACHE_MAGNITUDE
      if ( _M_recalc_mag )
	{
	  _M_mag_cached = this->compute_mag();
	  _M_recalc_mag = false;
	}

      return _M_mag_cached;
#else
      return this->compute_mag();
#endif
    }

    inline scalar_t
    mag() const
    {
#ifdef SPT_VECT_CACHE_MAGNITUDE
      if ( !_M_recalc_mag )
	return _M_mag_cached;
      else
#endif
	return this->compute_mag();
    }


    inline scalar_t
    compute_mag() const
    {
      return S_SQRT( SQ(_M_val[0]) +
		     SQ(_M_val[1]) +
		     SQ(_M_val[2]) );
    }


    inline void
    normalize()
    {
      scalar_t m(this->mag());

      if ( S_EQ(m, 1.0) ||
	   S_LT(m, S_SLOP) )
	return;

      _M_val[0] /= m;
      _M_val[1] /= m;
      _M_val[2] /= m;


#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_mag_cached = 1.0;
      _M_recalc_mag = false;
#endif

      return;
    }

    inline Vector
    unit()
    {
      scalar_t m(mag());

      if ( S_EQ(m, 1.0) || S_LT(m, S_SLOP) )
	return *this;

      Vector o(*this);

      o._M_val[0] /= m;
      o._M_val[1] /= m;
      o._M_val[2] /= m;

#ifdef USE_DEBUG
      assert(S_EQ(o.compute_mag(), 1.0));
#endif
#ifdef SPT_VECT_CACHE_MAGNITUDE
      o._M_mag_cached = 1.0;
      o._M_recalc_mag = false;
#endif

      return o;
    }


    inline Vector
    unit() const
    {
      scalar_t m = mag();

      if ( S_EQ(m, 1.0) ||
	   m < S_SLOP )
	return *this;

      Vector o = *this;

      o._M_val[0] /= m;
      o._M_val[1] /= m;
      o._M_val[2] /= m;

#ifdef USE_DEBUG
      assert(S_EQ(o.compute_mag(), 1.0));
#endif
#ifdef SPT_VECT_CACHE_MAGNITUDE
      o._M_mag_cached = 1.0;
      o._M_recalc_mag = false;
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
    scalar_t _M_mag_cached;
    bool _M_recalc_mag;
#endif
  };

  inline Vector
  operator*(const scalar_t s, const Vector& v)
  {
    return v * s;
  }





  /** A templatized implementation of Vector
   */
  template < unsigned int _N >
  class Vec
  {
  public:
    typedef unsigned int size_type;

    Vec()
#ifdef SPT_VECT_CACHE_MAGNITUDE
      : _M_mag_cached(0), _M_recalc_mag(false)
#endif
    {
    }


    Vec(const Vec<_N>& v)
#ifdef SPT_VECT_CACHE_MAGNITUDE
      : _M_mag_cached(v._M_mag_cached), _M_recalc_mag(v._M_recalc_mag)
#endif
    {
      memcpy(_M_val, v._M_val, _N * sizeof(scalar_t));
    }


    Vec(const scalar_t* v)
#ifdef SPT_VECT_CACHE_MAGNITUDE
      : _M_mag_cached(0), _M_recalc_mag(true)
#endif
    {
      memcpy(_M_val, v, _N * sizeof(scalar_t));
    }


    Vec(const scalar_t s0, ...)
#ifdef SPT_VECT_CACHE_MAGNITUDE
      : _M_mag_cached(0), _M_recalc_mag(true)
#endif
    {
      va_list ap;
      va_start(ap, s0);
      unsigned int i;
      for ( i = 1; i < _N; i++ )
	_M_val[i] = va_arg(ap, double);

      va_end(ap);
    }

    /** Destructor. */
    virtual
    ~Vec<_N>() {}

    void
    set(unsigned int vn, scalar_t val)
    {
      if ( vn < _N )
	_M_val[vn] = val;
      else
	throw std::out_of_range(MSG_OORANGE);
#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
#endif
    }

    void
    set(scalar_t nv[_N])
    {
      memcpy(_M_val, nv, _N * sizeof(scalar_t));
#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
#endif
    }

    void
    set(const Vec<_N>& r)
    {
      memcpy(_M_val, r._M_val, _N * sizeof(scalar_t));
    }

    void
    clear()
    {
      memset(_M_val, 0, _N * sizeof(scalar_t));

#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_mag_cached = 0;
      _M_recalc_mag = false;
#endif
    }

    scalar_t
    mag()
    {
#ifdef SPT_VECT_CACHE_MAGNITUDE
      if ( _M_recalc_mag )
	{
	  _M_mag_cached = this->compute_mag();
	  _M_recalc_mag = false;
	}

      return _M_mag_cached;
#else
      return this->compute_mag();
#endif
    }

    scalar_t
    mag() const
    {
#ifdef SPT_VECT_CACHE_MAGNITUDE
      if ( !_M_recalc_mag )
	return _M_mag_cached;
      else
#endif
	return this->compute_mag();
    }


    scalar_t
    compute_mag() const
    {
      unsigned int i;
      scalar_t S;

      for ( i = 0; i < _N; i++ )
	S += SQ(_M_val[i]);

      return S_SQRT( S );
    }


    /** Transform the vector to a unit vector, in-place.
     */
    void
    normalize()
    {
      unsigned int i;
      scalar_t m(this->mag());

      if ( S_EQ(m, 1.0) ||
	   S_LT(m, S_SLOP) )
	return;

      for ( i = 0; i < _N; i++ )
	_M_val[i] /= m;


#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_mag_cached = 1.0;
      _M_recalc_mag = false;
#endif

      return;
    }

    /** Get the unit vector with the same direction as this vector. */
    Vec<_N>
    unit()
    {
      scalar_t m(mag());

      if ( S_EQ(m, 1.0) || S_LT(m, S_SLOP) )
	return *this;

      unsigned int i;
      Vec<_N> o;

      for ( i = 0; i < _N; i++ )
	o._M_val[i] = _M_val[i] / m;

#ifdef USE_DEBUG
      assert(S_EQ(o.compute_mag(), 1.0));
#endif
#ifdef SPT_VECT_CACHE_MAGNITUDE
      o._M_mag_cached = 1.0;
      o._M_recalc_mag = false;
#endif

      return o;
    }


    Vec<_N>
    unit() const
    {
      scalar_t m(mag());

      if ( S_EQ(m, 1.0) ||
	   m < S_SLOP )
	return *this;

      unsigned int i;
      Vec<_N> o;

      for ( i = 0; i < _N; i++ )
	o._M_val[i] = _M_val[i] / m;

#ifdef USE_DEBUG
      assert(S_EQ(o.compute_mag(), 1.0));
#endif
#ifdef SPT_VECT_CACHE_MAGNITUDE
      o._M_mag_cached = 1.0;
      o._M_recalc_mag = false;
#endif

      return o;
    }
  

    /** @bug Possible buffer overflow for template parameter _N
     *	greater than BUFSIZ.  Unlikely, but possible.
     */
    const char*
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
	throw std::out_of_range(MSG_OORANGE);
    }


    inline const scalar_t&
    operator[](const size_type vn) const
    {
      if ( vn < _N )
	return const_cast<scalar_t&>(_M_val[vn]);
      else
	throw std::out_of_range(MSG_OORANGE);
    }

    inline bool
    operator==(const Vec<_N>& r) const
    {
      unsigned int i;
      for ( i = 0; i < _N; i++ )
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
      return Vec<_N>( _M_val[0] + r._M_val[0],
		     _M_val[1] + r._M_val[1],
		     _M_val[2] + r._M_val[2] );
    }


    inline Vec<_N>
    operator-(const Vec<_N>& r) const
    {
      return Vec<_N>( _M_val[0] - r._M_val[0],
		     _M_val[1] - r._M_val[1],
		     _M_val[2] - r._M_val[2] );
    }


    // scalar constant product
    inline Vec<_N>
    operator*(scalar_t r) const
    {
      return Vec<_N>( r * _M_val[0],
		     r * _M_val[1],
		     r * _M_val[2] );
    }

    // cross (vector) product
    inline Vec<_N>
    operator*(const Vec<_N>& r) const
    {
#ifdef USE_DEBUG
      if ( r.check_nan() )
	throw std::invalid_argument("Vec<_N>::operator*: right operand vector contains one or more NaN values");
#endif
  
      return Vec<_N>( ( _M_val[1] * r._M_val[2] ) - ( _M_val[2] * r._M_val[1] ),
		     ( _M_val[2] * r._M_val[0] ) - ( _M_val[0] * r._M_val[2] ),
		     ( _M_val[0] * r._M_val[1] ) - ( _M_val[1] * r._M_val[0] ) );
    }

    // dot (scalar) product
    inline scalar_t
    dot(const Vec<_N>& r) const
    {
      unsigned int i;
      scalar_t o(0);

      for ( i = 0; i < _N; i++ )
	o += _M_val[i] * r._M_val[i];

      return o;
    }


    inline Vec<_N>
    operator/(scalar_t r) const
    {

      unsigned int i;
      Vec<_N> o;

      for ( i = 0; i < _N; i++ )
	o._M_val[i] = _M_val[i] / r;

      return o;
    }


    /* scalar_t
     * operator/(const Vec<_N>& r) const
     * {
     *   scalar_t v[1];//3];
     *   v[0] = _M_val[0] / r._M_val[0];
     *   //       v[1] = _M_val[1] / r._M_val[1];
     *   //       v[2] = _M_val[2] / r._M_val[2];
     * 
     *   return v[0];
     * } */
      

    // Assignment
    inline Vec<_N>&
    operator+=(const Vec<_N>& r)
    {
      unsigned int i;
      for ( i = 0; i < _N; i++ )
	_M_val[i] += r._M_val[i];


#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
#endif
      return *this;
    }

    Vec<_N>&
    operator-=(const Vec<_N>& r)
    {
      unsigned int i;
      for ( i = 0; i < _N; i++ )
	_M_val[i] -= r._M_val[i];

#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
#endif

      return *this;
    }

    inline Vec<_N>
    operator-()
    {
      unsigned int i;
      Vec<_N> o;

      for ( i = 0; i < _N; i++ )
	o._M_val[i] = -_M_val[i];

      return o;
    }

    // Cross product.
/*     Vec<_N>&
 *     operator*=(const Vec<_N>& r)
 *     {
 *       scalar_t ov[3];
 *       ov[0] = _M_val[0];
 *       ov[1] = _M_val[1];
 *       ov[2] = _M_val[2];
 * 
 *       set( ( ov[1] * r._M_val[2] ) - ( ov[2] * r._M_val[1] ),
 * 	   ( ov[2] * r._M_val[0] ) - ( ov[0] * r._M_val[2] ),
 * 	   ( ov[0] * r._M_val[1] ) - ( ov[1] * r._M_val[0] ) );
 * 
 * #ifdef SPT_VECT_CACHE_MAGNITUDE
 *       _M_recalc_mag = true;
 * #endif
 * 
 *       return *this;
 *     } */

    inline Vec<_N>&
    operator*=(scalar_t r)
    {
      unsigned int i;
      for ( i = 0; i < _N; i++ )
	_M_val[i] *= r;

#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
#endif

      return *this;
    }

    inline Vec<_N>&
    operator/=(scalar_t r)
    {
      unsigned int i;
      for ( i = 0; i < _N; i++ )
	_M_val[i] /= r;

#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
#endif

      return *this;
    }

    inline const scalar_t*
    val() const
    {
      return const_cast<scalar_t*>(_M_val);
    }

  protected:

    scalar_t _M_val[_N];

#ifdef SPT_VECT_CACHE_MAGNITUDE
    scalar_t _M_mag_cached;
    bool _M_recalc_mag;
#endif
  };

  template < unsigned int _N >
  inline Vec<_N>
  operator*(scalar_t s, const Vec<_N>& v)
  {
    return v * s;
  }

}

#endif // i'm c++, damnit!
#endif	// SUPPORT_VECT_HH

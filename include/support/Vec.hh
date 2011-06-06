#ifndef Vec_hh
#define Vec_hh 1

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
#ifdef SPT_VECT_CACHE_MAGNITUDE
      : _M_mag_cached(0), _M_recalc_mag(false)
#endif
    {
    }


    template < typename _VectorType >
    Vec<_N>&
    operator =(const _VectorType& other)
    {
      for ( unsigned int i ( 0 ); i < N && i < _VectorType::N; ++i )
	_M_val[i] = other[i];
      _M_recalc_mag = true;
      return *this;
    }

    template < typename _VectorType >
    inline scalar_t
    dot(const _VectorType& other, bool include_missing = false) const
    {
      scalar_t o ( 0 );
      for ( unsigned int i ( 0 ); i < ( include_missing ? std::max(N, _VectorType::N) : std::min(N, _VectorType::N) ); ++i )
	o += ( i < N ? _M_val[i] : S_LITERAL(1.0) ) * ( i < _VectorType::N ? other[i] : S_LITERAL(1.0) );
      return o;
    }

    inline
    Vec(const Vec<_N>& v)
#ifdef SPT_VECT_CACHE_MAGNITUDE
      : _M_mag_cached(v._M_mag_cached), _M_recalc_mag(v._M_recalc_mag)
#endif
    {
      for ( unsigned int i ( 0 ); i < _N; ++i )
	_M_val[i] = v._M_val[i];
    }


    Vec(const scalar_t* v)
#ifdef SPT_VECT_CACHE_MAGNITUDE
      : _M_mag_cached(0), _M_recalc_mag(true)
#endif
    {
      for ( unsigned int i ( 0 ); i < _N; ++i )
	_M_val[i] = v[i];
    }


    inline
    Vec(const scalar_t s0, ...)
#ifdef SPT_VECT_CACHE_MAGNITUDE
      : _M_mag_cached(0), _M_recalc_mag(true)
#endif
    {
      va_list ap;
      va_start(ap, s0);
      
      _M_val[0] = s0;
      for ( unsigned int i ( 1 ); i < _N; i++ )
	_M_val[i] = static_cast<scalar_t>(va_arg(ap, double));

      va_end(ap);
    }

    /** Destructor. */
    inline virtual
    ~Vec<_N>() {}

    inline void
    set(scalar_t first, ...)
    {
      _M_val[0] = first;
      va_list ap;
      va_start(ap, first);
      for ( unsigned int i ( 1 ); i < _N; i++ )
	_M_val[i] = static_cast<scalar_t>(va_arg(ap, double));

      va_end(ap);
    }

    inline void
    set(scalar_t nv[_N])
    {
      for ( unsigned int i ( 0 ); i < _N; ++i )
	_M_val[i] = nv[i];
#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_recalc_mag = true;
#endif
    }

    inline void
    set(const Vec<_N>& r)
    {
      memcpy(_M_val, r._M_val, _N * sizeof(scalar_t));
    }

    inline void
    clear()
    {
      for ( unsigned int i ( 0 ); i < _N; ++i )
	_M_val[i] = 0;

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

      if ( S_EQ(m, S_LITERAL(1.0)) ||
	   S_LT(m, S_SLOP) )
	return;

      for ( i = 0; i < _N; i++ )
	_M_val[i] /= m;


#ifdef SPT_VECT_CACHE_MAGNITUDE
      _M_mag_cached = S_LITERAL(1.0);
      _M_recalc_mag = false;
#endif

      return;
    }

    /** Get the unit vector with the same direction as this vector. */
    Vec<_N>
    unit()
    {
      scalar_t m(mag());

      if ( S_EQ(m, S_LITERAL(1.0)) || S_LT(m, S_SLOP) )
	return *this;

      unsigned int i;
      Vec<_N> o;

      for ( i = 0; i < _N; i++ )
	o._M_val[i] = _M_val[i] / m;

#ifdef USE_DEBUG
      assert(S_EQ(o.compute_mag(), S_LITERAL(1.0)));
#endif
#ifdef SPT_VECT_CACHE_MAGNITUDE
      o._M_mag_cached = S_LITERAL(1.0);
      o._M_recalc_mag = false;
#endif

      return o;
    }


    Vec<_N>
    unit() const
    {
      scalar_t m(mag());

      if ( S_EQ(m, S_LITERAL(1.0)) ||
	   m < S_SLOP )
	return *this;

      unsigned int i;
      Vec<_N> o;

      for ( i = 0; i < _N; i++ )
	o._M_val[i] = _M_val[i] / m;

#ifdef USE_DEBUG
      assert(S_EQ(o.compute_mag(), S_LITERAL(1.0)));
#endif
#ifdef SPT_VECT_CACHE_MAGNITUDE
      o._M_mag_cached = S_LITERAL(1.0);
      o._M_recalc_mag = false;
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
	throw std::out_of_range("Bad index in read-write operator!");
    }


    inline scalar_t
    operator[](const size_type vn) const
    {
      if ( vn < _N )
	return const_cast<scalar_t&>(_M_val[vn]);
      else
	throw std::out_of_range("Bad index in access operator!");
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

    inline Vec<_N>&
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
    scalar_t _M_mag2_cached;
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
#endif	/* Vec_hh */

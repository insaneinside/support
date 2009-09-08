#ifndef SUPPORT_SCALAR_H
#define SUPPORT_SCALAR_H	1

#include <support/support-config.h>
#include <support/macro.h>

#ifdef __cplusplus
#include <cmath>
#include <cfloat>
#include <complex>
#else
#include <math.h>
#include <float.h>
#include <complex.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif



#ifndef S_SLOP
#define S_SLOP		(64 * S_EPSILON)
#endif
  /** \name Comparison  */
  /*@{*/
#define S_EQ(a,b) 	S_ABS((a) - (b)) < S_SLOP
#define S_NE(a,b)	S_ABS((a) - (b)) >= S_SLOP
#define S_NONZERO(a)	( a >= S_SLOP || a <= -S_SLOP ) /* S_ABS(a) >= S_SLOP ) */

#define S_ZERO(a) 	(S_ABS(a) < S_SLOP)

#define S_LT0(a)	( a < 0 )
#define S_GT0(a)	( a > 0 )

#define S_LT(a,b) 	( a < b ) /* S_LT0(a - b) */
#define S_LTE(a,b) 	(a) <= (b)
#define S_GT(a,b) 	( a > b ) /* S_LT0(b - a) */
#define S_GTE(a,b) 	((a) >= (b))
  /*@}*/
#define S_SIGNBIT(n)	std::signbit(n)

#ifdef S_TYPE_FLOAT

#define S_EPSILON FLT_EPSILON
#define S_MAXIMUM FLT_MAX

#define S_TYPE 		float

  /* Functions */
#define S_SQRT(n) 	sqrtf(n)
#define S_ABS(n) 	fabsf(n)

#define S_MIN(a, b) 	fminf(a, b)
#define S_MAX(a, b) 	fmaxf(a, b)

#define S_POW(a, b) 	powf(a, b)
#define S_FMOD(a,b)	fmodf(a,b)
#define S_LOG(n)	logf(n)
#define S_EXP(n)	expf(n)

#define S_SIN(n)	sinf(n)
#define S_COS(n)	cosf(n)
#define S_TAN(n)	tanf(n)
#define S_ASIN(n)	asinf(n)
#define S_ACOS(n)	acosf(n)
#define S_ATAN(n)	atanf(n)

#elif defined S_TYPE_DOUBLE

#define S_EPSILON DBL_EPSILON
#define S_MAXIMUM DBL_MAX

#define S_TYPE 		double

#define S_SQRT(n)	sqrt(n)
#define S_ABS(n) 	fabs(n)

#define S_MIN(a, b) 	fmin(a, b)
#define S_MAX(a, b) 	fmax(a, b)

#define S_POW(a, b) 	pow(a, b)
#define S_FMOD(a,b)	fmod(a,b)
#define S_LOG(n)	log(n)
#define S_EXP(n)	exp(n)

#define S_SIN(n)	sin(n)
#define S_COS(n)	cos(n)
#define S_TAN(n)	tan(n)
#define S_ASIN(n)	asin(n)
#define S_ACOS(n)	acos(n)
#define S_ATAN(n)	atan(n)

#elif defined S_TYPE_LONG_DOUBLE

#define S_EPSILON LDBL_EPSILON
#define S_MAXIMUM LDBL_MAX

#define S_TYPE 		long double

#define S_SQRT(n)	sqrtl(n)
#define S_ABS(n) 	fabsl(n)

#define S_MIN(a, b) 	fminl(a, b)
#define S_MAX(a, b) 	fmaxl(a, b)

#define S_POW(a, b) 	powl(a, b)
#define S_FMOD(a,b)	fmodl(a,b)
#define S_LOG(n)	logl(n)
#define S_EXP(n)	expl(n)

#define S_SIN(n)	sinl(n)
#define S_COS(n)	cosl(n)
#define S_TAN(n)	tanl(n)
#define S_ASIN(n)	asinl(n)
#define S_ACOS(n)	acosl(n)
#define S_ATAN(n)	atanl(n)

#else

#ifndef MAKEDEPEND
#error One of S_TYPE_FLOAT, S_TYPE_DOUBLE or S_TYPE_LONG_DOUBLE must be defined!
#endif

#endif /* S_TYPE */

  /* Constants */
#if defined(S_TYPE_DOUBLE) || defined(S_TYPE_FLOAT)
#define S_LITERAL(n)	n##f

#define SC_E		M_E
#define SC_LOG2E	M_LOG2E
#define SC_LOG10E	M_LOG10E
  
#define SC_LN2          M_LN2    
#define SC_LN10         M_LN10   
#define SC_3PI_2	(3 * M_PI_2)
#define SC_2PI		(2 * M_PI)
#define SC_PI           M_PI     
#define SC_PI_2         M_PI_2   
#define SC_PI_4         M_PI_4   
#define SC_1_PI         M_1_PI   
#define SC_2_PI         M_2_PI   
#define SC_2_SQRTP	M_2_SQRTP
#define SC_SQRT2        M_SQRT2  
#define SC_SQRT1_2	M_SQRT1_2
#elif defined(S_TYPE_LONG_DOUBLE)
#define S_LITERAL(n)	n##l

#define SC_E		M_El
#define SC_LOG2E	M_LOG2El
#define SC_LOG10E	M_LOG10El
  
#define SC_LN2          M_LN2l
  
#define SC_LN10         M_LN10l
#define SC_3PI_2	(3 * M_PI_2l)
#define SC_2PI		(2 * M_PIl)
#define SC_PI           M_PIl
#define SC_PI_2         M_PI_2l
#define SC_PI_4         M_PI_4l
#define SC_1_PI         M_1_PIl
#define SC_2_PI         M_2_PIl
#define SC_2_SQRTP	M_2_SQRTPl
#define SC_SQRT2        M_SQRT2l
#define SC_SQRT1_2	M_SQRT1_2l
#endif


typedef S_TYPE scalar_t;


#ifdef __cplusplus
}
#endif

#endif /* SUPPORT_SCALAR_H */

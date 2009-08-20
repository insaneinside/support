#ifndef SUPPORT_VECTOR_H
#define SUPPORT_VECTOR_H	1


#include <support/scalar.h>

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

#include <stdlib.h>		/* for alloca() */
#include <string.h>

#define SV_STRBUFSIZ	64

#define SV_TYPE		scalar_t[4]
#define SV_NAMED(n)	scalar_t n[4]
#define SV_NEW(x,y,z)	{ x, y, z, S_SQRT(SQ(x) + SQ(y) + SQ(y)) }


#define SV_CALC_MAG(v)	S_SQRT(SQ(v[0]) + SQ(v[1]) + SQ(v[2]))
#define SV_MAG(v)	v[3]
#define SV_UNIT(v)	SV_NEW(v[0]/v[3], v[1]/v[3], v[2]/v[3])
#define SV_NORMALIZE(v)	v[3] = V_CALC_MAG(v), v[0] /= v[3], v[1] /= v[3], v[2] /= v[3], v[3] = 1.0

#ifdef __cplusplus
#define SV_STR(v)	svec2str(v, SV_STRBUFSIZ, static_cast<char*>(alloca(SV_STRBUFSIZ)), NULL)
#else
#define SV_STR(v)	vec_simple_str(v, (char *) alloca(SV_STRBUFSIZ), NULL)
#endif

#define SV_DOTP(u, v)	( u[0] * v[0] + u[1] * v[1] + u[2] * v[2] )
#define SV_CROSSP(u, v)	SV_NEW(( u[1] * v[2] ) - ( u[2] * v[1] ),\
			       ( u[2] * v[0] ) - ( u[0] * v[2] ),	\
			       ( u[0] * v[1] ) - ( u[1] * v[0] ))

#define SV_EQ(u, v)	( S_EQ(u[0], v[0]) &&\
			  S_EQ(u[1], v[1]) &&\
			  S_EQ(u[2], v[2]) )


/* #define V_EQ(u, v)	( S_EQ(u[3], v[3]) || ( S_EQ(u[0], v[0]) &&\ */
/*						S_EQ(u[1], v[1]) &&\ */
/*						S_EQ(u[2], v[2]) ) ) */

#define SV_NE(u, v)	!SV_EQ(u, v)

#ifdef SPT_VECT_CACHE_MAGNITUDE
#define SV_COPY(u, v)	u[0] = v[0], u[1] = v[1], u[2] = v[2], u[3] = v[3]
#else
#define SV_COPY(u, v)	u[0] = v[0], u[1] = v[1], u[2] = v[2]
#endif
#define SV_SET(v, x, y, z)	v[0] = x, v[1] = y, v[2] = z
/* #define SV_SET(v, x, y, z)	v[0] = x, v[1] = y, v[2] = z, v[3] = SV_CALC_MAG(v) */

  typedef SV_NAMED(svec_t);
  typedef SV_NAMED(svdir_t);
  typedef SV_NAMED(svpos_t);

  /** Wrapper for vec_simple_str.
   *
   * @return buf
   */
  char*
  svec2str(const svec_t v, const size_t bufsiz, char* buf, const char* format);

  /** Format a simple vector as a string.
   *
   * @param v Vector to represent as a string
   *
   * @param bufsiz Maximum number of characters in the buffer (@c buf
   * argument) to use.
   *
   * @param buf Character buffer in which to store the generated
   * string.
   *
   * @param format printf-style format string.
   *
   * @return Total number of characters that would have been used if
   * no restriction were placed on the number of characters to use.
   * If this is different from @c bufsiz, the string is not a complete
   * representation of the vector.
   */
  int
  vec_simple_str(const svec_t v, const size_t bufsiz, char* buf, const char* format);


  /** Parse a string representation of a vector.
   *
   * @param input Input string.
   *
   * @param dest Pointer to the vector into which to store the result.
   *
   * @param format scanf-style format string.
   */
  int
  vec_from_string(const char* input, svec_t* dest, const char* format);


#ifdef __cplusplus
}
#include <support/vect.hh>

#define V_TYPE		iray::Vector
#define V_NAMED(n)	iray::Vector n
#define V_INIT(n, args)	iray::Vector n(args)
#define V_NEW(x,y,z)	V_TYPE(x, y, z)

#define V_MAG(v)	v.mag()
#define V_UNIT(v)	v.unit()
#define V_NORMALIZE(v)	v.normalize()
#define V_STR(v)	v.to_s()

#define V_DOTP(u, v)	u.dot(v)
#define V_CROSSP(u, v)	(u * v)
#define V_EQ(u,v)	(u == v)
#define V_NE(u,v)	(u != v)

#define V_COPY(u, v)	u.set(v)
#define V_SET(v, x, y, z)	v.set(x, y, z)
#endif	/* __cplusplus */

#endif /* SUPPORT_VECTOR_H */

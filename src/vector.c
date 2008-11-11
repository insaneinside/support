#include <stdio.h>
#include <support/vector.h>

char*
svec2str(const svec_t v, const size_t bufsiz, char* buf, const char* format)
{
  vec_simple_str(v, bufsiz, buf, format);
  return buf;
}

int
vec_simple_str(const svec_t v, const size_t bufsiz, char* buf,
	       const char* format)
{
  const char* default_format = "{ %.3f, %.3f, %.3f }";

  return snprintf(buf, bufsiz,
		  format ? format : default_format,
		  v[0], v[1], v[2]);
}

int
vec_from_string(const char* input, svec_t* dest,
		const char* format)
{
  const char* default_format = "{ %f, %f, %f }";

  return sscanf(input, format ? format : default_format,
		dest + 0,
		dest + sizeof(scalar_t),
		dest + 2 * sizeof(scalar_t));
}

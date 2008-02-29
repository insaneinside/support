#include <stdio.h>
#include <support/vector.h>


char*
vec_simple_str(const svec_t v, char* buf)
{
  sprintf(buf, "{ %.3f, %.3f, %.3f }", v[0], v[1], v[2]);
  return buf;
}


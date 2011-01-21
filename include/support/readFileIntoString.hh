#ifndef readFileIntoString_hh
#define readFileIntoString_hh	1

#include <utility>
#include <support/spt-context.h>

namespace spt
{
  std::pair<char*,size_t>
  readFileIntoString(const char* pathToFile, spt_context_t* errorContext = NULL);
}

#endif	/* defined(readFileIntoString_hh) */

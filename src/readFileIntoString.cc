#include <boost/filesystem.hpp>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <fcntl.h>
#include <malloc.h>
#include <unistd.h>
#include <utility>

#include <support/readFileIntoString.hh>

namespace bfs = boost::filesystem;

namespace spt
{
  std::pair<char*,size_t>
  readFileIntoString(const char* pathToFile, spt_context_t* errorContext)
  {
    size_t bufAllocSize ( BUFSIZ );
    int fd ( -1 );

    if ( !strcmp(pathToFile, "-") )
      fd = STDIN_FILENO;
    else
      {
	fd = open(pathToFile, O_RDONLY);
	if ( fd < 0 )
	  {
	    if ( SPT_IS_CONTEXT(errorContext) )
	      { cmlog(errorContext, V_ERR | F_ERRNO, "Failed to open `%s' for reading", pathToFile); }
	    else { perror(pathToFile); }

	    return std::make_pair<char*,size_t>(NULL, 0);
	  }

	bufAllocSize = bfs::file_size(pathToFile);
      }

    char* buf ( (char*) malloc(bufAllocSize) );
    char* bufPtr ( buf );
    ssize_t bytesRead;          /* bytes read by the last call to read() */
    ssize_t remain;             /* remaining unused bytes in the buffer */
    size_t totalRead ( 0 );
    do {
      remain = bufAllocSize - (bufPtr - buf);

      do {
	bytesRead = read(fd, bufPtr, remain);
	if ( bytesRead > 0 )
	  {
	    totalRead += bytesRead;
	    assert(bytesRead <= remain);
	    remain -= bytesRead;
	    bufPtr += bytesRead;
	  }
      } while ( bytesRead > 0 && remain > 0);

      if ( bytesRead > 0 && fd == STDIN_FILENO ) {
	assert(remain == 0);
	ptrdiff_t ptrOffset ( bufPtr - buf );

	bufAllocSize *= 2;
	buf = (char*) realloc(buf, bufAllocSize);
	bufPtr = buf + ptrOffset;
      }
    } while ( bytesRead > 0 );

    if ( bytesRead < 0 ) {
      size_t errbufLen ( strlen(pathToFile) + 15 ); /* 15 == strlen("Error reading ") + 1 */
      char* errbuf ( (char*) alloca(errbufLen) );
      snprintf(errbuf, errbufLen, "Error reading %s", pathToFile);
      perror(errbuf);

      free(buf);

      buf = NULL;
      totalRead = 0;
    }

    if ( fd != STDIN_FILENO  ) {
      assert(remain == 0);
      close(fd);
    } else /* Resize buffer to match total-bytes-read count  */
      buf = (char*) realloc(buf, totalRead);

    return std::make_pair(buf,totalRead);
  }
}

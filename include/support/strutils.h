#ifndef SUPPORT_STRUTILS_H
#define SUPPORT_STRUTILS_H

#ifdef __cplusplus
extern "C"
{
#endif
#include <sys/types.h>          /* for size_t */

  /** Concatenate strings `a' and `b', storing the result in a
   * separate location.  Automagically allocates memory if passed a
   * NULL dest or n, or a (*n) of 0.
   *
   * @param dest Pointer to destination pointer in which to store
   * result.  I
   *
   *
   * @param a A string.
   *
   * @param b Another string, to be appended to @p a.
   *
   * @return String pointer to the concatenation of @p a and @p b. If
   * @p dest is @c NULL, this will be allocated with @f malloc and
   * must be freed by the user.
   */
  char*
  stalins_cat(char** dest, size_t *n, const char* a, const char* b);


  /** Find the first non-space character in a string.
   *
   * @param in An input string.
   *
   * @return A pointer to the first non-space character in @p in.
   */
  char*
  strchomp(const char* in);

  /** Get a copy of a string, sans trailing whitespace.
   *
   * @param in Input string.
   *
   * @return A newly-allocated string, containing a copy of @p in sans
   * trailing whitespace.
   */
  char*
  strchug(const char* in);


  /** Convenience wrapper for both strchomp and strchug.  Returns a
   * new copy of string @p in with all whitespace removed.
   *
   * @param in The string to strip.
   *
   * @return A newly-allocated string.
   */
  __inline char*
  strstrip(const char* in)
  {
    return strchug(strchomp(in));
  }

  /** Alternative to @f asprintf that returns the created string.
   *
   * \param	format	Format string.
   *
   * \param	...	<code>printf</code>-style arguments for the
   * format string.
   *
   * \returns	A string.
   */
  char*
  astringf(const char* format, ...);

  /** Strip the argument data from a <code>__PRETTY_FUNCTION__</code>-style
   *  string.
   *
   * \param	s	The string to be modified.
   *
   * \returns	A modified string.
   */
  char*
  strip_args(char* s);


  /** Strip the return type information and everything past the
   *  function name from a <code>__PRETTY_FUNCTION__</code>-style
   *  string.  This function modifies the string passed to it.
   *
   * \param	s	The string to be modified.
   *
   * \returns	A modified string.
   */
  char*
  strip_pretty_function(char* s);

#ifdef __cplusplus
}
#endif

#endif	/* SUPPORT_STRUTILS_H */

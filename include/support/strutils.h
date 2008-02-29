#ifndef SUPPORT_STRUTILS_H
#define SUPPORT_STRUTILS_H

#ifdef __cplusplus
extern "C"
{
#endif

  /** Concatenate strings `a' and `b'. Automagically allocates memory
   * if passed a NULL dest or n, or a (*n) of 0.
   *
   * @param	dest	Destination in which to store result.
   */
  char*
  stalins_cat(char** dest, size_t *n, const char* a, const char* b);


  char*
  strchomp(const char*);

  char*
  strchug(const char*);

#define strstrip(s) strchug(strchomp(s))


  /** An alternative to asprintf.
   *
   * \param	format	Format string.
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

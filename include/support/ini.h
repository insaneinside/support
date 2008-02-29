/*! \defgroup	INI	.ini Reading/Writing
 */
#ifndef SUPPORT_INI_H
#define SUPPORT_INI_H

#include <support/meta.h>
#include <support/dllist.h>

#ifdef __cplusplus
extern "C"
{
#endif
  /*@{*/
  typedef struct __ini ini_t;
  typedef struct __ini_section ini_section_t;

  /** \name Creation/destruction */
  /*@{*/
  /** Create a new ini_t.
   *
   * \returns	A pointer to an empty \c ini_t.
   */
  ini_t*	ini_create();


  /** Release the memory used by an \c ini_t.
   *
   * \param	i	a pointer to the \c ini_t to be destroyed
   */
  void		ini_destroy(ini_t* i);


  /** Create a new \c ini_section_t.
   *
   * \returns A pointer to an empty \c ini_section_t.
   */
  ini_section_t* ini_section_create();


  /** Release the memory used by an \c ini_section_t.
   *
   * \param	s	A pointer to the \c ini_section_t to be destroyed.
   */
  void		ini_section_destroy(ini_section_t* s);

  /*@}*/
  /******************************************************************/


  /** Fetch an \c ini_section_t's label
   *
   * \param	s	The target \c ini_section_t.
   */
  const char*	ini_section_get_label(const ini_section_t* s);


  /******************************************************************/
  /** \name File access */
  /*@{*/

  /** Load an INI into memory.
   *
   * \param	path	path to the file to be loaded
   *
   * \returns	a pointer to an \c ini_t, or NULL if errors were encountered.
   */
  ini_t*	ini_load(const char* path);


  /** Write the contents of an \c ini_t to disk.
   *
   * \param	i	The \c ini_t to be written.
   *
   * \param	path	The filename to be written to.
   *
   * \returns	Zero on success, otherwise non-zero.
   */
  int		ini_write(const ini_t* i, const char* path);

  /*@}*/
  /******************************************************************/



  /** Retrieve a list of the sections in an ini.  The returned list
   *  <em>must <strong>not</strong></em> be modified.
   *
   * \param	ini	The \c ini_t to look in.
   *
   * \returns	a dllist of pointers to <code>ini_section_t</code>s.
   */
  const dllist_t*	ini_list_sections(const ini_t* ini);


  /** Find and return a pointer to an \c ini_section_t.
   *
   * \param	i	a pointer to the <code>ini_t</code> where the section is to be found.
   *
   * \param	name	the name of the section to be found
   *
   * \returns	A pointer to the \c ini_section_t if found, otherwise NULL.
   */
  ini_section_t*	ini_find_section(const ini_t* i, const char* name);


  /** Return a list of the values in an \c ini_section_t
   *
   * \param	s	a pointer to the \c ini_section_t
   *
   * \returns	A dllist of pointers to <code>meta_t</code>s.
   */
  const dllist_t*	ini_section_list_values(const ini_section_t* s);

  
  /*@}*/
#ifdef __cplusplus
}
#endif

#endif	/* SUPPORT_INI_H */

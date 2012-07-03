#ifndef String_hh
#define String_hh 1

#include <iostream>
#include <sys/types.h>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <stdexcept>

#include <boost/lexical_cast.hpp>

#include <IndexRange.hh>
#include <RefCountedObject.hh>
#include <StringData.hh>

#include <ostream>

#define STRING_USE_REFCOUNTED_OBJECT

class String;

/**  Shift-append operator for output of a String to an STL stream.
 */
inline std::basic_ostream<char>&
operator << (std::basic_ostream<char>& os, const String& s);


/** Range-defined string view type.  String allows multiple views on
 *  the same data.
 */
class String
#ifdef STRING_USE_REFCOUNTED_OBJECT
  : public RefCountedObject
#endif
{
public:
  typedef char element_type;
  typedef size_t size_type;
  typedef const element_type* const_needle_type;
  typedef StringData<element_type,void> data_type;

  typedef boost::intrusive_ptr<String> reference_type;
  typedef boost::intrusive_ptr<String> const const_reference_type;

  /** a la std::string::npos */
  static constexpr size_type npos = static_cast<size_type>(-1);

  typedef IndexRange<size_type, npos> range_type;

  /**@name Copy/conversion utilities
   *@{
   */
  static inline String
  take_ownership(element_type* str, size_type len = npos, data_type::FreeFunction _ff = &free)
  {
    if ( str )
      {
	if ( len == npos || len == 0)
	  len = strlen(str);
	return String(new data_type(str, len, true, _ff), String::range_type(0, len));
      }
    return String();
  }

  static inline String
  copy_of(const element_type* str, size_type len = npos)
  {
    if ( str )
      {
	if ( len == npos )
	  len = strlen(str);

	if ( len )
	  {
	    String out ( len );
	    if ( out.length() > 0 )
	      strncpy(out.data(), str, out.length());
	    return out;
	    /* return String::take_ownership(strndup(str, len), len); */
	  }
      }
    return String();
  }
  /**@}*/


  inline String()
    :
#ifdef STRING_USE_REFCOUNTED_OBJECT
      RefCountedObject ( ),
#endif
      m_sdata ( NULL ),
      m_range ( 0, 0 )
  {
  }

  inline String(data_type&& data)
    :
#ifdef STRING_USE_REFCOUNTED_OBJECT
      RefCountedObject ( ),
#endif
      m_sdata ( data.capacity > 0 ? new data_type(std::move(data)) : NULL ),
      m_range ( 0, data.capacity )
  {
  }


  inline String(const data_type::reference_type& sdata)
    :
#ifdef STRING_USE_REFCOUNTED_OBJECT
      RefCountedObject ( ),
#endif
      m_sdata ( sdata ),
      m_range ( 0, sdata->capacity )
  {
  }


  inline String(const data_type::reference_type& sdata, const range_type& range)
    :
#ifdef STRING_USE_REFCOUNTED_OBJECT
      RefCountedObject ( ),
#endif
      m_sdata ( sdata ),
      m_range ( range )
  {
  }

  /** Move constructor. */
  inline String(data_type&& sdata, range_type&& range)
    :
#ifdef STRING_USE_REFCOUNTED_OBJECT
      RefCountedObject ( ),
#endif
      m_sdata ( range.length() > 0 ? new data_type(std::move(sdata)) : NULL),
      m_range ( std::move(range) )
  {
  }


  inline String(const element_type* s, size_type length)
    :
#ifdef STRING_USE_REFCOUNTED_OBJECT
      RefCountedObject ( ),
#endif
      m_sdata ( length > 0 && s != NULL ? new data_type(s, length) : NULL),
      m_range ( 0, length )
  {
  }

  inline String(const String& source)
    :
#ifdef STRING_USE_REFCOUNTED_OBJECT
      RefCountedObject ( ),
#endif
      m_sdata ( source.m_sdata ),
      m_range ( source.m_range )
  {
  }

  inline String(const String& source, const range_type& range)
    :
#ifdef STRING_USE_REFCOUNTED_OBJECT
      RefCountedObject ( ),
#endif
      m_sdata ( source.m_sdata ),
      m_range ( source.m_range.startIndex + range.startIndex, source.m_range.startIndex + range.endIndex )
  {
    source.assert_in_range(range);
  }

  /** Read-only constructor. Does not free, copies on write.
   */
  inline String(const char* s)
    :
#ifdef STRING_USE_REFCOUNTED_OBJECT
      RefCountedObject ( ),
#endif
      m_sdata ( s ? new data_type(s) : NULL),
      m_range ( 0, s ? m_sdata->capacity : 0 )
  {
  }

  /** Move constructor.  This initializes the string using an r-value reference.
   */
  inline String(String&& s)
    :
#ifdef STRING_USE_REFCOUNTED_OBJECT
      RefCountedObject ( ),
#endif
      m_sdata ( std::move(s.m_sdata) ),
      m_range ( std::move(s.m_range) )
  {
    s.m_sdata.reset();
  }


  /** @p n copies of char  @p c, followed by <code>'\\0'</code>. */
  explicit inline String(size_type count, element_type c)
    :
#ifdef STRING_USE_REFCOUNTED_OBJECT
      RefCountedObject ( ),
#endif
      m_sdata ( count > 0 ? new data_type(count + 1) : NULL ),
      m_range ( 0, count )
  {
    memset(m_sdata->data, c, count);
    m_sdata->data[count] = '\0';
  }

  /** empty, with space for @p capacity characters (not including terminating null)  */
  explicit inline String(size_type capacity)
    :
#ifdef STRING_USE_REFCOUNTED_OBJECT
      RefCountedObject ( ),
#endif
      m_sdata ( capacity > 0 ? new data_type(capacity) : NULL ),
      m_range ( 0, capacity )
  {
  }

  inline ~String()
#ifdef STRING_USE_REFCOUNTED_OBJECT
  throw ( std::runtime_error )
#endif
  {
  }

  inline size_type
  length() const
  {
    return m_range.length();
  }

  inline size_type
  count(const_needle_type needle, size_t needleLength = npos) const
  {
    if ( needleLength == npos )
      needleLength = strlen(needle);
    size_t numFound ( 0 );

    if ( needleLength )
      {
	const_needle_type found ( static_cast<const_needle_type>(memmem(data(), length(), needle, needleLength)) );
	const element_type* end ( data() + length() );
	while ( found )
	  {
	    ++numFound;
	    found = static_cast<const_needle_type>((found + needleLength) < end
						   ? memmem(found + needleLength, end - found + needleLength, needle, needleLength)
						   : NULL);
	  }
      }
    return numFound;
  }

  inline size_type
  count(element_type elem) const
  {
    size_type out ( 0 );

    if ( data() && length() )
      {
	const element_type* end ( data() + length() );
	for ( const element_type* cur ( data() ); cur < end; ++cur )
	  {
	    if ( *cur == elem )
	      ++out;
	  }
      }

    return out;
  }

  inline size_type
  find_element(element_type elem, size_type startIndex, size_type endIndex = npos) const
  {
    return find_first_of(&elem, range_type(startIndex, endIndex));
  }

  inline size_type
  find_element(element_type elem, range_type range = range_type(npos, npos)) const
  {
    return find_first_of(&elem, range);
  }

  inline size_type
  find_first_of(const_needle_type set, size_type startIndex, size_type endIndex = npos) const
  {
    return find_first_of(set, range_type(startIndex, endIndex));
  }

  inline size_type
  find_first_of(const_needle_type set, range_type range = range_type(npos, npos)) const
  {
    if ( data() )
      {
	if ( range.startIndex == npos )
	  range.startIndex = 0;
	if ( range.endIndex == npos )
	  range = length();
	size_type p ( strcspn(m_sdata->data + m_range.startIndex, set) );
	return p != length() && p >= range.startIndex && p < range.endIndex ? p : npos;
      }
    else
      return npos;
  }

  /* inline reference_type
   * getReference()
   * {
   *   return reference_type(*this);
   * } */

  /** Get a @c const pointer to this string's first character.
   */
  inline const element_type*
  data() const
  {
    return m_sdata ? m_sdata->data + m_range.startIndex : NULL;
  }

  /** Get a pointer to this string's first character.
   */
  inline element_type*
  data()
  {
    return m_sdata ? m_sdata->data + m_range.startIndex : NULL;
  }

  inline String
  trim() const
  {
    String::range_type outRange ( 0, length() );

    while ( outRange.startIndex < outRange.endIndex && std::isspace((*this)[outRange.startIndex]) )
      ++outRange.startIndex;
    while ( outRange.endIndex > outRange.startIndex && std::isspace((*this)[outRange.endIndex - 1]) )
      --outRange.endIndex;

    return substring(outRange);
  }

  inline String
  trim(element_type trimChar) const
  {
    return trim(trimChar, trimChar);
  }
  inline String
  trim(element_type trimS, element_type trimE) const
  {
    String::range_type outRange ( 0, length() );

    while ( outRange.startIndex < outRange.endIndex && (*this)[outRange.startIndex] == trimS )
      ++outRange.startIndex;
    while ( outRange.endIndex > outRange.startIndex && (*this)[outRange.endIndex - 1] == trimE )
      --outRange.endIndex;

    return substring(outRange);
  }


  inline String
  trim(const char* trimSet) const
  {
    return trim(trimSet, trimSet);
  }

  inline String
  trim(const char* trimSetS, const char* trimSetE) const
  {
    String::range_type outRange ( 0, length() );

    if ( trimSetS )
      while ( outRange.startIndex < outRange.endIndex && std::strchr(trimSetS, (*this)[outRange.startIndex]) )
	++outRange.startIndex;
    if ( trimSetE )
      while ( outRange.endIndex > outRange.startIndex && std::strchr(trimSetE, (*this)[outRange.endIndex - 1]) )
	--outRange.endIndex;

    return substring(outRange);
  }


  inline String
  substring(range_type r) const
  {
    assert_in_range(r);
    return String(*this, r);
  }

  /*inline operator Ref<String>()
  {
    return Ref<String>(this);
    }*/

  inline String
  substring(size_type startIndex, size_type endIndex) const
  {
    return substring(range_type(startIndex, endIndex));
  }

  inline size_type
  find(const_needle_type __s, size_type __sLength, range_type range = range_type(npos, npos)) const
  {
    if ( data() )
      {
	if ( range.startIndex == npos )
	  {
	    range.startIndex = 0;
	    range.endIndex = length();
	  }
	assert_in_range(range);
	element_type* p ( static_cast<element_type*>(memmem(m_sdata->data + m_range.startIndex + range.startIndex,
							    m_sdata->capacity - m_range.startIndex - range.startIndex,
							    __s, __sLength)) );
	return p != NULL
	  ? ( p - m_sdata->data - static_cast<signed long int>(m_range.startIndex) < static_cast<ptrdiff_t>(range.endIndex)
	      ? p - m_sdata->data - m_range.startIndex
	      : npos )
	  : npos;
      }
    else
      return npos;
  }

  inline size_type
  find(const_needle_type __s, range_type range = range_type(npos, npos)) const
  {
    return find(__s, strlen(__s), range);
  }

  inline size_type
  find(element_type __s) const
  {
    if ( data() && length() )
      {
	const element_type
	  * cur ( data() ),
	  * end ( cur + length() );
	for ( ; cur < end; ++cur )
	  if ( *cur == __s )
	    return cur - data();
      }
    return npos;
  }


  inline size_type
  rfind(element_type __s) const
  {
    if ( data() && length() )
      {
	const element_type
	  * begin ( data() ),
	  * cur ( begin + length() );
	for ( ; cur >= begin; --cur )
	  if ( *cur == __s )
	    return cur - begin;
      }
    return npos;
  }


  inline size_type
  rfind(const_needle_type __s, size_type __pos, size_type __n = npos) const
  {
    if ( data() )
      {
	const element_type* __data = m_sdata->data + m_range.startIndex;
	size_type __size = this->length();

	if ( __n == npos )
	  __n = this->length();
	assert_in_range(range_type(__pos, __pos + __n));

	__pos = std::min(size_type(__size - __n), __pos);

	do
	  {
	    if (strncmp(__data + __pos, __s, __n) == 0)
	      return __pos - m_range.startIndex;
	  }
	while (__pos-- > 0);

	return npos;
      }
    else
      return npos;
  }

  inline bool
  range_is_valid(const range_type& r) const
  {
    return r.length() <= length() && r.endIndex < length() + 1;
  }
  

  /** Ensure that the given range, relative to the current string's
   *    range, is within bounds.
   */
  inline void
  assert_in_range(const range_type& r) const
  {
    if ( r.length() > length() )
      {
	std::cerr << "implicit: " << m_range << "; relative given: " << r << std::endl;
	throw std::out_of_range("Given range does not fit within implicit range of current string");
      }
    else if ( r.endIndex >= length() + 1 )
      {
	std::cerr << "implicit: " << m_range << "; relative given: " << r << std::endl;
	throw std::out_of_range("Given range exceeds implicit range window of current string");
      }
  }

  /** Checked-index element access. */
  inline element_type
  at(size_type __index) const
  {
    if ( ! data() || __index >= m_range.length() || __index + m_range.startIndex >= m_sdata->capacity )
      throw std::out_of_range("Element at __index is not in valid window");
    return m_sdata->data[__index + m_range.startIndex];
  }

  /** @name Operators
   */
  /**@{*/

  const element_type*
  operator + (size_type offset) const
  {
    return data() + offset;
  }

  const element_type*
  operator *() const
  {
    return data();
  }

  explicit operator const element_type* () const
  {
    return data();
  }

  explicit operator bool () const
  {
    return data() && length();
  }

  /** Unchecked element access.
   */
  inline element_type
  operator [] (size_type __index) const
  {  return m_sdata->data[__index + m_range.startIndex];  }


  inline String
  operator + (const String& s) const
  {
    if ( data() && s.data() )
      {
	data_type::reference_type sdata ( new data_type(this->length() + s.length()) );
	memcpy(stpncpy(sdata->data, this->m_sdata->data + m_range.startIndex, this->length()),
	       s.m_sdata->data + s.m_range.startIndex, s.length());
	return String(sdata, range_type(0, sdata->capacity));
      }
    else if ( data() )
      return *this;
    else
      return s;
  }

  inline bool
  operator == (const char* s) const
  {
    return data()
      ? ( s
	  ? 0 == strncmp(data(), s, length()) && s[length()] == '\0'
	  : false )
      : ( s ? false : true );
  }

  template < typename _Tp >
  inline bool
  operator != (_Tp s) const
  {
    static_assert(std::is_convertible<_Tp, const char*>::value ||
		  std::is_convertible<_Tp, const String&>::value,
		  "Type is not convertible to any accepted string type");
    return ! operator ==(s);
  }

  inline bool
  operator == (const String& s) const
  {
    if ( m_sdata.get() == NULL || s.m_sdata.get() == NULL )
      return m_range.length() == 0 && s.m_range.length() == 0;
    else
      return m_range.length() == s.m_range.length()
	&& 0 == strncmp(m_sdata->data + m_range.startIndex,
			s.m_sdata->data + s.m_range.startIndex,
			m_range.length());
  }
  inline String&
  operator += (const char* s)
  {
    size_type sLen ( strlen(s) );

    if ( data() )
      {
	if ( !hasFullRange() )
	  {
	    data_type* nd ( new data_type(length() + sLen) );
	    strncpy(nd->data, data(), length());
	    m_sdata = nd;
	  }
	else
	  m_sdata->ensureWritable();

	size_type minCapacity ( this->length() + sLen );

	if ( m_sdata->capacity < minCapacity )
	  m_sdata->resize(minCapacity);

	strncpy(m_sdata->data + this->length(), s, sLen);
	m_range.expand(0, sLen);
      }
    else
      *this = String::copy_of(s);

    return *this;
  }


  inline String&
  operator += (const String& s)
  {
    if ( data() )
      {
	if ( ! hasFullRange() )
	  {
	    data_type* nd ( new data_type(length() + s.length()) );
	    strncpy(nd->data, data(), length());
	    m_sdata = nd;
	  }
	else
	  m_sdata->ensureWritable();
	size_type minCapacity ( this->length() + s.length() );

	if ( m_sdata->capacity < minCapacity )
	  m_sdata->resize(minCapacity);

	strncpy(m_sdata->data + this->length(), s.m_sdata->data, s.length());
	m_range.expand(0, s.length());
      }
    else
      {
	m_sdata = s.m_sdata;
	m_range = s.m_range;
      }

    return *this;
  }

  inline String&
  operator = (const String& s)
  {
    m_sdata = s.m_sdata;
    m_range = s.m_range;
    return *this;
  }

  inline String&
  operator = (String&& s)
  {
    m_sdata.swap(s.m_sdata);
    m_range = std::move(s.m_range);
    return *this;
  }
  /**@}*/

  /** Modify the string's implicit range window. */
  inline range_type&
  range() { return m_range; }

  inline const range_type&
  range() const { return m_range; }

  /** Check if the current String is set to view the entire data buffer.  */
  inline bool hasFullRange() const
  {
    /* Note that we return true for NUL-terminated strings whose
     * ranges do not include the terminating NUL.
     */
    return
      (!data()) || ( m_range.startIndex == 0 && ( m_range.endIndex == m_sdata->capacity
						  || ( m_range.endIndex == m_sdata->capacity - 1
						       && m_sdata->data[m_sdata->capacity - 1] == '\0' ) ));
  }

  inline String
  withFullRange() const
  {
    return String(m_sdata);
  }

  /** Check if this is a substring created from the given String.
   *
   * @param s (Potential) parent String.
   *
   * @return @c true if @c *this and @p s refer to the same data, and
   * the implicit range of @p s contains this string's implicit range.
   */
  inline bool
  isSubstringOf(const String& s) const
  {
    return s.m_sdata == m_sdata && s.m_range.contains(m_range);
  }


  /** Get the range of @p substring relative to this String.
   *
   * @param substr Substring of @c *this.
   *
   * @pre @p substr was created as a substring of @c *this, i.e.,
   * <code>substr.isSubstringOf(*this) returns <code>true</code>
   *
   * @return A range corresponding to the relative range of @p substr on @c *this such that
   * <code>this->substring(this->getSubstringRange(substr)) == substr</code>.
   */
  inline range_type
  getSubstringRange(const String& substr) const
  {
    if ( ! substr.isSubstringOf(*this) )
      throw std::logic_error("`substr' is not a substring of `*this'!");

    return range_type(substr.m_range.startIndex - m_range.startIndex, substr.m_range.endIndex - m_range.startIndex);
  }


  inline int
  compare(const String& s) const
  {
    /* Since strcoll relies on nul-terminated buffers (and String does
       not use those -- ever), we must copy the data in each string's
       range. */
    char
      *buf ( static_cast<char*>(alloca(this->length() + s.length() + 2)) ),
      *sbuf ( buf + this->length() + 1 );

    if ( data() )
      strncpy(buf, data(), this->length());
    buf[this->length()] = '\0';

    if ( s.data() )
      strncpy(sbuf, s.data(), s.length());
    sbuf[s.length()] = '\0';

    return strcoll(buf, sbuf);
  }

  /** Collation-order comparator method.  This allows String to be
   *  used inside of STL containers.
   *
   * @param s Another String against which the current object is to be compared.
   *
   * @return @c true if <code>*this</code> sorts before @c s in the
   * current locale.
   */
  inline bool
  operator < (const String& s) const
  {
    return compare(s) < 0;
  }

  /**  Shift-append operator for output of a String to an STL stream.
   */
  inline
  String&
  operator >> (std::basic_ostream<char>& os)
  {
    element_type* _data ( data() );
    os.write(_data ? _data : "(null)", _data ? length() : 6);
    return *this;
  }

private:
  data_type::reference_type m_sdata;
  range_type m_range;
};

inline String::size_type
operator - (const String::element_type* a, const String& b)
{
  return a - b.data();
}

/**  Shift-append operator for output of a String to an STL stream.
 */
inline std::basic_ostream<char>&
operator << (std::basic_ostream<char>& os, const String& s)
{
  const char* data ( s.data() );
  return os.write(data ? data : "(null)", data ? s.length() : 6);
}

#endif	/* String_hh */

#ifndef String_hh
#define String_hh 1

#include <iostream>
#include <sys/types.h>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <StringRange.hh>
#include <boost/lexical_cast.hpp>

#include <RefCountedObject.hh>
#include <StringData.hh>






/** Range-defined string view type.  String allows multiple views on
 *  the same data.
 */
class String
  : public RefCountedObject
{
public:
  typedef char element_type;
  typedef size_t size_type;
  typedef const element_type* const_needle_type;
  typedef StringData<element_type,void> data_type;

  typedef boost::intrusive_ptr<String> reference_type;
  typedef boost::intrusive_ptr<String> const const_reference_type;

  /** a la std::string::npos */
  static const size_type npos = static_cast<size_type>(-1);

  typedef StringRange<size_type, npos> range_type;


  inline String(const data_type::reference_type& sdata, const range_type& range)
    : RefCountedObject ( ),
      m_sdata ( sdata ),
      m_range ( range )
  {
  }

  /** Move constructor. */
  inline String(data_type&& sdata, range_type&& range)
    : RefCountedObject ( ),
      m_sdata ( new data_type(std::move(sdata)) ),
      m_range ( std::move(range) )
  {
  }


  inline String(const element_type* s, size_type length)
    : RefCountedObject ( ),
      m_sdata ( new data_type(s, length) ),
      m_range ( 0, length )
  {
  }

  inline String(const String& source, const range_type& range)
    : RefCountedObject ( ),
      m_sdata ( source.m_sdata ),
      m_range ( source.m_range.startIndex + range.startIndex, source.m_range.startIndex + range.endIndex )
  {
    source.assert_in_range(range);
  }

  /** Read-only constructor. Does not free, copies on write.
   */
  inline String(const char* s = NULL)
    : RefCountedObject ( ),
      m_sdata ( new data_type(s) ),
      m_range ( 0, m_sdata->capacity )
  {
  }

  /** Move constructor.  This initializes the string using an r-value reference.
   */
  inline String(String&& s)
    : RefCountedObject ( ),
      m_sdata ( std::move(s.m_sdata) ),
      m_range ( std::move(s.m_range) )
  {
    /* s.m_sdata.reset(); */
  }


  /** @p n copies of char  @p c, followed by <code>'\\0'</code>. */
  explicit inline String(size_type count, element_type c)
    : RefCountedObject ( ),
      m_sdata ( new data_type(count + 1) ),
      m_range ( 0, count )
  {
    memset(m_sdata->data, c, count);
    m_sdata->data[count] = '\0';
  }

  /** empty, with space for @p capacity characters (not including terminating null)  */
  explicit inline String(size_type capacity)
    : RefCountedObject ( ),
      m_sdata ( new data_type(capacity + 1) ),
      m_range ( 0, capacity )
  {
  }

  inline ~String() throw ( std::runtime_error )
  {
    m_sdata.reset();
  }

  inline size_type
  length() const
  {
    return m_range.length();
  }

  inline size_type
  find_first_of(const_needle_type set, range_type range) const
  {
    size_type p ( strcspn(m_sdata->data + m_range.startIndex, set) );
    return p != length() && p >= range.startIndex && p < range.endIndex ? p : npos;
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

  inline size_type
  find(const_needle_type __s, range_type range = range_type(npos, npos)) const
  {
    return find(__s, strlen(__s), range);
  }

  inline size_type
  rfind(const_needle_type __s, size_type __pos, size_type __n = npos) const
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

  /** Determine whether the given range is valid on this string.
   *
   * @param r Range to test
   *
   * @return @c true if @p r specifies an index range within the length of this string, and @c
   *   false otherwise.
   */
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
      throw std::out_of_range("Given range does not fit within implicit range of current string");
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
    if ( __index >= m_range.length() || __index + m_range.startIndex >= m_sdata->capacity )
      throw std::out_of_range("Element at __index is not in valid window");
    return m_sdata->data[__index + m_range.startIndex];
  }


  /** Unchecked element access.
   */
  inline element_type
  operator [] (size_type __index) const
  {  return m_sdata->data[__index + m_range.startIndex];  }


  inline
  String operator + (const String& s) const
  {
    data_type::reference_type sdata ( new data_type(this->length() + s.length() + 1) );
    memcpy(stpncpy(sdata->data, this->m_sdata->data + m_range.startIndex, this->length()),
	   s.m_sdata->data + s.m_range.startIndex, s.length());
    return String(sdata, range_type(0, sdata->capacity - 1));
  }

  inline bool
  operator == (const String& s) const
  {
    return m_range.length() == s.m_range.length()
      && ( m_sdata.get() == NULL
	   ? m_sdata.get() == s.m_sdata.get()
	   : 0 == strncmp(m_sdata->data + m_range.startIndex,
			  s.m_sdata->data + s.m_range.startIndex, m_range.length()) );
  }

  inline String&
  operator += (const String& s)
  {
    if ( ! hasFullRange() )
      {
	throw std::runtime_error(std::string("In-place-append called on String \"") + "\" without full range (range = ["
			       + boost::lexical_cast<std::string>(m_range.startIndex) + ","
			       + boost::lexical_cast<std::string>(m_range.endIndex) + "]; data length = "
			       + boost::lexical_cast<std::string>(m_sdata->capacity) + ")");
      }
    m_sdata->ensureWritable();
    m_sdata->resize(this->length() + s.length() + 1);
    strncpy(m_sdata->data + this->length(), s.m_sdata->data, s.length());
    return *this;
  }

  /** Check if the current String is set to view the entire data buffer.  */
  inline bool hasFullRange() const
  {
    return m_range.startIndex == 0 && m_range.endIndex == m_sdata->capacity;
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

    return range_type(m_range.startIndex + substr.m_range.startIndex, m_range.startIndex + substr.m_range.endIndex);
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
    /* Since strcoll relies on nul-terminated buffers (and String does
       not use those -- ever), we must copy the data in each string's
       range. */
    char* buf ( static_cast<char*>(alloca(this->length() + s.length() + 2)) );
    char* sbuf ( buf + this->length() + 1 );

    strncpy(buf, data(), this->length());
    buf[this->length()] = '\0';

    strncpy(sbuf, s.data(), s.length());
    sbuf[s.length()] = '\0';

    return strcoll(buf, sbuf) < 0;
  }

private:
  data_type::reference_type m_sdata;
  range_type m_range;
};

#include <ostream>

/**  Shift-append operator for output of a String to an STL stream.
 */
inline std::basic_ostream<char>&
operator << (std::basic_ostream<char>& os, const String& s)
{
  return os.write(s.data(), s.length());
}

#endif	/* String_hh */

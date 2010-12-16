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

/** String-data management helper.  The StringData structure stores
    string data independently of its access  */
template < typename _T, typename _U = _T >
struct StringData
  : RefCountedObject
{
  typedef _T element_type;
  typedef size_t size_type;
  typedef Ref< StringData<_T,_U> > reference_type;
  typedef void (*FreeFunction) (_U*);

  /** Pointer to first byte of data buffer. */
  element_type*	data;

  /** Allocated capacity or known minimum length.
   */
  size_type	capacity;

  /** If @c true, this StringData will free data on destruction. */
  bool		ownsData;

  /** Function the StringData will use to free data. */
  FreeFunction	freeFunction;


  inline StringData(StringData&& __sd)
    : RefCountedObject ( ),
      data ( std::move ( __sd.data ) ),
      capacity ( std::move ( __sd.capacity ) ),
      ownsData ( std::move ( __sd.ownsData ) ),
      freeFunction ( std::move ( __sd.freeFunction ) )
  {
    __sd.data = NULL;
    __sd.ownsData = false;
  }

  explicit inline StringData(const element_type* s)
    : RefCountedObject ( ),
      data ( const_cast<element_type*>(s) ),
      capacity ( s == NULL ? 0 : strlen(s) ),
      ownsData ( false ),
      freeFunction ( NULL )
  {
  }

  explicit inline StringData(size_type _capacity)
    : RefCountedObject ( ),
      data ( static_cast<element_type*>(calloc(_capacity, sizeof(_T))) ),
      capacity ( _capacity ),
      ownsData ( true ),
      freeFunction ( &free )
  {
  }

  explicit inline StringData(element_type* s, size_type knownCapacity, bool takeOwnership = false, FreeFunction _freeFunction = &free)
    : RefCountedObject ( ),
      data ( s ),
      capacity ( knownCapacity ),
      ownsData ( takeOwnership ),
      freeFunction ( _freeFunction )
  {
  }

  inline virtual ~StringData()
  {
    if ( ownsData )
      freeFunction(data);
  }

  /** Ensure that the current StringData owns its data.  If ownsData
   * is @c false, the contents of data will be copied into a new
   *  buffer.
   */
  inline void ensureWritable()
  {
    if ( ! ownsData )
      {
	element_type* newData = static_cast<element_type*>(malloc(capacity));
	memcpy(newData, data, capacity);
	data = newData;
	ownsData = true;
      }
  }

  inline void resize(size_type newCapacity)
  {
    if ( ! ownsData )
      throw std::runtime_error("Resize not allowed on non-owning StringData!");
    else if ( capacity != newCapacity )
      {
	data = static_cast<element_type*>(realloc(data, newCapacity));
	capacity = newCapacity;
      }
  }

};

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

  typedef Ref<String> reference_type;
  typedef Ref<String> const const_reference_type;

  /** a la std::string::npos */
  static const size_type npos = static_cast<size_type>(-1);

  typedef StringRange<size_type, npos> range_type;


  inline String(const Ref<data_type>& sdata, const range_type& range)
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

  inline String(const String& source, const range_type& range)
    : RefCountedObject ( ),
      m_sdata ( source.m_sdata ),
      m_range ( source.m_range.startIndex + range.startIndex, source.m_range.startIndex + range.endIndex )
  {
  }

  /**  Substring reference. */
  explicit inline String(const String& s, size_type startIndex, size_type endIndex)
    : RefCountedObject ( ),
      m_sdata ( s.m_sdata ),
      m_range ( s.m_range.startIndex + startIndex, s.m_range.startIndex + endIndex )
  {
    
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

  inline ~String()
  {
    m_sdata.reset(NULL);
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

  /** Ensure that the given range, relative to the current string's
   *    range, is within bounds.
   */
  inline void
  assert_in_range(const range_type& r) const
  {
    if ( r.startIndex > m_range.endIndex || m_range.startIndex + r.length() > m_range.endIndex )
      throw std::out_of_range("Given range does not fit within range of current string");
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
    Ref<data_type> sdata ( new data_type(this->length() + s.length() + 1) );
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

  /** Collation-order comparator method.  This allows String to be
   *  used inside of STL containers.
   */
  inline bool
  operator < (const String& s) const
  {
    char* buf ( static_cast<char*>(alloca(this->length() + s.length() + 2)) );
    char* sbuf ( buf + this->length() + 1 );

    strncpy(buf, data(), this->length());
    buf[this->length()] = '\0';

    strncpy(sbuf, s.data(), s.length());
    sbuf[s.length()] = '\0';

    return strcoll(buf, sbuf) < 0;
  }

private:
  Ref< data_type >   m_sdata;
  range_type m_range;
};

#include <ostream>

inline std::basic_ostream<char>&
operator << (std::basic_ostream<char>& os, const String& s)
{
  return os.write(s.data(), s.length());
}

#endif	/* String_hh */

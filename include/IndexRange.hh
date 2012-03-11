#ifndef IndexRange_hh
#define IndexRange_hh 1

#include <cassert>
#include <initializer_list>
#include <stdexcept>

/** Index range storage type.  This type is used to specify index
 *  domains for various objects and functions.
 *
 * IndexRanges are specified by the index of the initial element of
 * the target portion of input, and by the index of the first
 * character that <em>follows</em> the target portion of input; thus
 * the length is easily computed as <code>endIndex -
 * startIndex</code>.
 *
 *
 * @param _index_type Storage type for indices.
 *
 * @param _bad_index Value used to represent invalid indices.
 */
template < typename _index_type, _index_type _bad_index >
struct IndexRange
{
  typedef _index_type index_type;

  /** Constructor; initializes the range object to refer to the given range.
   *
   * @param _startIndex Index of the first character in the target range.
   *
   * @param _endIndex Index of the character immediately
   * <em>following</em> the last character in the target range.
   */
  inline IndexRange(index_type _startIndex = _bad_index, index_type _endIndex = _bad_index)
    : startIndex ( _startIndex ),
      endIndex ( _endIndex )
  {
    /* if ( _startIndex != _bad_index )
     *   assert(_endIndex != _bad_index); */
  }

  inline IndexRange(const std::initializer_list<index_type>& ixl)
    : startIndex ( ixl.size() > 0 ? *ixl.begin() : _bad_index ),
      endIndex ( ixl.size() > 1 ? *(ixl.begin() + 1) : _bad_index )
  {
    if ( ixl.size() > 2 )
      throw std::logic_error("Too many values in initializer list to IndexRange<...>");
  }

  inline bool
  valid()
  {
    return endIndex >= startIndex && startIndex >= 0 && endIndex >= 0 && startIndex != _bad_index && endIndex != _bad_index;
  }
  

  static inline index_type
  badIndex()
  {
    return _bad_index;
  }

  /** Less-than operator provided for STL-container compatibility. One range is considered
   * "less" than another if its @c startIndex is less than the other's @c startIndex.
   *
   * @note If you wish to determine whether one IndexRange precedes
   *   another, use IndexRange::precedes.
   */
  inline bool
  operator < (const IndexRange& __rhs) const
  {
    return startIndex < __rhs.startIndex;
  }
  /** Equal-to operator.  Two IndexRanges are equal if their @c startIndex members are equal
   * <strong>and</strong> their @c endIndex members are equal.
   */
  inline bool
  operator == (const IndexRange& __rhs) const
  {
    return startIndex == __rhs.startIndex && endIndex == __rhs.endIndex;
  }


  inline bool
  operator != (const IndexRange& __rhs) const
  {
    return startIndex != __rhs.startIndex
      || endIndex != __rhs.endIndex;
  }

  /** Get the number of characters contained within this range.
   *
   * @return Length of the range.
   */
  inline index_type
  length() const
  {
    return endIndex - startIndex;
  }

  /** Check if this range contains another.
   *
   * @p __rhs IndexRange to check.
   *
   * @return @c true if @p __rhs fits inside <code>*this</code>.
   */
  inline bool
  contains(const IndexRange& __rhs) const
  {
    return __rhs.startIndex >= startIndex && __rhs.endIndex <= endIndex;
  }

  /* Note: Recall that the character at a range's endIndex is /not/ a
       part of that range; this is why we use less-than-or-equal-to
       and greater-than-or-equal-to operators (as opposed to strict
       less-than and greater-than), respectively, in the `precedes'
       and `follows' functions below. */

  /** Check if this range precedes another.
   *
   * @param __rhs IndexRange to check.
   *
   * @return @c true if <code>*this</code> begins and ends prior to @p __rhs.
   */
  inline bool
  precedes(const IndexRange& __rhs) const
  {
    return endIndex <= __rhs.startIndex;
  }

  /** Check if the this range follows another.
   *
   * @param __rhs IndexRange to check.
   *
   * @return @c true if @p __rhs begins and ends prior to <code>*this</code>.
   */
  inline bool
  follows(const IndexRange& __rhs) const
  {
    return startIndex >= __rhs.endIndex;
  }

  inline IndexRange&
  operator = ( const index_type* args )
  {
    startIndex = args[0];
    endIndex = args[1];
    return *this;
  }

  inline IndexRange
  operator - (const IndexRange& __rhs) const
  {
    /* if ( this->contains(__rhs) )
     *   { */
	if ( __rhs.startIndex == startIndex )
	  return IndexRange(__rhs.endIndex, endIndex);
	else if ( __rhs.endIndex == endIndex )
	  return IndexRange(startIndex, __rhs.startIndex);
	else
	  return *this;
    /*   }
     * else
     *   return *this; */
  }

  inline IndexRange&
  operator -= (const IndexRange& __rhs)
  {
    return (*this = *this - __rhs);
  }


  /** Expand the range by a given increment at both limits.  */
  IndexRange&
  expand(index_type frontIncrement, index_type endIncrement)
  {
    if ( startIndex >= frontIncrement )
      startIndex -= frontIncrement;
    else
      startIndex = 0;

    endIndex += endIncrement;
    return *this;
  }


  /** Index of the first character in the target range. */
  index_type startIndex;

  /** Index of the character immediately following the target range. */
  index_type endIndex;
};

#define IndexRange IndexRange

#include <ostream>
#ifdef SWIG
%template(IndexRange) IndexRange<size_t, (size_t) -1>;
#endif

template < typename _index_type, _index_type _bad_index >
inline std::basic_ostream<char>&
operator << (std::basic_ostream<char>& os, const IndexRange<_index_type, _bad_index>& r)
{
#ifdef SWIG
  os.put('[');
  os << r.startIndex;
  os.put(',');
  os << r.endIndex;
  os.put(')');
  return os;
#else
  return os << '[' << r.startIndex << ',' << r.endIndex << ')';
#endif
}


#endif	/* IndexRange_hh */

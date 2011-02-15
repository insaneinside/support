#ifndef StringRange_hh
#define StringRange_hh 1

#include <cassert>

/** String-range storage type.  This type is used to specify the
 *  string-index domain on which various parse functions should
 *  operate.
 *
 * Ranges are specified by the index of the initial character of the target portion of input,
 * and by the index of the first character that follows the target portion of input; thus the
 * length is easily computed as <code>endIndex - startIndex</code>.
 *
 *
 * @param _index_type Storage type for string indices.
 *
 * @param _bad_index Value used to represent invalid indices.
 */
template < typename _index_type, _index_type _bad_index >
struct StringRange
{
  typedef _index_type index_type;

  /** Constructor; initializes the range object to refer to the given range.
   *
   * @param _startIndex Index of the first character in the target range.
   *
   * @param _endIndex Index of the character immediately
   * <em>following</em> the last character in the target range.
   */
  inline StringRange(index_type _startIndex = _bad_index, index_type _endIndex = _bad_index)
    : startIndex ( _startIndex ),
      endIndex ( _endIndex )
  {
    if ( _startIndex != _bad_index )
      assert(_endIndex != _bad_index);
  }

  static inline index_type
  badIndex()
  {
    return _bad_index;
  }

  /** Less-than operator provided for STL-container compatibility. One range is considered
   * "less" than another if its @c startIndex is less than the other's @c startIndex.
   *
   * @note If you wish to determine whether one StringRange precedes
   *   another, use StringRange::precedes.
   */
  inline bool
  operator < (const StringRange& __rhs) const
  {
    return startIndex < __rhs.startIndex;
  }
  /** Equal-to operator.  Two StringRanges are equal if their @c startIndex members are equal
   * <strong>and</strong> their @c endIndex members are equal.
   */
  inline bool
  operator == (const StringRange& __rhs) const
  {
    return startIndex == __rhs.startIndex && endIndex == __rhs.endIndex;
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

  /** Check if the current string range contains another.
   *
   * @p __rhs StringRange to check.
   *
   * @return @c true if @p __rhs fits inside <code>*this</code>.
   */
  inline bool
  contains(const StringRange& __rhs) const
  {
    return __rhs.startIndex >= startIndex && __rhs.endIndex <= endIndex;
  }

  /* Note: Recall that the character at a range's endIndex is /not/ a
       part of that range; this is why we use less-than-or-equal-to
       and greater-than-or-equal-to operators (as opposed to strict
       less-than and greater-than), respectively, in the `precedes'
       and `follows' functions below. */

  /** Check if the current string range precedes another.
   *
   * @param __rhs StringRange to check.
   *
   * @return @c true if <code>*this</code> begins and ends prior to @p __rhs.
   */
  inline bool
  precedes(const StringRange& __rhs) const
  {
    return endIndex <= __rhs.startIndex;
  }

  /** Check if the current string range follows another.
   *
   * @param __rhs StringRange to check.
   *
   * @return @c true if @p __rhs begins and ends prior to <code>*this</code>.
   */
  inline bool
  follows(const StringRange& __rhs) const
  {
    return startIndex >= __rhs.endIndex;
  }

  inline StringRange&
  operator = ( const index_type* args )
  {
    startIndex = args[0];
    endIndex = args[1];
    return *this;
  }

  /** Index of the first character in the target range. */
  index_type startIndex;

  /** Index of the character immediately following the target range. */
  index_type endIndex;
};


#endif	/* StringRange_hh */

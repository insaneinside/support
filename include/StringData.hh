#ifndef StringData_hh
#define StringData_hh 1

/** String-data management helper.  The StringData structure stores
 *  string data independently of its access semantics.
 *
 * @param _T Element (character) type.
 *
 * @param _U Type used by data-free function (e.g., void -> free(void*)).
 */
template < typename _T, typename _U = _T >
struct StringData
  : RefCountedObject
{
  typedef _T element_type;
  typedef size_t size_type;
  typedef boost::intrusive_ptr< StringData<_T,_U> > reference_type;
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


#ifndef SWIG_VERSION
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
#endif

  /** Allocating constructor.  Allocates and initializes the specified number of elements.
   */
  explicit inline StringData(size_type _capacity)
    : RefCountedObject ( ),
      data ( static_cast<element_type*>(calloc(_capacity, sizeof(_T))) ),
      capacity ( _capacity ),
      ownsData ( true ),
      freeFunction ( &free )
  {
  }

  explicit inline StringData(const element_type* s, size_type knownCapacity = 0, bool takeOwnership = false, FreeFunction _freeFunction = &free)
    : RefCountedObject ( ),
      data ( const_cast<element_type*>(s) ),
      capacity ( knownCapacity != 0 ? knownCapacity : ( s == NULL ? 0 : strlen(s) ) ),
      ownsData ( takeOwnership ),
      freeFunction ( _freeFunction )
  {
  }

  inline virtual ~StringData() throw ( std::runtime_error )
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

#endif	/* StringData_hh */

#ifndef SUPPORT_REF_HH
#define SUPPORT_REF_HH

#include <map>
#include <stdexcept>
#include <iostream>

class RefBase {
public:
  RefBase()
  {}

  virtual ~RefBase()
  {}

protected:
  struct info_type {
    unsigned int refcount;
    bool deleteable;

    info_type()
      : refcount(0),
	deleteable(false)
    {}

    info_type(unsigned int rc, bool can_del)
      : refcount(rc),
	deleteable(can_del)
    {}
  };
      
  typedef std::map<void*,info_type > map_type;

  static map_type _S_reflist;
};


template <typename _Tp >
class Ref : RefBase
{
public:
  Ref()
    : _M_ptr()
  {
  }

  Ref(const Ref& __r)
    : _M_ptr(__r._M_ptr)
  {
    if ( _M_ptr )
      refcount(_M_ptr)++;
  }

  Ref(/*const*/ _Tp* __p)
    : _M_ptr()
  {
    reset(__p);
  }

  Ref(_Tp& __r)
    : _M_ptr()
  {
    reset(__r);
  }


  virtual ~Ref()
  {
    if ( _M_ptr )
      disown();
  }

  void
  disown()
  {
    static char buf[64];

    if ( _M_ptr )
      {
	info_type& i = refinfo(_M_ptr);

	i.refcount--;
	if ( i.refcount < 0 )
	  {
	    sprintf(buf, "invalid refcount (%d) for pointer %p", i.refcount, reinterpret_cast<const void*>(_M_ptr));
	    throw std::logic_error(buf);
	  }
	else if ( i.refcount == 0 && i.deleteable == true )
	  {
#ifdef USE_DEBUG
	    sprintf(buf, "deleting pointer %p... ", reinterpret_cast<void*>(_M_ptr));
	    std::cerr << buf;
#endif
	    delete _M_ptr;
#ifdef USE_DEBUG
	    std::cerr << "deleted." << std::endl;
#endif
	  }

	_M_ptr = NULL;
      }
  }


  void
  reset(_Tp* __p = NULL)
  {
    if ( _M_ptr )
      disown();

    if ( __p )
      {
	_M_ptr = __p;
	
	info_type& i = refinfo(_M_ptr);

	if (! i.refcount )
	  {
	    i.deleteable = true;
#ifdef USE_DEBUG
	    static char buf[64];
	    sprintf(buf,
		    "tracking %s %s pointer %p...",
		    i.refcount ? "old" : "new",
		    i.deleteable ? "deleteable" : "indelible",
		    reinterpret_cast<void*>(_M_ptr));
	    std::cout << buf << std::endl;
#endif
	  }
	i.refcount++;
      }
  }

  void reset(Ref& __r)
  {
    if ( _M_ptr )
      disown();

    *this = __r;
  }

  void reset(_Tp& __r)
  {
    if ( _M_ptr )
      disown();

    _M_ptr = &__r;
    if ( !_M_ptr )
      throw std::logic_error("address of given reference is NULL?!");
    refinfo(_M_ptr).refcount++;
  }

  int
  refcount() const
  {
    return refcount(_M_ptr);
  }


  _Tp* get()
  {
    return _M_ptr;
  }

  _Tp* get() const
  {
    return const_cast<_Tp*>(_M_ptr);
  }


  _Tp*
  operator->()
  {
    //static char buf[64];
    //sprintf(buf, "refcount (0x%p) == %d\n", reinterpret_cast<void*>(_M_ptr), refcount());
    //std::cout << buf << std::endl;
    return _M_ptr;
  }

  _Tp*
  operator*()
  {
    return _M_ptr;
  }

  const _Tp*
  operator->() const
  {
    return const_cast<_Tp*>(_M_ptr);
  }
    
  Ref&
  operator=(_Tp* __r)
  {
    if ( __r )
      reset(__r);
    return *this;
  }

  Ref&
  operator=(const Ref& __r)
  {
    reset(__r._M_ptr);
    return *this;
  }

  bool
  operator==(const Ref& __r)
  {
    return this->_M_ptr == __r._M_ptr;
  }

  bool
  operator==(bool __r)
  {
    return ( this->_M_ptr != NULL ) == __r;
  }


  bool operator!=(bool __r)
  {
    return ( this->_M_ptr != NULL) != __r;
  }


  /*bool
    operator!(void)
    {
    return this->_M_ptr == NULL;
    }*/

  bool operator!()
  {
    return this->_M_ptr == NULL;
  }

  bool operator()()
  {
    return this->_M_ptr != NULL;
  }

protected:
  typedef std::pair<_Tp*,info_type > pair_type;

  static info_type&
  refinfo(_Tp* __p)
  {
    if ( ! __p )
      throw std::logic_error("attempted to retrieve refinfo for NULL pointer!");

    if (  _S_reflist.find(__p) != _S_reflist.end() )
      return _S_reflist.find(__p)->second;
    else
      {
	_S_reflist.insert(pair_type(__p, info_type(0, false)));
	return refinfo(__p);
      }
  }


  /*static info_type&
    refinfo(const _Tp* __p)
    {
    return refinfo(dynamic_cast<_Tp*>(__p));
    }*/



  static unsigned int&
  refcount(/*const*/ _Tp* __p)
  {
    return refinfo(__p).refcount;
  }

  static bool&
  deleteable(/*const*/ _Tp* __p)
  {
    return refinfo(__p).deletable;
  }

  _Tp* _M_ptr;
};
/*template <class _Tp >
  bool operator!(Ref<_Tp>& __r)
  {
  return __r->operator!();
  }*/




#endif /* SUPPORT_REF_HH */

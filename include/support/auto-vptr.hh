/* This file is derived from the GNU C++ Standard Template Library header file
 * <memory>.
 */
/*  Provides an `auto_vptr' template class using `delete[]' ( as
 * opposed to `delete' for `std::auto_ptr' ).  Not a very elegant solution...
 */

// Copyright (C) 2001, 2002, 2004 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.

/*
 * Copyright (c) 1997-1999
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */

#ifndef SUPPORT_AUTO_VPTR_HH
#define SUPPORT_AUTO_VPTR_HH

#include <bits/stl_algobase.h>
#include <bits/allocator.h>
#include <bits/stl_construct.h>
#include <bits/stl_iterator_base_types.h> //for iterator_traits
#include <bits/stl_uninitialized.h>
#include <bits/stl_raw_storage_iter.h>
#include <debug/debug.h>
#include <limits>


/**
 *  A wrapper class to provide auto_vptr with reference semantics.
 *  For example, an auto_vptr can be assigned (or constructed from)
 *  the result of a function which returns an auto_vptr by value.
 *
 *  All the auto_vptr_ref stuff should happen behind the scenes.
 */
template<typename _Tp1>
struct auto_vptr_ref
{
  _Tp1* _M_ptr;
      
  explicit
  auto_vptr_ref(_Tp1* __p): _M_ptr(__p) { }
};


/**
 *  @brief  A simple smart pointer providing strict ownership semantics.
 *
 *  The Standard says:
 *  <pre>
 *  An @c auto_vptr owns the object it holds a pointer to.  Copying
 *  an @c auto_vptr copies the pointer and transfers ownership to the
 *  destination.  If more than one @c auto_vptr owns the same object
 *  at the same time the behavior of the program is undefined.
 *
 *  The uses of @c auto_vptr include providing temporary
 *  exception-safety for dynamically allocated memory, passing
 *  ownership of dynamically allocated memory to a function, and
 *  returning dynamically allocated memory from a function.  @c
 *  auto_vptr does not meet the CopyConstructible and Assignable
 *  requirements for Standard Library <a
 *  href="tables.html#65">container</a> elements and thus
 *  instantiating a Standard Library container with an @c auto_vptr
 *  results in undefined behavior.
 *  </pre>
 *  Quoted from [20.4.5]/3.
 *
 *  Good examples of what can and cannot be done with auto_vptr can
 *  be found in the libstdc++ testsuite.
 *
 *  @if maint
 *  _GLIBCXX_RESOLVE_LIB_DEFECTS
 *  127.  auto_vptr<> conversion issues
 *  These resolutions have all been incorporated.
 *  @endif
 */
template<typename _Tp>
class auto_vptr
{
private:
  _Tp* _M_ptr;
      
public:
  /// The pointed-to type.
  typedef _Tp element_type;
      
  /**
   *  @brief  An %auto_vptr is usually constructed from a raw pointer.
   *  @param  __p  A pointer (defaults to NULL).
   *
   *  This object now @e owns the object pointed to by @a p.
   */
  explicit
  auto_vptr(element_type* __p = 0) throw() : _M_ptr(__p) { }

  /**
   *  @brief  An %auto_vptr can be constructed from another %auto_vptr.
   *  @param  __a  Another %auto_vptr of the same type.
   *
   *  This object now @e owns the object previously owned by @a a,
   *  which has given up ownsership.
   */
  auto_vptr(auto_vptr& __a) throw() : _M_ptr(__a.release()) { }

  /**
   *  @brief  An %auto_vptr can be constructed from another %auto_vptr.
   *  @param  __a  Another %auto_vptr of a different but related type.
   *
   *  A pointer-to-Tp1 must be convertible to a
   *  pointer-to-Tp/element_type.
   *
   *  This object now @e owns the object previously owned by @a a,
   *  which has given up ownsership.
   */
  template<typename _Tp1>
  auto_vptr(auto_vptr<_Tp1>& __a) throw() : _M_ptr(__a.release()) { }

  /**
   *  @brief  %auto_vptr assignment operator.
   *  @param  __a  Another %auto_vptr of the same type.
   *
   *  This object now @e owns the object previously owned by @a a,
   *  which has given up ownsership.  The object that this one @e
   *  used to own and track has been delete[]d.
   */
  auto_vptr&
  operator=(auto_vptr& __a) throw()
  {
    reset(__a.release());
    return *this;
  }

  /**
   *  @brief  %auto_vptr assignment operator.
   *  @param  __a  Another %auto_vptr of a different but related type.
   *
   *  A pointer-to-Tp1 must be convertible to a pointer-to-Tp/element_type.
   *
   *  This object now @e owns the object previously owned by @a a,
   *  which has given up ownsership.  The object that this one @e
   *  used to own and track has been delete[]d.
   */
  template<typename _Tp1>
  auto_vptr&
  operator=(auto_vptr<_Tp1>& __a) throw()
  {
    reset(__a.release());
    return *this;
  }

  /**
   *  When the %auto_vptr goes out of scope, the object it owns is
   *  delete[]d.  If it no longer owns anything (i.e., @c get() is
   *  @c NULL), then this has no effect.
   *
   *  @if maint
   *  The C++ standard says there is supposed to be an empty throw
   *  specification here, but omitting it is standard conforming.  Its
   *  presence can be detected only if _Tp::~_Tp() throws, but this is
   *  prohibited.  [17.4.3.6]/2
   *  @endif
   */
  ~auto_vptr() { delete[] _M_ptr; }
      
  /**
   *  @brief  Smart pointer dereferencing.
   *
   *  If this %auto_vptr no longer owns anything, then this
   *  operation will crash.  (For a smart pointer, "no longer owns
   *  anything" is the same as being a null pointer, and you know
   *  what happens when you dereference one of those...)
   */
  element_type&
  operator*() const throw() 
  {
    _GLIBCXX_DEBUG_ASSERT(_M_ptr != 0);
    return *_M_ptr; 
  }
      
  /**
   *  @brief  Smart pointer dereferencing.
   *
   *  This returns the pointer itself, which the language then will
   *  automatically cause to be dereferenced.
   */
  element_type*
  operator->() const throw() 
  {
    _GLIBCXX_DEBUG_ASSERT(_M_ptr != 0);
    return _M_ptr; 
  }
      
  /**
   *  @brief  Bypassing the smart pointer.
   *  @return  The raw pointer being managed.
   *
   *  You can get a copy of the pointer that this object owns, for
   *  situations such as passing to a function which only accepts
   *  a raw pointer.
   *
   *  @note  This %auto_vptr still owns the memory.
   */
  element_type*
  get() const throw() { return _M_ptr; }
      
  /**
   *  @brief  Bypassing the smart pointer.
   *  @return  The raw pointer being managed.
   *
   *  You can get a copy of the pointer that this object owns, for
   *  situations such as passing to a function which only accepts
   *  a raw pointer.
   *
   *  @note  This %auto_vptr no longer owns the memory.  When this object
   *  goes out of scope, nothing will happen.
   */
  element_type*
  release() throw()
  {
    element_type* __tmp = _M_ptr;
    _M_ptr = 0;
    return __tmp;
  }
      
  /**
   *  @brief  Forcibly delete[]s the managed object.
   *  @param  __p  A pointer (defaults to NULL).
   *
   *  This object now @e owns the object pointed to by @a __p.  The
   *  previous object has been delete[]d.
   */
  void
  reset(element_type* __p = 0) throw()
  {
    if (__p != _M_ptr)
      {
	delete[] _M_ptr;
	_M_ptr = __p;
      }
  }
      
  /** 
   *  @brief  Automatic conversions
   *
   *  These operations convert an %auto_vptr into and from an auto_vptr_ref
   *  automatically as needed.  This allows constructs such as
   *  @code
   *    auto_vptr<Derived>  func_returning_auto_vptr(.....);
   *    ...
   *    auto_vptr<Base> ptr = func_returning_auto_vptr(.....);
   *  @endcode
   */
  auto_vptr(auto_vptr_ref<element_type> __ref) throw()
    : _M_ptr(__ref._M_ptr) { }
      
  auto_vptr&
  operator=(auto_vptr_ref<element_type> __ref) throw()
  {
    if (__ref._M_ptr != this->get())
      {
	delete[] _M_ptr;
	_M_ptr = __ref._M_ptr;
      }
    return *this;
  }
      
  template<typename _Tp1>
  operator auto_vptr_ref<_Tp1>() throw()
  { return auto_vptr_ref<_Tp1>(this->release()); }

  template<typename _Tp1>
  operator auto_vptr<_Tp1>() throw()
  { return auto_vptr<_Tp1>(this->release()); }
};


#endif // SUPPORT_AUTO_VPTR_HH

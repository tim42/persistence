//
// file : array_wrapper.hpp
// in : file:///home/tim/projects/yaggler/yaggler/tools/array_wrapper.hpp
//
// created by : Timothée Feuillet on linux-coincoin.tim
// date: 26/10/2013 17:19:39
//
//
// Copyright (C) 2013-2014 Timothée Feuillet
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//


#ifndef __N_750243581824727426_857942864__ARRAY_WRAPPER_HPP__
# define __N_750243581824727426_857942864__ARRAY_WRAPPER_HPP__

namespace neam
{
  // a simple array wrapper (you could use it with yaggler::shader::uniform_variable)
  template<typename Type>
  struct array_wrapper
  {
    constexpr array_wrapper(Type *_array, size_t _size) : array(_array), size(_size) {}

    template<size_t Size>
    constexpr array_wrapper(const Type (&_array)[Size]) : array(const_cast<Type *>(_array)), size(Size) {}

    Type *array;
    size_t size;
  };
} // namespace neam

#endif /*__N_750243581824727426_857942864__ARRAY_WRAPPER_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


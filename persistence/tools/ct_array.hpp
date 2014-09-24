//
// file : ct_array.hpp
// in : file:///home/tim/projects/nsched/nsched/tools/ct_array.hpp
//
// created by : Timothée Feuillet on linux.site
// date: 16/07/2014 17:31:04
//
//
// Copyright (C) 2014 Timothée Feuillet
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

#ifndef __N_14558597131808588598_248435571__CT_ARRAY_HPP__
# define __N_14558597131808588598_248435571__CT_ARRAY_HPP__

namespace neam
{
  namespace ct
  {
    // a simple compile time array
    template<typename Type, Type... Values>
    struct array
    {
      static constexpr size_t size()
      {
        return sizeof...(Values);
      }

      // access
      constexpr operator Type *()
      {
        return data;
      }

      constexpr operator const Type *()
      {
        return data;
      }

      constexpr const Type &operator [](size_t pos)
      {
        return data[pos];
      }

      template<size_t Index>
      static constexpr const Type &at()
      {
        static_assert(Index < sizeof...(Values), "index out of range");
        return data[Index];
      }

      static constexpr const Type &at(size_t index)
      {
        return data[index];
      }

      // iterators
      static constexpr Type *begin()
      {
        return data;
      }
      static constexpr Type *end()
      {
        return data + sizeof...(Values);
      }

      static constexpr Type *rbegin()
      {
        return data + sizeof...(Values) - 1;
      }
      static constexpr Type *rend()
      {
        return data - 1;
      }

      // the CT data
      static constexpr Type data[sizeof...(Values)] = {Values...};
    };

    template<typename Type, Type... Values>
    constexpr Type array<Type, Values...>::data[sizeof...(Values)];

  } // namespace ct
} // namespace neam

#endif /*__N_14558597131808588598_248435571__CT_ARRAY_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


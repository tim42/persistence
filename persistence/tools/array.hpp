//
// file : array.hpp
// in : file:///home/tim/projects/yaggler/yaggler/tools/array.hpp
//
// created by : Timothée Feuillet on linux-coincoin.tim
// date: 08/01/2014 14:24:10
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


#ifndef __N_1633553069808326696_1809251105__ARRAY_HPP__
# define __N_1633553069808326696_1809251105__ARRAY_HPP__

#include <cstddef>

namespace neam
{
  namespace cr
  {
    template <typename Type, size_t Num>
    struct array
    {
      static constexpr size_t size()
      {
        return Num;
      }

      // access
      operator Type *()
      {
        return data;
      }

      constexpr operator const Type *() const
      {
        return data;
      }

      Type &operator [](size_t pos)
      {
        return data[pos];
      }
      constexpr const Type &operator [](size_t pos) const
      {
        return data[pos];
      }

      // iterators
      constexpr Type *begin() const
      {
        return data;
      }
      constexpr Type *end() const
      {
        return data + Num;
      }
      Type *begin()
      {
        return data;
      }
      Type *end()
      {
        return data + Num;
      }

      // data (the C type)
      Type data[Num];
    };
  } // namespace cr
} // namespace neam

#endif /*__N_1633553069808326696_1809251105__ARRAY_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


//
// file : type_at_index.hpp
// in : file:///home/tim/projects/yaggler/yaggler/tools/type_at_index.hpp
//
// created by : Timothée Feuillet on linux-coincoin.tim
// date: 25/01/2014 11:50:49
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

#ifndef __N_116125905863745338_727783173__TYPE_AT_INDEX_HPP__
# define __N_116125905863745338_727783173__TYPE_AT_INDEX_HPP__

#include <cstdint>
#include <cstddef>

namespace neam
{
  namespace ct
  {
    // default (unused)
    template<size_t Index, typename... Types>
    struct type_at_index {};

    // non-empty list
    template<size_t Index, typename Current, typename... Types>
    struct type_at_index<Index, Current, Types...>
    {
      using type = typename type_at_index<Index - 1, Types...>::type;
    };

    template<typename Current, typename... Types>
    struct type_at_index<0, Current, Types...>
    {
      using type = Current;
    };

    // empty list
    template<size_t Index>
    struct type_at_index<Index>
    {
      static_assert(!(Index + 1), "requesting type_at_index of an empty 'type' list. (this could also be caused by an out of range access).");
    };
  } // namespace ct
} // namespace neam

#endif /*__N_116125905863745338_727783173__TYPE_AT_INDEX_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


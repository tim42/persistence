//
// file : ref.hpp
// in : file:///home/tim/projects/yaggler/yaggler/tools/ref.hpp
//
// created by : Timothée Feuillet on linux-coincoin.tim
// date: 16/12/2013 19:59:53
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


#ifndef __N_462308859677856537_1340905228__REF_HPP__
# define __N_462308859677856537_1340905228__REF_HPP__
#include <type_traits>

namespace neam
{
  namespace cr
  {
    // a reference struct
    template<typename Type>
    struct ref
    {
      constexpr ref(Type &_value) : value(_value) {}

      operator Type& ()
      {
        return value;
      }
      operator const Type& () const
      {
        return value;
      }

      Type &value;
    };

    // helpers
    template<typename RefType>
    constexpr ref<RefType> make_ref(RefType &value)
    {
      return ref<RefType>(value);
    }
    template<typename RefType>
    constexpr ref<const RefType> make_const_ref(const RefType &value)
    {
      return ref<const RefType>(value);
    }

    // type traits
    template<typename X> struct is_ref : public std::false_type {};
    template<typename T> struct is_ref<ref<T>> : public std::true_type {};

  } // namespace cr
} // namespace neam

#endif /*__N_462308859677856537_1340905228__REF_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


//
// file : pair.hpp
// in : file:///home/tim/projects/yaggler/yaggler/tools/pair.hpp
//
// created by : Timothée Feuillet on linux-coincoin.tim
// date: 11/02/2014 08:57:23
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

#ifndef __N_10163000121490696598_1767804787__PAIR_HPP__
# define __N_10163000121490696598_1767804787__PAIR_HPP__

namespace neam
{
  namespace ct
  {
    // a pair of types
    template<typename Type1, typename Type2>
    struct pair
    {
      using type_1 = Type1;
      using first = Type1;
      using type_2 = Type2;
      using second = Type2;
    };
  } // namespace ct
} // namespace neam

#endif /*__N_10163000121490696598_1767804787__PAIR_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


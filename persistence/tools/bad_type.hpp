//
// file : bad_type.hpp
// in : file:///home/tim/projects/yaggler/yaggler/tools/bad_type.hpp
//
// created by : Timothée Feuillet on linux-coincoin.tim
// date: 27/01/2014 22:07:37
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

#ifndef __N_2094796812152749172_1184666918__BAD_TYPE_HPP__
# define __N_2094796812152749172_1184666918__BAD_TYPE_HPP__

namespace neam
{
  namespace cr
  {
    class bad_type
    {
      public:
        class construct_from_return {};

        bad_type() = delete;
        bad_type(construct_from_return) {}
        bad_type(bad_type &&) {}
        bad_type(const bad_type &) = delete;
    };
  } // namespace cr
} // namespace neam

#endif /*__N_2094796812152749172_1184666918__BAD_TYPE_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


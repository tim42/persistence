//
// file : genseq.hpp
// in : file:///home/tim/projects/nkark/src/genseq.hpp
//
// created by : Timothée Feuillet on linux-coincoin.tim
// date: 11/08/2013 15:42:19
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


#ifndef __GENSEQ_HPP__
# define __GENSEQ_HPP__

#include <cstdint>

namespace neam
{
  namespace cr
  {
    // a sequence generator (doesn't work on some mingw :( )
    // this comes from a stackoverflow answer.
    template <uint64_t... Idx> struct seq{};
    template <uint64_t Cr, size_t... Idx> struct gen_seq : gen_seq<Cr - 1, Cr - 1, Idx...> {};
    template <uint64_t... Idx> struct gen_seq<0, Idx...> : seq<Idx...> {};

  } // namespace internal
} // namespace nkark

#endif /*__GENSEQ_HPP__*/

//
// file : execute_pack.hpp
// in : file:///home/tim/projects/yaggler/yaggler/tools/execute_pack.hpp
//
// created by : Timothée Feuillet on linux-coincoin.tim
// date: 03/03/2014 11:53:24
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

#ifndef __N_99570474794081008_1534814754__EXECUTE_PACK_HPP__
# define __N_99570474794081008_1534814754__EXECUTE_PACK_HPP__

// do you see THIS ??
// THIS IS THE TRUE (AHEM... ALMOST.) POWER.
//
// this execute 'instr' for each value in the pack (instr must depends of an variadic template / argument pack)
// The compiler is smart enought to just remove the array and keep the calls. (and inlining thems, if possible).
// So this produce no overhead !! :) (and avoid a recursive proxy function).
#define NEAM_EXECUTE_PACK(instr)    void((int []){((instr), 0)...})

// -- usage exemple:
//
// // return the sum of the sizes of all types
// template<typename... Types>
// size_t sizeof_all()
// {
//  size_t size = 0;
//  NEAM_EXECUTE_PACK(size += sizeof(Types));
//  return size;
// }
//

#endif /*__N_99570474794081008_1534814754__EXECUTE_PACK_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


//
// file : enable_if
// in : file:///home/tim/projects/yaggler/yaggler/tools/enable_if
//
// created by : Timothée Feuillet on linux-coincoin.tim
// date: 18/10/2013 00:19:15
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


#ifndef __N_68570312629476691_194362390__ENABLE_IF__
# define __N_68570312629476691_194362390__ENABLE_IF__

namespace neam
{
  namespace cr
  {
    // an enable if (as the one packed with the gnu libc++ doesn't work in my case -- g++ 4.7 & 4.8 on linux --)
    // it cause an error where the sfinae isn't respected and g++ try to get the ::type when Cond is false.
    // this is a simple workaround that work with argument type instead of sfinae
    // there's a new template parameter: Line (usually setted to __LINE__) to create a different type when false (WARNING: this is an **UGLY** solution)
    template <bool Cond, size_t Line, typename Type> struct enable_if
    {
      using type = struct _
      {
        _() = delete;
        ~_() = delete;
      };
    };
    template <typename Type, size_t Line> struct enable_if<true, Line, Type>
    {
      using type = Type;
    };

// and an easy macro to use it :)
// NOTE: DO NOT FORGET BRACKETS AROUND 'Cond' ;)
#define NCR_ENABLE_IF(Cond, Type)   typename neam::cr::enable_if<(Cond), __LINE__ + __COUNTER__, Type>::type

  } // namespace cr

} // namespace neam

#endif /*__N_68570312629476691_194362390__ENABLE_IF__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


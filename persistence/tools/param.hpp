//
// file : param.hpp
// in : file:///home/tim/projects/nsched/nsched/tools/param.hpp
//
// created by : Timothée Feuillet on linux.site
// date: 24/07/2014 20:32:28
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

#ifndef __N_3448725641047881181_206828962__PARAM_HPP__
# define __N_3448725641047881181_206828962__PARAM_HPP__

namespace neam
{
  namespace ct
  {
    template<size_t Index> struct param
    {
      constexpr param() {}
      static constexpr size_t index = Index;
    };
    template<size_t Index> constexpr size_t param<Index>::index;

    // stored param ref
    template<size_t Index, typename Type> struct _sub_param : param<Index>
    {
      Type value;
    };

    // reference to _sub_param
    template<size_t Index, typename Type> struct _ref_sub_param : param<Index>
    {
      _sub_param<Index, Type> &value;
    };

    // a parameter range (used in partial)
    // (suppling only the first parameter mean 'to infinity')
    template<size_t Start, size_t Number = 0xFFFFFFFF>
    struct param_range
    {
      static_assert(Number > 0, "invalid null number of arguments in param_range");

      static constexpr size_t index = Start;
      static constexpr size_t size = Number;
    };

  } // namespace ct
} // namespace neam

#endif /*__N_3448725641047881181_206828962__PARAM_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


//
// file : merge_pack.hpp
// in : file:///home/tim/projects/yaggler/yaggler/tools/merge_pack.hpp
//
// created by : Timothée Feuillet on linux-coincoin.tim
// date: 28/01/2014 13:16:17
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

#ifndef __N_894093274638761124_1022288400__MERGE_PACK_HPP__
# define __N_894093274638761124_1022288400__MERGE_PACK_HPP__

#include <tools/ct_list.hpp>
#include <tools/tuple.hpp>

// merge two type_list / merger / tuple in one type_list / tuple

namespace neam
{
  namespace ct
  {
    // append or prepend a type to a tuple
    template <typename Cr, typename Type> struct prepend_type {};
    template <typename Cr, typename Type> struct append_type {};

    template <typename Cr, typename ...Others>
    struct prepend_type<Cr, cr::tuple<Others...>>
    {
      using type = cr::tuple<Cr, Others...>;
    };
    template <typename Cr, typename ...Others>
    struct append_type<Cr, cr::tuple<Others...>>
    {
      using type = cr::tuple<Others..., Cr>;
    };

    // the merger
    template<typename Type1, typename Type2>
    struct merger {};

    // cr::tuple/cr::tuple case
    template<typename... Types1, typename... Types2>
    struct merger<cr::tuple<Types1...>, cr::tuple<Types2...>>
    {
      using tuple = cr::tuple<Types1..., Types2...>;
      using type_list = ct::type_list<Types1..., Types2...>;
    };

    // ct::type_list/ct::type_list
    template<typename... Types1, typename... Types2>
    struct merger<ct::type_list<Types1...>, ct::type_list<Types2...>>
    {
      using tuple = cr::tuple<Types1..., Types2...>;
      using type_list = ct::type_list<Types1..., Types2...>;
    };

    // ct::merger/ct::merger
    template<typename... Types1, typename... Types2>
    struct merger<ct::merger<Types1...>, ct::merger<Types2...>>
    {
      using tuple = typename ct::merger<typename ct::merger<Types1...>::type_list, typename ct::merger<Types2...>::type_list>::tuple;
      using type_list = typename ct::merger<typename ct::merger<Types1...>::type_list, typename ct::merger<Types2...>::type_list>::type_list;
    };

    // cr::tuple/ct::type_list case
    template<typename... Types1, typename... Types2>
    struct merger<cr::tuple<Types1...>, ct::type_list<Types2...>>
    {
      using tuple = cr::tuple<Types1..., Types2...>;
      using type_list = ct::type_list<Types1..., Types2...>;
    };

    // ct::type_list/cr::tuple case
    template<typename... Types1, typename... Types2>
    struct merger<ct::type_list<Types1...>, cr::tuple<Types2...>>
    {
      using tuple = cr::tuple<Types1..., Types2...>;
      using type_list = ct::type_list<Types1..., Types2...>;
    };

    // ct::merger/ct::type_list
    template<typename... Types1, typename... Types2>
    struct merger<ct::merger<Types1...>, ct::type_list<Types2...>>
    {
      using tuple = typename ct::merger<typename ct::merger<Types1...>::type_list, ct::type_list<Types2...>>::tuple;
      using type_list = typename ct::merger<typename ct::merger<Types1...>::type_list, ct::type_list<Types2...>>::type_list;
    };

    // ct::merger/cr::tuple
    template<typename... Types1, typename... Types2>
    struct merger<ct::merger<Types1...>, cr::tuple<Types2...>>
    {
      using tuple = typename ct::merger<typename ct::merger<Types1...>::type_list, ct::type_list<Types2...>>::tuple;
      using type_list = typename ct::merger<typename ct::merger<Types1...>::type_list, ct::type_list<Types2...>>::type_list;
    };

    // ct::type_list/ct::merger
    template<typename... Types1, typename... Types2>
    struct merger<ct::type_list<Types1...>, ct::merger<Types2...>>
    {
      using tuple = typename ct::merger<ct::type_list<Types1...>, typename ct::merger<Types2...>::type_list>::tuple;
      using type_list = typename ct::merger<ct::type_list<Types1...>, typename ct::merger<Types2...>::type_list>::type_list;
    };

    // cr::tuple/ct::merger
    template<typename... Types1, typename... Types2>
    struct merger<cr::tuple<Types1...>, ct::merger<Types2...>>
    {
      using tuple = typename ct::merger<ct::type_list<Types1...>, typename ct::merger<Types2...>::type_list>::tuple;
      using type_list = typename ct::merger<ct::type_list<Types1...>, typename ct::merger<Types2...>::type_list>::type_list;
    };

  } // namespace ct
} // namespace neam

#endif /*__N_894093274638761124_1022288400__MERGE_PACK_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


//
// file : partial.hpp
// in : file:///home/tim/projects/nsched/nsched/tools/partial.hpp
//
// created by : Timothée Feuillet on linux.site
// date: 24/07/2014 17:33:01
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

#ifndef __N_95822632386518238_1472834320__PARTIAL_HPP__
# define __N_95822632386518238_1472834320__PARTIAL_HPP__

//
// partial: allow automatic creation of partial template bindings (a bit like boost::bind() would do for functions)
// NOTE: only works with type-only templates
// ATTENTION/NOTICE: you should use a simple 'using' instead !!! ;)
//
// exemple: partial<my_class, type1, type2, param<0>, type3, type4, param<3>>::type
// or       partial<my_class, type_list<type1, type2, param<0>, type3, type4, param<3>>>::type
//
// and      type<ptype1, ptype2, ptype3, ptype4, ...>::type (or using cast operators with the 'partial<>' class directly, or with the 'type<>' alias.)

#include <tools/param.hpp>
#include <tools/ct_list.hpp>

namespace neam
{
  namespace ct
  {
    template<template<typename...> class Type, typename... Args>
    struct partial
    {
      private: // maker/filter
        // filter
        template<typename ParamList, typename Current, typename TL> struct filter {};
        template<typename ParamList, typename Current, typename... Others>
        struct filter<ParamList, Current, type_list<Others...>>
        {
          using type = type_list<Current, Others...>;
        };
        template<typename ParamList, size_t Index, typename... Others>
        struct filter<ParamList, param<Index>, type_list<Others...>>
        {
          static_assert(Index < ParamList::size, "Parameter out of range in neam::ct::partial");
          using type = type_list<typename ParamList::template get_type<Index>, Others...>;
        };

        // maker
        template<typename ParamList, typename... Nexts>
        struct maker
        {
          using type = type_list<>;
        };
        template<typename ParamList, typename Current, typename... Nexts>
        struct maker<ParamList, Current, Nexts...>
        {
          using type = typename filter<ParamList, Current, typename maker<ParamList, Nexts...>::type>::type;
        };

      private: // specialisation
        template<typename ...SubArgs>
        struct _sub_
        {
          using type = typename extract_types<Type, typename maker<type_list<SubArgs...>, Args...>::type>::type;
        };

        // for type_list
        template<typename ...SubArgs>
        struct _sub_<type_list<SubArgs...>> : public _sub_<SubArgs...> {};

      public:
        // with the variadic template
        template<typename ...SubArgs>
        using type = typename _sub_<SubArgs...>::type;

        // other 'types' decls
        template<typename T1>
        using type_1 = typename _sub_<T1>::type;
        template<typename T1, typename... V>
        using type_1v = typename _sub_<T1, V...>::type;

        template<typename T1, typename T2>
        using type_2 = typename _sub_<T1, T2>::type;
        template<typename T1, typename T2, typename... V>
        using type_2v = typename _sub_<T1, T2, V...>::type;

        template<typename T1, typename T2, typename T3>
        using type_3 = typename _sub_<T1, T2, T3>::type;
        template<typename T1, typename T2, typename T3, typename... V>
        using type_3v = typename _sub_<T1, T2, T3, V...>::type;

        template<typename T1, typename T2, typename T3, typename T4>
        using type_4 = typename _sub_<T1, T2, T3, T4>::type;
        template<typename T1, typename T2, typename T3, typename T4, typename... V>
        using type_4v = typename _sub_<T1, T2, T3, T4, V...>::type;

        template<typename T1, typename T2, typename T3, typename T4, typename T5>
        using type_5 = typename _sub_<T1, T2, T3, T4, T5>::type;
        template<typename T1, typename T2, typename T3, typename T4, typename T5, typename... V>
        using type_5v = typename _sub_<T1, T2, T3, T4, T5, V...>::type;

        template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
        using type_6 = typename _sub_<T1, T2, T3, T4, T5, T6>::type;
        template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename... V>
        using type_6v = typename _sub_<T1, T2, T3, T4, T5, T6, V...>::type;

        template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
        using type_7 = typename _sub_<T1, T2, T3, T4, T5, T6, T7>::type;
        template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename... V>
        using type_7v = typename _sub_<T1, T2, T3, T4, T5, T6, T7, V...>::type;

        template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
        using type_8 = typename _sub_<T1, T2, T3, T4, T5, T6, T7, T8>::type;
        template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename... V>
        using type_8v = typename _sub_<T1, T2, T3, T4, T5, T6, T7, T8, V...>::type;

        template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
        using type_9 = typename _sub_<T1, T2, T3, T4, T5, T6, T7, T8, T9>::type;
        template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename... V>
        using type_9v = typename _sub_<T1, T2, T3, T4, T5, T6, T7, T8, T9, V...>::type;

        template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
        using type_10 = typename _sub_<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>::type;
        template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename... V>
        using type_10v = typename _sub_<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, V...>::type;
    };

    // for type_list
    template<template<typename...> class Type, typename... Args>
    struct partial<Type, type_list<Args...>> : public partial<Type, Args...>{};
  } // namespace ct
} // namespace neam

#endif /*__N_95822632386518238_1472834320__PARTIAL_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


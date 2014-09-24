//
// file : function.hpp
// in : file:///home/tim/projects/nsched/nsched/tools/function.hpp
//
// created by : Timothée Feuillet on linux.site
// date: 17/07/2014 11:59:09
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

#ifndef __N_2111737739159269973_1204098121__FUNCTION_HPP__
# define __N_2111737739159269973_1204098121__FUNCTION_HPP__

#include <tools/ct_list.hpp>
#include <tools/execute_pack.hpp> // some magic
#include <tools/constructor_call.hpp>
#include <tools/param.hpp>

namespace neam
{
  namespace ct
  {
    template<typename Ptr>
    struct is_function_pointer
    {
      static constexpr bool value = false;
    };
    template<typename Ret, typename... Params>
    struct is_function_pointer<Ret(*)(Params...)>
    {
      static constexpr bool value = true;
    };
    template<typename Ret, typename Class, typename... Params>
    struct is_function_pointer<Ret(Class::*)(Params...)>
    {
      static constexpr bool value = true;
    };
    template<typename Ret, typename... Params>
    struct is_function_pointer<Ret(&)(Params...)>
    {
      static constexpr bool value = true;
    };

    // /// ///
    // for a ct function wrapper
    // /// ///

    // the wrappers
    template<typename PtrType, PtrType Ptr, typename... Args> struct function_wrapper {};      // the one that wrap
    template<typename PtrType, PtrType Ptr, typename... Args> struct _sub_function_wrapper {}; // the result one

    // for C functions
    template<typename PtrType, PtrType Ptr, typename Return, typename... FuncArgList>
    struct function_wrapper<PtrType, Ptr, Return, type_list<FuncArgList...>>
    {
      template<typename... Types>
      static constexpr _sub_function_wrapper<PtrType, Ptr, Return, type_list<FuncArgList...>, type_list<Types...>> wrap(Types... values)
      {
        return _sub_function_wrapper<PtrType, Ptr, Return, type_list<FuncArgList...>, type_list<Types...>>(values...);
      }
    };

    template<typename PtrType, PtrType Ptr, typename Return, typename... FuncArgList, typename... ArgList>
    struct _sub_function_wrapper<PtrType, Ptr, Return, type_list<FuncArgList...>, type_list<ArgList...>>
    {
      static_assert(sizeof...(FuncArgList) == sizeof...(ArgList), "the number of supplied argument does not match the function's arity");

      constexpr _sub_function_wrapper(ArgList... values)
      
      {
      }
    };

    // /// ///
    // function traits
    // /// ///

    template<typename FunctionType>
    struct function_traits
    {
      static constexpr bool is_function = false;
    };
    template<typename FunctionType>
    constexpr bool function_traits<FunctionType>::is_function;

    // standard function
    template<typename Return, typename... Parameters>
    struct function_traits<Return (Parameters...)>
    {
      static constexpr bool is_function = true;
      static constexpr bool is_member = false;
      using return_type = Return;
      using ptr_type = Return (*)(Parameters...);
      using arg_list = type_list<Parameters...>;

      template<ptr_type ptr>
      using create_wrapper = function_wrapper<ptr_type, ptr, Return, /*Class,*/ arg_list>;
    };
    template<typename Return, typename... Parameters>
    constexpr bool function_traits<Return (Parameters...)>::is_function;
    template<typename Return, typename... Parameters>
    constexpr bool function_traits<Return (Parameters...)>::is_member;

    // class member function
    template<typename Return, typename Class,typename... Parameters>
    struct function_traits<Return (Class::*)(Parameters...)>
    {
      static constexpr bool is_function = true;
      static constexpr bool is_member = true;
      static constexpr bool is_const = false;
      static constexpr bool is_volatile = false;
      using return_type = Return;
      using class_type = Class;
      using ptr_type = Return (Class::*)(Parameters...);
      using arg_list = type_list<Parameters...>;
    };
    template<typename Return, typename Class, typename... Parameters>
    constexpr bool function_traits<Return (Class::*)(Parameters...)>::is_function;
    template<typename Return, typename Class, typename... Parameters>
    constexpr bool function_traits<Return (Class::*)(Parameters...)>::is_member;
    template<typename Return, typename Class, typename... Parameters>
    constexpr bool function_traits<Return (Class::*)(Parameters...)>::is_const;
    template<typename Return, typename Class, typename... Parameters>
    constexpr bool function_traits<Return (Class::*)(Parameters...)>::is_volatile;

    template<typename Return, typename Class, typename... Parameters>
    struct function_traits<Return (Class::*)(Parameters...) const>
    {
      static constexpr bool is_function = true;
      static constexpr bool is_member = true;
      static constexpr bool is_const = true;
      static constexpr bool is_volatile = false;
      using return_type = Return;
      using class_type = Class;
      using ptr_type = Return (Class::*)(Parameters...) const;
      using arg_list = type_list<Parameters...>;
    };
    template<typename Return, typename Class, typename... Parameters>
    constexpr bool function_traits<Return (Class::*)(Parameters...) const>::is_function;
    template<typename Return, typename Class, typename... Parameters>
    constexpr bool function_traits<Return (Class::*)(Parameters...) const>::is_member;
    template<typename Return, typename Class, typename... Parameters>
    constexpr bool function_traits<Return (Class::*)(Parameters...) const>::is_const;
    template<typename Return, typename Class, typename... Parameters>
    constexpr bool function_traits<Return (Class::*)(Parameters...) const>::is_volatile;

    template<typename Return, typename Class, typename... Parameters>
    struct function_traits<Return (Class::*)(Parameters...) volatile>
    {
      static constexpr bool is_function = true;
      static constexpr bool is_member = true;
      static constexpr bool is_const = false;
      static constexpr bool is_volatile = true;
      using return_type = Return;
      using class_type = Class;
      using ptr_type = Return (Class::*)(Parameters...) volatile;
      using arg_list = type_list<Parameters...>;
    };
    template<typename Return, typename Class, typename... Parameters>
    constexpr bool function_traits<Return (Class::*)(Parameters...) volatile>::is_function;
    template<typename Return, typename Class, typename... Parameters>
    constexpr bool function_traits<Return (Class::*)(Parameters...) volatile>::is_member;
    template<typename Return, typename Class, typename... Parameters>
    constexpr bool function_traits<Return (Class::*)(Parameters...) volatile>::is_const;
    template<typename Return, typename Class, typename... Parameters>
    constexpr bool function_traits<Return (Class::*)(Parameters...) volatile>::is_volatile;

    template<typename Return, typename Class, typename... Parameters>
    struct function_traits<Return (Class::*)(Parameters...) const volatile>
    {
      static constexpr bool is_function = true;
      static constexpr bool is_member = true;
      static constexpr bool is_const = false;
      static constexpr bool is_volatile = true;
      using return_type = Return;
      using class_type = Class;
      using ptr_type = Return (Class::*)(Parameters...) const volatile;
      using arg_list = type_list<Parameters...>;
    };
    template<typename Return, typename Class, typename... Parameters>
    constexpr bool function_traits<Return (Class::*)(Parameters...) const volatile>::is_function;
    template<typename Return, typename Class, typename... Parameters>
    constexpr bool function_traits<Return (Class::*)(Parameters...) const volatile>::is_member;
    template<typename Return, typename Class, typename... Parameters>
    constexpr bool function_traits<Return (Class::*)(Parameters...) const volatile>::is_const;
    template<typename Return, typename Class, typename... Parameters>
    constexpr bool function_traits<Return (Class::*)(Parameters...) const volatile>::is_volatile;


    

    
  } // namespace ct
} // namespace neam

#endif /*__N_2111737739159269973_1204098121__FUNCTION_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


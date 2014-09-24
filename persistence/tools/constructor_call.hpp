//
// file : constructor_call.hpp
// in : file:///home/tim/projects/nsched/nsched/tools/constructor_call.hpp
//
// created by : Timothée Feuillet on linux.site
// date: 24/07/2014 12:15:02
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

#ifndef __N_333063978185995303_2133399434__CONSTRUCTOR_CALL_HPP__
# define __N_333063978185995303_2133399434__CONSTRUCTOR_CALL_HPP__

//
// constructor::call: allow embedding object definitions in templates. (also: works recursively).
// exemple: my_class0<N_EMBED_OBJECT(my_class1, N_EMBED_OBJECT(my_class2, N_EMBED(1), N_EMBED(10), N_EMBED(1)), N_EMBED(13))> my_obj;
// for my_class1::my_class1(const my_class2 &, int) and my_class2::my_class2(int, int, double)
// also works for constexpr and non-constexpr constructors.
//

#include <new>
#include <memory>
#include <tools/ct_list.hpp>

namespace neam
{
  namespace ct
  {
    // allow to embed an object in a template
    // their is absolutely no overhead at runtime: the assembly code is *exactly* the same as if the constructor has been called directly.
    // to works with 'neam::embed', simply use the 'embed' nested class (or cr_embed if the class doesn't meet the constexpr requirement).
    template<typename ObjectType, typename... Values>
    struct constructor
    {
      struct call       // could be also used in place of 'embed' (when cast isn't needed)
      {
        constexpr call(){}

        // cast to ObjectType (by calling the constructor)
        inline constexpr operator ObjectType()
        {
          return ObjectType(Values::get()...);
        }

        // allow cast operators of ObjectType to works within this class
        template<typename T>
        inline constexpr operator T()
        {
          return static_cast<T>(ObjectType(Values::get()...));
        }

        // the prefered way to use this class :)
        constexpr static ObjectType get()
        {
          return ObjectType(Values::get()...);
        }

        using type = ObjectType;


        // defined if needed :) (again: no overhead if unused)
        // To use this, you MUST have a constexpr constructor.
        template<typename EmbeddedType>
        struct embed
        {
          constexpr embed() {}

          operator EmbeddedType &()
          {
            return value;
          }
          constexpr operator EmbeddedType()
          {
            return ObjectType(Values::get()...);
          }

          // the preferred way to use this class :)
          constexpr static EmbeddedType get()
          {
            return ObjectType(Values::get()...);
          }

          using type = EmbeddedType;

          static constexpr EmbeddedType value __attribute__((deprecated)) = ObjectType(Values::get()...);
        };

        // defined if needed :) (again: no overhead if unused)
        // To use this, you could have a non-constexpr constructor. (but ::value is NOT defined).
        template<typename EmbeddedType>
        struct cr_embed
        {
          constexpr cr_embed() {}

          inline operator EmbeddedType() const
          {
            return ObjectType(Values::get()...);
          }

          // the preferred way to use this class :)
          static inline EmbeddedType get()
          {
            return ObjectType(Values::get()...);
          }

          using type = EmbeddedType;

        };

        // use a placement new on an existing object
        struct placement
        {
          static inline void on(ObjectType *ptr)
          {
            new (ptr) ObjectType(Values::get()...);
          }
          using type = ObjectType;
        };
      };
    };

    // the same, but with a type_list<...> for arguments values
    template<typename ObjectType, typename... Types>
    struct constructor<ObjectType, type_list<Types...>> : public constructor<ObjectType, Types...> {};


// a macro to go with it. (it simply create the constructor<>::template call<> type from the value list).
// use it like this: neam::ct::N_CT_CONSTRUCTOR_CALL_TYPE(1, &this_variable).
// Doesn't works with strings (this won't work: neam::ct::N_CT_CONSTRUCTOR_CALL_TYPE("coucou")).
#define N_CT_CONSTRUCTOR_CALL_TYPE(ObjectType, ...)     neam::ct::constructor<ObjectType, neam::ct::type_list<__VA_ARGS__>>::call

// this one return directly the embed for the underlying type
#define N_CT_CONSTRUCTOR_CALL_EMBED(ObjectType, ...)    N_CT_CONSTRUCTOR_CALL_TYPE(ObjectType, ##__VA_ARGS__)::template embed<ObjectType>
// and for the cr_embed
#define N_CT_CONSTRUCTOR_CALL_CR_EMBED(ObjectType, ...) N_CT_CONSTRUCTOR_CALL_TYPE(ObjectType, ##__VA_ARGS__)::template cr_embed<ObjectType>
// and for the placement new
#define N_CT_CONSTRUCTOR_CALL_PLACEMENT(ObjectType, ...)    N_CT_CONSTRUCTOR_CALL_TYPE(ObjectType, ##__VA_ARGS__)::placement

// this one return directly the underlying type (the constructor::call casted to the underlying type)
#define N_CT_CONSTRUCTOR_CALL(ObjectType, ...)          static_cast<ObjectType>(N_CT_CONSTRUCTOR_CALL_TYPE(ObjectType, ##__VA_ARGS__)())

// for some similarity with neam::embed:
#define N_EMBED_OBJECT(ObjectType, ...)                 N_CT_CONSTRUCTOR_CALL_EMBED(ObjectType, ##__VA_ARGS__)
#define N_CR_EMBED_OBJECT(ObjectType, ...)              N_CT_CONSTRUCTOR_CALL_CR_EMBED(ObjectType, ##__VA_ARGS__)

// for simple usage with neam::cr::persistence
#define N_CALL_CONSTRUCTOR(ObjectType, ...)             N_CT_CONSTRUCTOR_CALL_PLACEMENT(ObjectType, ##__VA_ARGS__)

  } // namespace ct
} // namespace neam

#endif /*__N_333063978185995303_2133399434__CONSTRUCTOR_CALL_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


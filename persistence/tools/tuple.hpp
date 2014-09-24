//
// file : tuple.hpp
// in : file:///home/tim/projects/nkark/src/tuple.hpp
//
// created by : Timothée Feuillet on linux-coincoin.tim
// date: 11/08/2013 15:17:29
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


#ifndef __TUPLE_HPP__
# define __TUPLE_HPP__

#include <type_traits>
#include <utility>

#include <cstdint>

#include <tools/genseq.hpp>
#include <tools/bad_type.hpp>

namespace neam
{
  namespace cr
  {
    // some tuple utilities

    // when passed as argument to the constructor, the argument will be moved.
    template<typename Type>
    struct move
    {
      move(const move &o) : value(std::move(o.value)) {}
      move(move &&o) : value(std::move(o.value)) {}
      move(Type && o) : value(std::move(o)) {}
      move(Type &o) : value(std::move(o)) {}

      Type &&value;
    };

    template<typename Type>
    move<Type> make_move(Type &t)
    {
      return move<Type>(t);
    }

    // a constexpr tuple
    template<typename... Types>
    class tuple
    {
      private: // helper structs
        // out of range checks
        template<uint64_t Index, typename Type>
        struct out_of_range_check_type
        {
          static_assert(Index < sizeof...(Types), "index is out of range");
          using type = Type;
        };
        template<uint64_t Index>
        class out_of_range_check
        {
          static_assert(Index < sizeof...(Types), "index is out of range");
        };

        // retrieve type at index
        template<uint64_t Index, typename ThisType, typename... OtherTypes>
        struct get_type_at_index
        {
          out_of_range_check<Index> __oorc;
          using type = typename get_type_at_index<Index - 1, OtherTypes...>::type;
        };
        template<typename ThisType, typename... OtherTypes>
        struct get_type_at_index<0, ThisType, OtherTypes...>
        {
          out_of_range_check<0> __oorc;
          using type = ThisType;
        };

        // 'recursively' store values
        template<typename ThisType, typename... OtherTypes>
        struct store
        {
          constexpr store() : value(), next() {}
          constexpr store(const store &o) : value((o.value)), next((o.next)) {}
          constexpr store(store &&o) : value(std::move(o.value)), next(std::move(o.next)) {}

          constexpr store(ThisType o, OtherTypes... ot) : value(o), next(ot...) {}
          template<typename... ConstructOtherTypes>
          constexpr store(move<ThisType> &o, ConstructOtherTypes...ot) : value(std::move(o.value)), next(ot...) {}

          template<uint64_t Index, typename RetType>
          constexpr auto get() const -> typename std::enable_if<Index != 0, const RetType &>::type
          {
            return (next.template get<Index - 1, RetType>());
          }
          template<uint64_t Index, typename RetType>
          constexpr auto get() const -> typename std::enable_if<Index == 0, const RetType &>::type
          {
            return value;
          }

          template<uint64_t Index, typename RetType>
          auto get() -> typename std::enable_if<Index != 0, RetType &>::type
          {
            return (next.template get<Index - 1, RetType>());
          }
          template<uint64_t Index, typename RetType>
          auto get() -> typename std::enable_if<Index == 0, RetType &>::type
          {
            return value;
          }

          ThisType value;
          store<OtherTypes...> next;
        };

        template<typename ThisType>
        struct store<ThisType>
        {
          constexpr store() : value() {}
          constexpr store(const store &o) : value((o.value)) {}
          constexpr store(store &&o) : value(std::move(o.value)) {}

          constexpr store(ThisType o) : value(o) {}
          constexpr store(move<ThisType> &o) : value(std::move(o.value)) {}

          template<uint64_t Index, typename RetType>
          constexpr auto get() const -> typename std::enable_if<Index, const ThisType &>::type
          {
            static_assert(!Index, "index is out of range : BAD USAGE OF neam::cr::tuple !!!");
            return *(void *)0; // clang complain... but this won't seg anyway, as the code won't be built.
          }
          template<uint64_t Index, typename RetType>
          constexpr auto get() const -> typename std::enable_if<!Index, const ThisType &>::type
          {
            return value;
          }

          template<uint64_t Index, typename RetType>
          auto get() -> typename std::enable_if<Index, ThisType &>::type
          {
            static_assert(!Index, "index is out of range : BAD USAGE OF neam::cr::tuple !!!");
            return *(void *)0; // clang complain... but this won't seg anyway, as the code won't be built.
          }
          template<uint64_t Index, typename RetType>
          auto get() -> typename std::enable_if<!Index, ThisType &>::type
          {
            return value;
          }

          ThisType value;
        };

      public:
        constexpr tuple()
          : storage()
        {
        }

        template<uint64_t... Idx>
        constexpr tuple(const tuple &o) : storage(o.storage) {}

        template<uint64_t... Idx>
        constexpr tuple(tuple &&o) : storage(std::move(o.storage)) {}

        template<typename... OtherTypes>
        explicit constexpr tuple(OtherTypes... t) : storage(t...) {}

        template<uint64_t Index>
        constexpr auto get() const -> const typename out_of_range_check_type<Index, typename get_type_at_index<Index, Types...>::type>::type &
        {
          return storage.template get<Index, const typename out_of_range_check_type<Index, typename get_type_at_index<Index, Types...>::type>::type &>();
        }

        template<uint64_t Index>
        auto get() -> typename out_of_range_check_type<Index, typename get_type_at_index<Index, Types...>::type>::type &
        {
          return storage.template get<Index, typename out_of_range_check_type<Index, typename get_type_at_index<Index, Types...>::type>::type &>();
        }

        constexpr static size_t size()
        {
          return sizeof...(Types);
        }

      private:
        store<Types...> storage;
    };

    // an empty tuple
    template<>
    class tuple<>
    {
      public:
        constexpr tuple() {}
        constexpr tuple(const tuple &) {}
        constexpr tuple(tuple &&) {}

        template<size_t Index>
        constexpr cr::bad_type get() const
        {
          static_assert(!(Index + 1), "tuple<>::get() on an empty tuple.");
          return cr::bad_type(cr::bad_type::construct_from_return());
        }

        constexpr static size_t size()
        {
          return 0;
        }
    };

    // and a motherfucking constexpr tuple builder
    template<typename... Args>
    tuple<Args...> make_tuple(Args... _args)
    {
      return tuple<typename std::remove_reference<Args>::type...>((_args)...);
    }

  } // namespace cr
} // namespace neam

#endif /*__TUPLE_HPP__*/

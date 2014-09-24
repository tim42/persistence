//
// file : comparison.hpp
// in : file:///home/tim/projects/yaggler/yaggler/tools/comparison.hpp
//
// created by : Timothée Feuillet on linux-coincoin.tim
// date: 04/04/2014 12:42:30
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

#ifndef __N_180166067663833477_2096411615__COMPARISON_HPP__
# define __N_180166067663833477_2096411615__COMPARISON_HPP__

#include <cstddef>
#include <tools/tuple.hpp>
#include <tools/execute_pack.hpp>

namespace neam
{
  namespace cr
  {
    // this is a great horror story for any coder :)
    namespace comparison
    {
      // the base class. Each comparison operator returns the number of 'true' values.
      template<typename... Types>
      class _comparator
      {
        public:
          _comparator(Types... t) : values(t...) {}

          template<typename O>
          size_t operator < (const O &o) const
          {
            return operator_lt(o, _seq);
          }

          template<typename O>
          size_t operator <= (const O &o) const
          {
            return operator_lte(o, _seq);
          }

          template<typename O>
          size_t operator > (const O &o) const
          {
            return operator_gt(o, _seq);
          }

          template<typename O>
          size_t operator >= (const O &o) const
          {
            return operator_gte(o, _seq);
          }

          template<typename O>
          size_t operator != (const O &o) const
          {
            return operator_ne(o, _seq);
          }

          template<typename O>
          size_t operator == (const O &o) const
          {
            return operator_eq(o, _seq);
          }

          size_t operator ! () const
          {
            return operator_not(_seq);
          }

          // return the number of 'true' elements
          operator size_t() const
          {
            return operator_lt(_seq);
          }


        public:
          const cr::tuple<Types...> values;

        protected:
          inline void inc_if_non_null(bool op, size_t &r) const
          {
            if (op)
              ++r;
          }

          template<typename O, size_t... Idxs>
          size_t operator_lt (const O &o, cr::seq<Idxs...>) const
          {
            size_t ret = 0;
            NEAM_EXECUTE_PACK(inc_if_non_null(values.template get<Idxs>() < o, ret));
            return ret;
          }

          template<typename O, size_t... Idxs>
          size_t operator_lte (const O &o, cr::seq<Idxs...>) const
          {
            size_t ret = 0;
            NEAM_EXECUTE_PACK(inc_if_non_null(values.template get<Idxs>() <= o, ret));
            return ret;
          }

          template<typename O, size_t... Idxs>
          size_t operator_gt (const O &o, cr::seq<Idxs...>) const
          {
            size_t ret = 0;
            NEAM_EXECUTE_PACK(inc_if_non_null(values.template get<Idxs>() > o, ret));
            return ret;
          }

          template<typename O, size_t... Idxs>
          size_t operator_gte (const O &o, cr::seq<Idxs...>) const
          {
            size_t ret = 0;
            NEAM_EXECUTE_PACK(inc_if_non_null(values.template get<Idxs>() >= o, ret));
            return ret;
          }

          template<typename O, size_t... Idxs>
          size_t operator_ne (const O &o, cr::seq<Idxs...>) const
          {
            size_t ret = 0;
            NEAM_EXECUTE_PACK(inc_if_non_null(values.template get<Idxs>() != o, ret));
            return ret;
          }

          template<typename O, size_t... Idxs>
          size_t operator_eq (const O &o, cr::seq<Idxs...>) const
          {
            size_t ret = 0;
            NEAM_EXECUTE_PACK(inc_if_non_null(values.template get<Idxs>() == o, ret));
            return ret;
          }

          template<size_t... Idxs>
          size_t operator_not (cr::seq<Idxs...>) const
          {
            size_t ret = 0;
            NEAM_EXECUTE_PACK(inc_if_non_null(!values.template get<Idxs>(), ret));
            return ret;
          }

          template<size_t... Idxs>
          size_t operator_size_t (cr::seq<Idxs...>) const
          {
            size_t ret = 0;
            NEAM_EXECUTE_PACK(inc_if_non_null(values.template get<Idxs>(), ret));
            return ret;
          }

        protected:
          cr::gen_seq<sizeof...(Types)> _seq;
      };

      template<typename... Types>
      class any : public _comparator<Types...>
      {
        public:
          any(Types... t) : _comparator<Types...>(t...) {}

          template<typename O>
          bool operator < (const O &o) const
          {
            return _comparator<Types...>::operator_lt(o, _seq);
          }

          template<typename O>
          bool operator <= (const O &o) const
          {
            return _comparator<Types...>::operator_lte(o, _seq);
          }

          template<typename O>
          bool operator > (const O &o) const
          {
            return _comparator<Types...>::operator_gt(o, _seq);
          }

          template<typename O>
          bool operator >= (const O &o) const
          {
            return _comparator<Types...>::operator_gte(o, _seq);
          }

          template<typename O>
          bool operator != (const O &o) const
          {
            return _comparator<Types...>::operator_ne(o, _seq);
          }

          template<typename O>
          bool operator == (const O &o) const
          {
            return _comparator<Types...>::operator_eq(o, _seq);
          }

          bool operator ! () const
          {
            return _comparator<Types...>::operator_not(_seq);
          }

          operator bool() const
          {
            return _comparator<Types...>::operator_size_t(_seq);
          }

        protected:
          cr::gen_seq<sizeof...(Types)> _seq;
      };

      // none is !any
      template<typename... Types>
      class none : public _comparator<Types...>
      {
        public:
          none(Types... t) : _comparator<Types...>(t...) {}

          template<typename O>
          bool operator < (const O &o) const
          {
            return !_comparator<Types...>::operator_lt(o, _seq);
          }

          template<typename O>
          bool operator <= (const O &o) const
          {
            return !_comparator<Types...>::operator_lte(o, _seq);
          }

          template<typename O>
          bool operator > (const O &o) const
          {
            return !_comparator<Types...>::operator_gt(o, _seq);
          }

          template<typename O>
          bool operator >= (const O &o) const
          {
            return !_comparator<Types...>::operator_gte(o, _seq);
          }

          template<typename O>
          bool operator != (const O &o) const
          {
            return !_comparator<Types...>::operator_ne(o, _seq);
          }

          template<typename O>
          bool operator == (const O &o) const
          {
            return !_comparator<Types...>::operator_eq(o, _seq);
          }

          bool operator ! () const
          {
            return !_comparator<Types...>::operator_not(_seq);
          }

          operator bool() const
          {
            return !_comparator<Types...>::operator_size_t(_seq);
          }

        protected:
          cr::gen_seq<sizeof...(Types)> _seq;
      };

      template<typename... Types>
      class all : public _comparator<Types...>
      {
        public:
          all(Types... t) : _comparator<Types...>(t...) {}

          template<typename O>
          bool operator < (const O &o) const
          {
            return _comparator<Types...>::operator_lt(o, _seq) == sizeof...(Types);
          }

          template<typename O>
          bool operator <= (const O &o) const
          {
            return _comparator<Types...>::operator_lte(o, _seq) == sizeof...(Types);
          }

          template<typename O>
          bool operator > (const O &o) const
          {
            return _comparator<Types...>::operator_gt(o, _seq) == sizeof...(Types);
          }

          template<typename O>
          bool operator >= (const O &o) const
          {
            return _comparator<Types...>::operator_gte(o, _seq) == sizeof...(Types);
          }

          template<typename O>
          bool operator != (const O &o) const
          {
            return _comparator<Types...>::operator_ne(o, _seq) == sizeof...(Types);
          }

          template<typename O>
          bool operator == (const O &o) const
          {
            return _comparator<Types...>::operator_eq(o, _seq) == sizeof...(Types);
          }

          bool operator ! () const
          {
            return _comparator<Types...>::operator_not(_seq) == sizeof...(Types);
          }

          operator bool() const
          {
            return _comparator<Types...>::operator_size_t(_seq) == sizeof...(Types);
          }

        protected:
            cr::gen_seq<sizeof...(Types)> _seq;
      };

      // most is true when the number 'true' is > of the number of elements / 2
      template<typename... Types>
      class most : public _comparator<Types...>
      {
        public:
          most(Types... t) : _comparator<Types...>(t...) {}

          template<typename O>
          bool operator < (const O &o) const
          {
            return _comparator<Types...>::operator_lt(o, _seq) > (sizeof...(Types) / 2);
          }

          template<typename O>
          bool operator <= (const O &o) const
          {
            return _comparator<Types...>::operator_lte(o, _seq) > (sizeof...(Types) / 2);
          }

          template<typename O>
          bool operator > (const O &o) const
          {
            return _comparator<Types...>::operator_gt(o, _seq) > (sizeof...(Types) / 2);
          }

          template<typename O>
          bool operator >= (const O &o) const
          {
            return _comparator<Types...>::operator_gte(o, _seq) > (sizeof...(Types) / 2);
          }

          template<typename O>
          bool operator != (const O &o) const
          {
            return _comparator<Types...>::operator_ne(o, _seq) > (sizeof...(Types) / 2);
          }

          template<typename O>
          bool operator == (const O &o) const
          {
            return _comparator<Types...>::operator_eq(o, _seq) > (sizeof...(Types) / 2);
          }

          bool operator ! () const
          {
            return _comparator<Types...>::operator_not(_seq) > (sizeof...(Types) / 2);
          }

          operator bool() const
          {
            return _comparator<Types...>::operator_size_t(_seq) > (sizeof...(Types) / 2);
          }

        protected:
          cr::gen_seq<sizeof...(Types)> _seq;
      };

      // some is true when the number 'true' is > than 'num'.
      template<typename... Types>
      class some : public _comparator<Types...>
      {
        public:
          some(size_t _num, Types... t) : _comparator<Types...>(t...), num(_num) {}

          template<typename O>
          bool operator < (const O &o) const
          {
            return _comparator<Types...>::operator_lt(o, _seq) > num;
          }

          template<typename O>
          bool operator <= (const O &o) const
          {
            return _comparator<Types...>::operator_lte(o, _seq) > num;
          }

          template<typename O>
          bool operator > (const O &o) const
          {
            return _comparator<Types...>::operator_gt(o, _seq) > num;
          }

          template<typename O>
          bool operator >= (const O &o) const
          {
            return _comparator<Types...>::operator_gte(o, _seq) > num;
          }

          template<typename O>
          bool operator != (const O &o) const
          {
            return _comparator<Types...>::operator_ne(o, _seq) > num;
          }

          template<typename O>
          bool operator == (const O &o) const
          {
            return _comparator<Types...>::operator_eq(o, _seq) > num;
          }

          bool operator ! () const
          {
            return _comparator<Types...>::operator_not(_seq) > num;
          }

          operator bool() const
          {
            return _comparator<Types...>::operator_size_t(_seq) > num;
          }

        protected:
          cr::gen_seq<sizeof...(Types)> _seq;
          size_t num;
      };

      // helpers
      template<typename... Types>
      any<Types...> any_of(const Types &... t)
      {
        return any<Types...>(t...);
      }

      template<typename... Types>
      none<Types...> none_of(const Types &... t)
      {
        return none<Types...>(t...);
      }

      template<typename... Types>
      all<Types...> all_of(const Types &... t)
      {
        return all<Types...>(t...);
      }

      template<typename... Types>
      most<Types...> most_of(const Types &... t)
      {
        return most<Types...>(t...);
      }

      template<typename... Types>
      some<Types...> some_of(size_t count, const Types &... t)
      {
        return some<Types...>(count, t...);
      }
      template<size_t Count, typename... Types>
      some<Types...> some_of(const Types &... t)
      {
        return some<Types...>(Count, t...);
      }

      // usage: (count_of(1, 2, 3) > 1) < 2
      // --> "the count of elements in [1, 2, 3] that are greter than 1 is less than two".
      template<typename... Types>
      _comparator<Types...> count_of(const Types &... t)
      {
        return _comparator<Types...>(t...);
      }
    } // namespace comparison
  } // namespace cr
} // namespace neam

#endif /*__N_180166067663833477_2096411615__COMPARISON_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


// cr: a c++ template parser
// Copyright (C) 2012-2013-2014  Timothée Feuillet
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

#include <initializer_list>
#include <type_traits>
#include <utility>
#include <memory>
#include <vector>
#include <stdexcept>

#ifndef __CR_STRING_HPP__
#define __CR_STRING_HPP__

#define _SL_STRINGIZE(x) #x
#define STRINGIZE(x) _SL_STRINGIZE(x)

namespace neam
{
  // use this as 'constexpr string_t myvar = "blablabla";'
  using string_t = char[];

  namespace ct
  {

    static constexpr size_t npos = static_cast<size_t>(-1);

    // strlen function
    constexpr size_t strlen(const char *str)
    {
      return str[0] ? ct::strlen(str + 1) + 1 : 0;
    }

    // Compile time string class
    // support substring, comparison,
    // casting to char (Except when it's a substring, and when the end is < the length of the string)
    // length computation,
    template <const char *Str, size_t Start = 0, size_t End = npos>
    class string
    {
      private: // helpers
        // comparison helpers
        template<const char *const S1, const char *const S2, size_t Pos = Start>
        inline static constexpr typename std::enable_if < S1[Pos] != S2[Pos], bool >::type str_is_same()
        {
          return false;
        }
        template<const char *const S1, const char *const S2, size_t Pos = Start>
        inline static constexpr typename std::enable_if < S1[Pos] == S2[Pos] && (S1[Pos] == 0 || S2[Pos] == 0 || Pos == End), bool >::type str_is_same()
        {
          return true;
        }
        template<const char *const S1, const char *const S2, size_t Pos = Start>
        inline static constexpr typename std::enable_if < S1[Pos] == S2[Pos] && S1[Pos] != 0 && S2[Pos] != 0 && Pos != End, bool >::type str_is_same()
        {
          return str_is_same < S1, S2, Pos + 1 > ();
        }

      public:
        constexpr string() {}
        ~string() {}

        // allow casting back the class to a const char *
        constexpr operator const char *() const
        {
          return value + Start;
        }

        // comparison operators
        template<const char *OStr>
        constexpr bool operator == (const string<OStr> &) const
        {
          return str_is_same<Str, OStr>();
        }
        template<const char *OStr>
        constexpr bool operator != (const string<OStr> &other) const
        {
          return !(*this == other);
        }

        constexpr char operator[](size_t pos) const
        {
          return pos + Start < End ? Str[pos + Start] : 0;
        }

      private: // helpers
        // length computation
        template<size_t Pos>
        constexpr static typename std::enable_if<Str[Pos], size_t>::type _compute_length()
        {
          return _compute_length < Pos + 1 > () + 1;
        }
        template<size_t Pos>
        constexpr static typename std::enable_if < !Str[Pos], size_t >::type _compute_length()
        {
          return 0;
        }
        constexpr static size_t compute_length()
        {
          return _compute_length<0>();
        }

      public:
        static constexpr const char *value = Str + Start; // data
        static constexpr size_t length = compute_length(); // length of the string
        static constexpr size_t start_index = Start;
        static constexpr size_t end_index = End;
    };

    // string set (could also be a string set of string AND string sets)
    // and everytypes that define the used operators and static vars constexpr
    // they are:
    //  - length (type: size_t)
    //  - char operator[](size_t)
    //  - a default constructor
#ifndef __clang__
    template<size_t Start, size_t End, typename... StringSet>
    class string_set
    {
      private: //helpers
        // operator [] helper
        template <size_t Unused>
        static constexpr char get_char_at_index(size_t)
        {
          return (0);
        }
        template <size_t Unused, typename CurrentStr, typename... Others>
        static constexpr char get_char_at_index(size_t index)
        {
          return (index > End ? 0 : (index < CurrentStr::length ? ((CurrentStr()) [index]) : (get_char_at_index<0, Others...> (index - CurrentStr::length))));
        }

        // comparison func
        template<typename T>
        constexpr bool is_same(const T &o, size_t index = 0) const
        {
          return ((*this)[index] == o[index] ? ((*this)[index] ? is_same(o, index + 1) : true) : false);
        }

      public: // funcs
        constexpr string_set() {}
        ~string_set() {}

        constexpr char operator[](size_t index) const
        {
          return get_char_at_index<0, StringSet...>(index + Start);
        }

        template<typename T>
        constexpr bool operator == (const T &o) const
        {
          return is_same(o);
        }
        template<typename T>
        constexpr bool operator != (const T &o) const
        {
          return !(*this == o);
        }

      private: //helpers
        template <size_t Unused>
        static constexpr size_t compute_length()
        {
          return 0;
        }
        template <size_t Unused, typename CurrentStr, typename... Others>
        static constexpr size_t compute_length()
        {
          return CurrentStr::length + compute_length<0, Others...>();
        }
        // uniform length computation for substrings
        static constexpr size_t length_recalc(size_t len)
        {
          return (len < Start ? 0 : (len > End ? (End - Start) : (len - Start)));
        }

      public: // vars
        static constexpr size_t length = length_recalc(compute_length<Start, StringSet...>()); // length of the strings
        static constexpr size_t start_index = Start;
        static constexpr size_t end_index = End;
    };

    // substring operation
    // for strings
    template <size_t NewStart = 0, size_t NewEnd = npos, const char *Str, size_t Start, size_t End>
    constexpr auto substring(const string<Str, Start, End> &) -> string<Str, NewStart, NewEnd>
    {
      return string<Str, NewStart, NewEnd>();
    }
    // for string_sets
    template <size_t NewStart = 0, size_t NewEnd = npos, size_t Start, size_t End, typename... Type>
    constexpr auto substring(const string_set<Start, End, Type...> &) -> string_set<NewStart, NewEnd, Type...>
    {
      return string_set<NewStart, NewEnd, Type...>();
    }

    // operators +

    // string + string
    template <const char *Str1, size_t Start1, size_t End1, const char *Str2, size_t Start2, size_t End2>
    constexpr string_set<0, npos, string<Str1, Start1, End1>, string<Str1, Start1, End1>> operator +(const string<Str1, Start1, End1> &, const string<Str2, Start2, End2> &)
    {
      return string_set<0, npos, string<Str1, Start1, End1>, string<Str1, Start1, End1>>();
    }

    // string + string_set
    template <const char *Str1, size_t Start1, size_t End1, typename... Type2>
    constexpr string_set<0, npos, string<Str1, Start1, End1>, string_set<Type2...>> operator +(const string<Str1, Start1, End1> &, const string_set<Type2...> &)
    {
      return string_set<0, npos, string<Str1, Start1, End1>, string_set<Type2...>>();
    }

    // string_set + string
    template <const char *Str1, size_t Start1, size_t End1, typename... Type2>
    constexpr string_set<0, npos, string_set<Type2...>, string<Str1, Start1, End1>> operator +(const string_set<Type2...> &, const string<Str1, Start1, End1> &)
    {
      return string_set<0, npos, string_set<Type2...>, string<Str1, Start1, End1>>();
    }

    // string_set + string_set
    template <typename... Type1, typename... Type2>
    constexpr string_set<0, npos, string_set<Type1...>, string_set<Type2...>> operator +(const string_set<Type1...> &, const string_set<Type2...> &)
    {
      return string_set<0, npos, string_set<Type1...>, string_set<Type2...>>();
    }
#endif /*__clang__*/
  } // namespace ct
} // namespace neam

#endif /*__CR_STRING_HPP__*/

//
// file : serializable_specs_json_internal.hpp
// in : file:///home/tim/projects/persistence/persistence/serializable_specs_json_internal.hpp
//
// created by : Timothée Feuillet on linux-vnd3.site
// date: 07/01/2016 17:54:41
//
//
// Copyright (c) 2014-2016 Timothée Feuillet
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef __N_7078393271892028266_731342187__SERIALIZABLE_SPECS_JSON_INTERNAL_HPP__
# define __N_7078393271892028266_731342187__SERIALIZABLE_SPECS_JSON_INTERNAL_HPP__

#include <type_traits>
#include <string>
#include <cstdlib>
#include <cstdint>
#include <climits>
#include <limits>
#include <cmath>

#include "../tools/array_wrapper.hpp"
#include "../object.hpp" // for my IDE
#include "../raw_data.hpp"

#include "json_string_utilities.hpp"

namespace neam
{
  namespace cr
  {
    namespace internal
    {
      namespace json
      {
        // // deserialization stuff

        // I have to convert things[8-16-32-64-bit integers/double/float] from string
        // but using the same function name (why on the earth this doesn't exists in the standard ?)
        static inline bool number_from_string(const char *number, int64_t &dest)
        {
          static_assert((sizeof(int64_t) <= sizeof(long long)), "Unable to convert 64bit numbers from strings");
          char *end;
          if (sizeof(int64_t) <= sizeof(long))
            dest = std::strtol(number, &end, 10);
          else if (sizeof(int64_t) <= sizeof(long long))
            dest = std::strtoll(number, &end, 10);
          return dest != LLONG_MAX && dest != LONG_MAX;
        }
        static inline bool number_from_string(const char *number, uint64_t &dest)
        {
          static_assert((sizeof(int64_t) <= sizeof(long long)), "Unable to convert 64bit numbers from strings");
          char *end;
          if (sizeof(int64_t) <= sizeof(long))
            dest = std::strtoul(number, &end, 10);
          else if (sizeof(int64_t) <= sizeof(long long))
            dest = std::strtoull(number, &end, 10);
          return dest != ULLONG_MAX && dest != ULONG_MAX;
        }

        static inline bool number_from_string(const char *number, float &dest)
        {
          char *end;
          dest = std::strtof(number, &end);
          return dest != HUGE_VALF;
        }
        static inline bool number_from_string(const char *number, double &dest)
        {
          char *end;
          dest = std::strtod(number, &end);
          return dest != HUGE_VAL;
        }

        // And now all the possible derivations
        static inline bool number_from_string(const char *number, uint8_t &dest)
        {
          uint64_t td;
          if (!number_from_string(number, td) || td > std::numeric_limits<uint8_t>::max())
            return false;
          dest = td;
          return true;
        }
        static inline bool number_from_string(const char *number, int8_t &dest)
        {
          int64_t td;
          if (!number_from_string(number, td) || td > std::numeric_limits<int8_t>::max())
            return false;
          dest = td;
          return true;
        }
        static inline bool number_from_string(const char *number, uint16_t &dest)
        {
          uint64_t td;
          if (!number_from_string(number, td) || td > std::numeric_limits<uint16_t>::max())
            return false;
          dest = td;
          return true;
        }
        static inline bool number_from_string(const char *number, int16_t &dest)
        {
          int64_t td;
          if (!number_from_string(number, td) || td > std::numeric_limits<int16_t>::max())
            return false;
          dest = td;
          return true;
        }
        static inline bool number_from_string(const char *number, uint32_t &dest)
        {
          uint64_t td;
          if (!number_from_string(number, td) || td > std::numeric_limits<uint32_t>::max())
            return false;
          dest = td;
          return true;
        }
        static inline bool number_from_string(const char *number, int32_t &dest)
        {
          int64_t td;
          if (!number_from_string(number, td) || td > std::numeric_limits<int32_t>::max())
            return false;
          dest = td;
          return true;
        }

        enum class types
        {
          other = 0, // true, false, null
          list = 1,
          collection = 2,
          number = 3,
          string = 4,
          bad_type = 0xFFFFFF
        };

        // Return a possible type from its inital character
        // Luckily the JSON format propose only a few different possibilities
        // and each ones have at most a few unique character
        static inline types get_type(char t)
        {
          switch (t)
          {
            case '[':
              return types::list;
            case '{':
              return types::collection;
            case '"':
              return types::string;
            case 't': // true
            case 'f': // false
            case 'n': // null
              return types::other;
            case '-': case '0': case '1': case '2':
            case '3': case '4': case '5': case '6':
            case '7': case '8': case '9':
              return types::number;
          }
          return types::bad_type;
        }

        // Advance to the next entry
        static inline bool advance_next(const char *memory, size_t max_size, size_t &index, char expect = 0)
        {
          switch (memory[index])
          {
            case ':': if (expect == ':') expect = 0;
            case ',': if (expect == ',') expect = 0;
            case '"':
              ++index;
          }

          for (; index < max_size; ++index)
          {
            if (std::isspace(memory[index]))
              continue;
            if (memory[index] == expect && expect)
            {
              expect = 0;
              continue;
            }
            switch (memory[index])
            {
              case '}': case ']': // skip ending tokens
                continue;
            }
            return !expect;
          }
          return false;
        }

        static inline bool get_element_end_index(const char *memory, size_t max_size, size_t &index, types requested_type)
        {
          if (requested_type == types::bad_type)
            return false;

          bool in_qm = (memory[index] == '"'); // is inside quotation marks
          bool escaped = false; // if inside quotation marks, is last character an escape character
          size_t bracket_count = (memory[index] == '[' ? 1 : 0);
          size_t braces_count = (memory[index] == '{' ? 1 : 0);

//           if (!in_qm && requested_type == types::string)
//             return false;
//           if (!braces_count && requested_type == types::collection)
//             return false;
//           if (!bracket_count && requested_type == types::list)
//             return false;

          for (++index; index < max_size; ++index)
          {
            bool old_escaped = escaped;
            switch (memory[index])
            {
              // handle quotation marks
              case '\\':
                if (!escaped && in_qm)
                  escaped = true;
                break;
              case '"':
                if (!in_qm)
                  in_qm = true;
                else if (!escaped)
                {
                  in_qm = false;
                  if (requested_type == types::string) // easy case: the quotation mark can't be nested
                    return true;
                }
                break;

              // handle brackets
              case '{':
                if (!in_qm)
                  ++braces_count;
                break;
              case '[':
                if (!in_qm)
                  ++bracket_count;
                break;
              case '}':
                if (!in_qm)
                {
                  if (requested_type == types::other || requested_type == types::number) // can't be nested
                    return true;
                  --braces_count;
                  if (!bracket_count && !braces_count && requested_type == types::collection)
                    return true;
                }
                break;
              case ']':
                if (!in_qm)
                {
                  if (requested_type == types::other || requested_type == types::number) // can't be nested
                    return true;
                  --bracket_count;
                  if (!bracket_count && !braces_count && requested_type == types::list)
                    return true;
                }
                break;
              default:
                if (!in_qm && !bracket_count && !braces_count)
                {
                  if ((memory[index] == ',' || std::isspace(memory[index])) &&
                      (requested_type == types::number || requested_type == types::other))
                    return true;
                }
                break;
            }

            // reset the escaped flag
            if (old_escaped == escaped)
              escaped = false;
          }
          return false;
        }
      } // namespace json
    } // namespace internal
  } // namespace cr
} // namespace neam

#endif /*__N_7078393271892028266_731342187__SERIALIZABLE_SPECS_JSON_INTERNAL_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


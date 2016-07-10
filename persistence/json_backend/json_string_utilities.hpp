//
// file : json_string_utilities.hpp
// in : file:///home/tim/projects/persistence/persistence/json_backend/json_string_utilities.hpp
//
// created by : Timothée Feuillet on linux-vnd3.site
// date: 09/01/2016 13:17:20
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
//

#ifndef __N_10778411561649808392_1269877289__JSON_STRING_UTILITIES_HPP__
# define __N_10778411561649808392_1269877289__JSON_STRING_UTILITIES_HPP__

#include <type_traits>
#include <string>
#include <cstdlib>
#include <cstdint>

// This file is mostly serialization oriented
//  With a bit of deserialization

namespace neam
{
  namespace cr
  {
    namespace internal
    {
      namespace json
      {
        static char hex_alphabet[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
        static inline std::string escape_string(const std::string &s)
        {
          std::string res;
          res.reserve(s.size());
          for (size_t i = 0; i < s.size(); ++i)
          {
            if (((std::isgraph(s[i]) || s[i] >= ' ') && s[i] != '"' && s[i] != '\\') || s[i] == ' ')
              res += s[i];
            else if (s[i] != '"' && s[i] != '\\')
            {
              switch (s[i])
              {
                case '\b': res += "\\b"; break;
                case '\f': res += "\\f"; break;
                case '\n': res += "\\n"; break;
                case '\r': res += "\\r"; break;
                case '\t': res += "\\t"; break;
                default:
                  res += "\\u00";
                  res += internal::json::hex_alphabet[(s[i] & 0xF0) >> 4];
                  res += internal::json::hex_alphabet[s[i] & 0x0F];
              }
            }
            else
            {
              res += "\\";
              res += s[i];
            }
          }
          return res;
        }

        static inline std::string unescape_string(const char *s, size_t size)
        {
          std::string res;
          res.reserve(size);
          for (size_t i = 0; i < size; ++i)
          {
            if (s[i] == '\\' && i + 1 < size)
            {
              ++i;
              switch (s[i])
              {
                case '\\': case '"': res += s[i];break;
                case 'b': res += '\b'; break;
                case 'f': res += '\f'; break;
                case 'n': res += '\n'; break;
                case 'r': res += '\r'; break;
                case 't': res += '\t'; break;
                case 'u':
                  ++i;
                  if (i + 3 >= size)
                    return res;
                  char data[5] = {res[i], res[i + 1], res[1 + 2], res[1 + 3], 0};
                  char *_u = data;
                  uint16_t t = strtoul(data, &_u, 16);
                  if (t & 0xFF00)
                    res += char(t >> 8);
                  res += char(t & 0xFF);
                  i += 3;
                  break;
              }
            }
            else
              res += s[i];
          }
          return res;
        }

        static inline bool _allocate_string(memory_allocator &mem, size_t &size, size_t indent_level, const std::string &data)
        {
          void *d = mem.allocate(indent_level * 2 + data.size());

          if (d)
          {
            size_t i = 0;
            for (; i < indent_level * 2; ++i) ((char *)d)[i] = ' ';

            memcpy((char *)d + i, data.data(), data.size());
            size += indent_level * 2 + data.size();
            return true;
          }
          return false;
        }

        static inline bool _allocate_format_string(memory_allocator &mem, size_t &size, size_t indent_level, const char *name, const std::string &data)
        {
          std::string res;
          if (name)
            res = "\"" + escape_string(name) + "\" : ";
          res += data;

          return _allocate_string(mem, size, indent_level, res);
        }
      } // namespace json
    } // namespace internal
  } // namespace cr
} // namespace neam

#endif /*__N_10778411561649808392_1269877289__JSON_STRING_UTILITIES_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


//
// file : json_string_utilities.hpp
// in : file:///home/tim/projects/persistence/persistence/json_backend/json_string_utilities.hpp
//
// created by : Timothée Feuillet on linux-vnd3.site
// date: 09/01/2016 13:17:20
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


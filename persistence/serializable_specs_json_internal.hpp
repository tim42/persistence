//
// file : serializable_specs_json_internal.hpp
// in : file:///home/tim/projects/persistence/persistence/serializable_specs_json_internal.hpp
//
// created by : Timothée Feuillet on linux-vnd3.site
// date: 07/01/2016 17:54:41
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

#ifndef __N_7078393271892028266_731342187__SERIALIZABLE_SPECS_JSON_INTERNAL_HPP__
# define __N_7078393271892028266_731342187__SERIALIZABLE_SPECS_JSON_INTERNAL_HPP__

#include <type_traits>
#include <string>
#include <cstdlib>
#include <cstdint>
#include <climits>
#include <limits>
#include <cmath>

#include <tools/array_wrapper.hpp>
#include "object.hpp" // for my IDE
#include "raw_data.hpp"

namespace neam
{
  namespace cr
  {
    namespace internal
    {
      namespace json
      {
        // // serialization things

        template<typename Type> struct is_string_key : public std::false_type {};

        template<> struct is_string_key<const char *> : public std::true_type {};
        template<> struct is_string_key<char *> : public std::true_type {};
        template<> struct is_string_key<char *const> : public std::true_type {};
        template<> struct is_string_key<const char *const> : public std::true_type {};
        template<typename CharT, typename Traits, typename Alloc> struct is_string_key<std::basic_string<CharT, Traits, Alloc>> : public std::true_type {};
        template<typename CharT, typename Traits, typename Alloc> struct is_string_key<const std::basic_string<CharT, Traits, Alloc>> : public std::true_type {};

        static char hex_alphabet[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
        static inline std::string escape_string(const std::string &s)
        {
          std::string res;
          res.reserve(s.size());
          for (size_t i = 0; i < s.size(); ++i)
          {
            if ((std::isgraph(s[i]) && s[i] != '"' && s[i] != '\\') || s[i] == ' ')
              res += s[i];
            else if (s[i] != '"' && s[i] != '\\')
            {
              res += "\\u";
              res += "00";
              res += internal::json::hex_alphabet[(s[i] & 0xF0) >> 4];
              res += internal::json::hex_alphabet[s[i] & 0x0F];
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


        /// \brief A simple unserializer for key/value elements.
        /// The Child class have to provide the following methods (as static)
        ///   bool initialize_object(cr::allocation_transaction &transaction, Type *ptr) // --> called at the very beginning of the operation
        ///   bool push_entry(cr::allocation_transaction &transaction, Type *ptr, std::pair< Key, Value >* entry) // --> called for each element to push
        template<typename Child, typename Key, typename Value>
        class collection_list_unserializer
        {
          public:
            /// \brief deserialize the object
            /// \param[in] memory the serialized object
            /// \param[in] size the size of the memory area
            /// \param[out] ptr a pointer to the object (the one that the function will fill)
            /// \return true if successful
            template<typename Type>
            static inline bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, Type *ptr)
            {
              internal::json::types type = internal::json::get_type(memory[0]);
              if (type == internal::json::types::collection)
                return from_memory_collection(transaction, memory, size, ptr);
              else if (type == internal::json::types::list)
                return from_memory_list(transaction, memory, size, ptr);
              return false;
            }


          private:
            /// \brief Handle the collection mode of the std::map
            template<typename Type>
            static inline bool from_memory_collection(cr::allocation_transaction &transaction, const char *memory, size_t size, Type *ptr)
            {
              --size;   // skip the closing brace
              ++memory; // skip the opening brace

              new(ptr) std::map<Key, Value>();
              transaction.register_destructor_call_on_failure(ptr);

              // temporary transaction
              cr::allocation_transaction temp_transaction;

              // I know that this is unforgivable, but I want to have an object without dynamic allocation and without calling its constructor
              int8_t temp_memory[sizeof(std::pair<Key, Value>)];
              std::pair<Key, Value> *pair = reinterpret_cast<std::pair<Key, Value>*>(temp_memory);

              size_t index = 0;
              if (memory[0] == '"' || internal::json::advance_next(memory, size, index))
              {
                while (true)
                {
                  // retrieve the string index
                  if (internal::json::get_type(memory[index]) != internal::json::types::string)
                    return false;
                  size_t end_index = index;
                  if (!internal::json::get_element_end_index(memory, size, end_index, internal::json::types::string)) // the JSON is not well formatted
                    return false;

                  // get the key (must be a string)
                  if (!::neam::cr::persistence::serializable<persistence_backend::json, Key>::from_memory(temp_transaction, memory + index, end_index + 1 - index, &pair->first))
                    return false;

                  // skip the : token
                  index = end_index;
                  if (!internal::json::advance_next(memory, size, index, ':'))
                    break;

                  // chain unserialize
                  internal::json::types elem_type = internal::json::get_type(memory[index]);
                  end_index = index;
                  if (!internal::json::get_element_end_index(memory, size, end_index, elem_type))
                    return false;
                  if (!::neam::cr::persistence::serializable<persistence_backend::json, Value>::from_memory(temp_transaction, memory + index, end_index + 1 - index, &pair->second))
                    return false;
                  index = end_index;

                  ptr->emplace_hint(ptr->end(), std::move(pair->first), std::move(pair->second));

                  if (!internal::json::advance_next(memory, size, index, ','))
                    break; // can't do much more
                }
              }
              return true;
            }

            /// \brief Handle the list mode (this is just a copy/pasting of the default from_memory).
            template<typename Type>
            static inline bool from_memory_list(cr::allocation_transaction &transaction, const char *memory, size_t size, Type *ptr)
            {
              neam::array_wrapper<std::pair<Key, Value>> tmp(nullptr, 0);
              cr::allocation_transaction temp_transaction;
              if (!::neam::cr::persistence::serializable<persistence_backend::json, neam::array_wrapper<std::pair<Key, Value>>>::from_memory(temp_transaction, memory, size, &tmp))
              {
                temp_transaction.rollback();
                return false;
              }

//               new(ptr) std::map<Key, Value, Compare, Alloc>();
              transaction.register_destructor_call_on_failure(ptr);

              for (size_t i = 0; i < tmp.size; ++i)
                ptr->emplace_hint(ptr->end(), std::move(tmp.array[i].first), std::move(tmp.array[i].second));

              temp_transaction.rollback(); // free the memory allocated by the from_memory call

              return true;
            }
        };
      } // namespace json
    } // namespace internal
  } // namespace cr
} // namespace neam

#endif /*__N_7078393271892028266_731342187__SERIALIZABLE_SPECS_JSON_INTERNAL_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


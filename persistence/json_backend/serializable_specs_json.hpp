//
// file : serializable_specs.hpp
// in : file:///home/tim/projects/reflective/reflective/persistence/serializable_specs_json.hpp
//
// created by : Timothée Feuillet on linux.site
// date: 21/09/2014 12:52:25
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

#ifndef __N_331927306113683385_1782336113__SERIALIZABLE_SPECS_JSON_HPP__
# define __N_331927306113683385_1782336113__SERIALIZABLE_SPECS_JSON_HPP__

#include <type_traits>
#include <string>
#include <tools/array_wrapper.hpp>
#include <tools/demangle.hpp>
#include "object.hpp" // for my IDE
#include "raw_data.hpp"

#include "serializable_specs_json_internal.hpp"

namespace neam
{
  namespace cr
  {
    /// \brief Pointer serializer
    template<typename Type>
    class persistence::serializable<persistence_backend::json, Type *>
    {
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        template<typename... Params>
        static inline bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, Type **ptr, Params &&... p)
        {
          // handle the null pointer case
          if (size == 4 && memcmp(memory, "null", 4) == 0)
          {
            *ptr = nullptr;
            return true;
          }

          Type *tptr = reinterpret_cast<Type *>(transaction.allocate_raw(sizeof(Type)));
          if (!tptr)
            return false;
          *ptr = tptr;
          return serializable<persistence_backend::json, Type>::from_memory(transaction, memory, size, tptr, std::forward<Params>(p)...);
        }

        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &, Type **ptr)
        {
          *ptr = nullptr;
          return true;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static inline bool to_memory(memory_allocator &mem, size_t &size, const Type* const* ptr, size_t indent = 0, const char *name = nullptr)
        {
          // handle the null pointer case
          if (!*ptr)
            return internal::json::_allocate_format_string(mem, size, indent, name, "null");
          return serializable<persistence_backend::json, Type>::to_memory(mem, size, *ptr, indent, name);
        }
    };

    /// \brief boolean serializer
    template<>
    class persistence::serializable<persistence_backend::json, bool>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &, bool *ptr)
        {
          *ptr = false;
          return true;
        }

        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static inline bool from_memory(cr::allocation_transaction &, const char *memory, size_t size, bool *ptr)
        {
          internal::json::types type = internal::json::get_type(memory[0]);
          if (type != internal::json::types::other)
            return false;
          // handle the true pointer case
          if (size == 4 && memcmp(memory, "true", 4) == 0)
          {
            *ptr = true;
            return true;
          }
          // handle the false pointer case
          if (size == 5 && memcmp(memory, "false", 5) == 0)
          {
            *ptr = false;
            return true;
          }

          return false;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, const bool *ptr, size_t indent = 0, const char *name = nullptr)
        {
          if (*ptr)
            return internal::json::_allocate_format_string(mem, size, indent, name, "true");
          else
            return internal::json::_allocate_format_string(mem, size, indent, name, "false");
        }
    };

    /// \brief the default serializer for numeric types
    template<typename Type>
    class persistence::serializable<persistence_backend::json, Type, internal::numeric>
    {
      static_assert(std::is_arithmetic<Type>::value, "only arithmetic types here !!!");
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &, Type *ptr)
        {
          *ptr = 0;
          return true;
        }

        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static inline bool from_memory(cr::allocation_transaction &, const char *memory, size_t, Type *ptr)
        {
          internal::json::types type = internal::json::get_type(memory[0]);
          if (type != internal::json::types::number)
            return false;
          return internal::json::number_from_string(memory, *ptr);
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, const Type *ptr, size_t indent = 0, const char *name = nullptr)
        {
          return internal::json::_allocate_format_string(mem, size, indent, name, std::to_string(*ptr));
        }
    };

    // for arithmetic types
    template<> class persistence::serializable<persistence_backend::json, char> : public persistence::serializable<persistence_backend::json, char, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::json, unsigned char> : public persistence::serializable<persistence_backend::json, unsigned char, internal::numeric> {};

    template<> class persistence::serializable<persistence_backend::json, short> : public persistence::serializable<persistence_backend::json, short, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::json, unsigned short> : public persistence::serializable<persistence_backend::json, unsigned short, internal::numeric> {};

    template<> class persistence::serializable<persistence_backend::json, int> : public persistence::serializable<persistence_backend::json, int, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::json, unsigned int> : public persistence::serializable<persistence_backend::json, unsigned int, internal::numeric> {};

    template<> class persistence::serializable<persistence_backend::json, long> : public persistence::serializable<persistence_backend::json, long, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::json, unsigned long> : public persistence::serializable<persistence_backend::json, unsigned long, internal::numeric> {};

    template<> class persistence::serializable<persistence_backend::json, float> : public persistence::serializable<persistence_backend::json, float, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::json, double> : public persistence::serializable<persistence_backend::json, double, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::json, long double> : public persistence::serializable<persistence_backend::json, long double, internal::numeric> {};

    /// \brief a special case for C strings
    template<>
    class persistence::serializable<persistence_backend::json, char *>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &, char **ptr)
        {
          *ptr = nullptr;
          return true;
        }

        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static inline bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, char **ptr)
        {
          internal::json::types type = internal::json::get_type(memory[0]);
          // handle the null pointer case
          if (type == internal::json::types::other && size == 4 && memcmp(memory, "null", 4) == 0)
          {
            *ptr = nullptr;
            return true;
          }

          if (type != internal::json::types::string)
            return false;

          std::string un = internal::json::unescape_string(memory + 1, size - 1);

          // We have to remove the two quotation marks from the memory area
          *ptr = reinterpret_cast<char *>(transaction.allocate_raw(un.size() + 1));
          if (*ptr)
          {
            memcpy(*ptr, un.data(), un.size());
            (*ptr)[un.size()] = 0;
          }
          return !!*ptr;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        /// \note the stored string doesn't have the null byte stored (as we store its size instead)
        static bool to_memory(memory_allocator &mem, size_t &size, const char **ptr, size_t indent_level = 0, const char *name = nullptr)
        {
          if (!*ptr)
            return internal::json::_allocate_format_string(mem, size, indent_level, name, "null");
          return internal::json::_allocate_format_string(mem, size, indent_level, name, "\"" + internal::json::escape_string(*ptr) + "\"");
        }
    };

    template<typename First, typename Second>
    class persistence::serializable<persistence_backend::json, std::pair<First, Second>>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &transaction, std::pair<First, Second> *ptr)
        {
          if (!serializable<persistence_backend::json, First>::default_initializer(transaction, &ptr->first))
            return false;
          if (!serializable<persistence_backend::json, Second>::default_initializer(transaction, &ptr->second))
            return false;
          return true;
        }

        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static inline bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, std::pair<First, Second> *ptr)
        {
          internal::json::types type = internal::json::get_type(memory[0]);
          if (type != internal::json::types::collection)
            return false;
          --size;   // skip the closing brace
          ++memory; // skip the opening brace

          int status[2] = {0, 0}; // -1: failed, 0: not done, 1: done with success

          size_t index = 0;
          if (memory[0] == '"' || internal::json::advance_next(memory, size, index))
          {
            while (status[0] != 1 || status[1] != 1)
            {
              // retrieve the string index
              if (internal::json::get_type(memory[index]) != internal::json::types::string)
                return false;
              size_t end_index = index;
              if (!internal::json::get_element_end_index(memory, size, end_index, internal::json::types::string)) // the JSON is not well formatted
                return false;

              // test the string
              int element = -1; // BAD
              if (end_index - (index + 1) == 5 && !memcmp(memory + index + 1, "first", 5))
                element = 0; // first
              else if (end_index - (index + 1) == 6 && !memcmp(memory + index + 1, "second", 6))
                element = 1; // second

              // skip the : token
              index = end_index;
              if (!internal::json::advance_next(memory, size, index, ':'))
                break;

              // chain unserialize
              internal::json::types elem_type = internal::json::get_type(memory[index]);
              end_index = index;
              if (!internal::json::get_element_end_index(memory, size, end_index, elem_type))
                return false;
              if (element == 0 && status[0] != 1) // Not done or failed: do it this turn !
              {
                if (!serializable<persistence_backend::json, First>::from_memory(transaction, memory + index, end_index + 1 - index, &ptr->first))
                  status[0] = -1;
                else
                  status[0] = 1;
              }
              else if (element == 1 && status[1] != 1) // Not done or failed: do it this turn !
              {
                if (!serializable<persistence_backend::json, Second>::from_memory(transaction, memory + index, end_index + 1 - index, &ptr->second))
                  status[1] = -1;
                else
                  status[1] = 1;
              }
              index = end_index;
              if (!internal::json::advance_next(memory, size, index, ','))
                break; // can't do much more
            }
          }

          if (!status[0]) // not present in the JSON: default initialize it
          {
            if (!serializable<persistence_backend::json, First>::default_initializer(transaction, &ptr->first))
              return false;
          }
          if (!status[1]) // not present in the JSON: default initialize it
          {
            if (!serializable<persistence_backend::json, Second>::default_initializer(transaction, &ptr->second))
              return false;
          }
          return true;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, const std::pair<First, Second> *ptr, size_t indent_level = 0, const char *name = nullptr)
        {
          size_t global_size = 0;
          if (!internal::json::_allocate_format_string(mem, global_size, indent_level, name, ""))
            return false;
          if (!internal::json::_allocate_string(mem, global_size, 0, "{\n"))
            return false;

          size_t s1 = 0;
          bool ret = serializable<persistence_backend::json, First>::to_memory(mem, s1, &ptr->first, indent_level + 1, "first");
          if (!ret)
            return false;
          if (!internal::json::_allocate_string(mem, global_size, 0, ",\n"))
            return false;

          size_t s2 = 0;
          ret = serializable<persistence_backend::json, Second>::to_memory(mem, s2, &ptr->second, indent_level + 1, "second");
          if (!ret)
            return false;

          if (!internal::json::_allocate_string(mem, global_size, 0, "\n"))
            return false;
          if (!internal::json::_allocate_string(mem, global_size, indent_level, "}"))
            return false;

          size = global_size + s1 + s2;
          return true;
        }
    };

    template<>
    class persistence::serializable<persistence_backend::json, raw_data>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &transaction, raw_data *ptr)
        {
          new(ptr) raw_data(); // call the placement new for the default constructor
          transaction.register_destructor_call_on_failure(ptr);
          return true;
        }

        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static inline bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, raw_data *ptr)
        {
          // handle the null pointer case
          internal::json::types type = internal::json::get_type(memory[0]);
          if (type == internal::json::types::other && size == 4 && memcmp(memory, "null", 4) == 0)
          {
            new(ptr) raw_data(); // call the placement new for the default constructor
            transaction.register_destructor_call_on_failure(ptr);
            return true;
          }

          if (type != internal::json::types::string)
            return false;
          ++memory;

          // We have to remove the two quotation marks from the memory area
          uint8_t *data = reinterpret_cast<uint8_t *>(transaction.allocate_raw(size - 1));
          if (data)
          {
            // unescape the string
            size_t i = 0;
            size_t shift = 0;
            for (; i + shift < size - 1; ++i)
            {
              if (memory[i + shift] == '\\')
              {
                ++shift;
                if (i + shift >= size - 1)
                  return false;
                switch (memory[i + shift])
                {
                  case '\\':
                  case '"':
                    data[i] = memory[i + shift];
                    break;
                  case 'n':
                    data[i] = '\n';
                  case 't':
                    data[i] = '\t';
                  case 'b':
                    data[i] = '\b';
                  case 'r':
                    data[i] = '\r';
                  case 'f':
                    data[i] = '\f';
                  case 'u': // convert the following 4 [0-9A-F] code to binary data
                    ++shift;
                    if (i + shift + 3 >= size - 1)
                      return false;
                    char sdata[] = {memory[i + shift + 0], memory[i + shift + 1], memory[i + shift + 2], memory[i + shift + 3], 0};
                    shift += 3;
                    char *_u = sdata;
                    uint16_t t = strtoul(sdata, &_u, 16);
                    if (t & 0xFF00)
                    {
                      data[i++] = t >> 8;
                      --shift; // compensate the incrementation of i
                    }
                    data[i] = t & 0xFF;
                }
              }
              else
                data[i] = memory[i + shift];
            }
            ptr->data = reinterpret_cast<int8_t *>(data);
            ptr->size = i;
            ptr->ownership = true;
            // WE DON'T REGISTER THE DESTRUCTOR IN THE TRANSACTION: WE ALREADY HAVE THE MEMORY REGISTERED TO BE DELETED
            return true;
          }
          return false;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, const raw_data *ptr, size_t indent_level = 0, const char *name = nullptr)
        {
          if (!ptr->data)
            return internal::json::_allocate_format_string(mem, size, indent_level, name, "null");
          std::string data;
          data.reserve(ptr->size * 8 + 2);
          data += "\"";
          for (size_t i = 0; i < ptr->size; ++i)
          {
            if ((std::isgraph(ptr->data[i]) && ptr->data[i] != '"' && ptr->data[i] != '\\') || ptr->data[i] == ' ')
              data += ptr->data[i];
            else if(ptr->data[i] != '"' && ptr->data[i] != '\\')
            {
              data += "\\u00";
              data += internal::json::hex_alphabet[(ptr->data[i] & 0xF0) >> 4];
              data += internal::json::hex_alphabet[ptr->data[i] & 0x0F];
            }
            else
            {
              data += "\\";
              data += ptr->data[i];
            }
          }
          data += "\"";
          return internal::json::_allocate_format_string(mem, size, indent_level, name, data);
        }
    };

    /// \brief Helper to [de]serialize list-like objects
    namespace persistence_helper
    {
      template<typename Type, typename Caller>
      class list_serializable<persistence_backend::json, Type, Caller>
      {
        public:
          /// \brief Called to deserialize the list-object
          static inline bool from_memory(allocation_transaction &transaction, const char *memory, size_t size, Type *ptr)
          {
            internal::json::types type = internal::json::get_type(memory[0]);

            // handle the null pointer case
            if (type == internal::json::types::other && size == 4 && memcmp(memory, "null", 4) == 0)
              return Caller::from_memory_null(transaction, ptr);

            if (type != internal::json::types::list)
              return false;

            ++memory; // skip the opening bracket
            --size;

            size_t index = 0;
            if (!std::isspace(memory[0]) || internal::json::advance_next(memory, size, index))
            {
              const size_t element_count = get_slot_count(memory + index, size);
              if (element_count)
              {
                if (!Caller::from_memory_allocate(transaction, element_count, ptr))
                  return false;

                if (element_count)
                {
                  size_t element_index = 0;
                  while (true)
                  {
                    size_t end_index = index;

                    internal::json::types elem_type = internal::json::get_type(memory[index]);
                    if (!internal::json::get_element_end_index(memory, size, end_index, elem_type))
                      return false;

                    // chain unserialize
                    if (!Caller::from_memory_single(transaction, ptr, memory + index, end_index - index, element_index))
                      return false;

                    index = end_index;
                    ++element_index;

                    if (!internal::json::advance_next(memory, size, index, ','))
                      break; // can't do much more
                  }
                }
              }
              else
                return Caller::from_memory_null(transaction, ptr);
            }
            return true;
          }

          /// \brief Called to serialize the list-object
          static inline bool to_memory(memory_allocator &mem, size_t &size, const Type *ptr, size_t indent_level = 0, const char *name = nullptr)
          {
            size_t whole_object_size = 0;

            const size_t element_count = Caller::to_memory_get_element_count(ptr);
            auto iterator = Caller::to_memory_get_iterator(ptr);

            if (!internal::json::_allocate_format_string(mem, whole_object_size, indent_level, name, ""))
              return false;

            if (!internal::json::_allocate_string(mem, whole_object_size, 0, "[\n"))
              return false;

            for (size_t index = 0; index < element_count; ++index)
            {
              if (index > 0)
              {
                if (!internal::json::_allocate_string(mem, whole_object_size, 0, ",\n"))
                  return false;
              }
              size_t element_size = 0;
              if (!Caller::to_memory_single(mem, element_size, iterator, ptr, indent_level + 1))
                return false;
              whole_object_size += element_size;
              if (!Caller::to_memory_increment_iterator(iterator))
                return false;
            }
            if (!Caller::to_memory_end_iterator(iterator))
              return false;

            if (!internal::json::_allocate_string(mem, whole_object_size, 0, "\n"))
              return false;
            if (!internal::json::_allocate_string(mem, whole_object_size, indent_level, "]"))
              return false;

            size = whole_object_size;

            return true;
          }

        private:
          /// \todo find something better than this slow thing
          static inline size_t get_slot_count(const char *memory, size_t size)
          {
            size_t count = 0;
            size_t index = 0;
            while (true)
            {
              size_t end_index = index;

              // chain unserialize
              internal::json::types elem_type = internal::json::get_type(memory[index]);
              end_index = index;
              if (!internal::json::get_element_end_index(memory, size, end_index, elem_type))
                return 0;
              ++count;
              index = end_index;

              if (!internal::json::advance_next(memory, size, index, ','))
                break; // can't do much more
            }
            return count;
          }
      };
    } // namespace persistence_helper

    template<typename Key, typename Value, typename Compare, typename Alloc>
    class persistence::serializable<persistence_backend::json, std::map<Key, Value, Compare, Alloc>>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &transaction, std::map<Key, Value> *ptr)
        {
          new(ptr) std::map<Key, Value>();
          transaction.register_destructor_call_on_failure(ptr);
          return true;
        }

        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static inline bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, std::map<Key, Value> *ptr)
        {
          internal::json::types type = internal::json::get_type(memory[0]);
          if (type == internal::json::types::collection)
            return from_memory_collection(transaction, memory, size, ptr);
          else if (type == internal::json::types::list)
            return from_memory_list(transaction, memory, size, ptr);
          return false;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static inline bool to_memory(memory_allocator &mem, size_t &size, const std::map<Key, Value> *ptr, size_t indent_level = 0, const char *name = nullptr)
        {
          if (internal::json::is_string_key<Key>::value)
            return to_memory_collection(mem, size, ptr, indent_level, name);
          else
            return to_memory_list(mem, size, ptr, indent_level, name);
        }

      private:
        /// \brief Handle the collection mode of the std::map
        static inline bool from_memory_collection(cr::allocation_transaction &transaction, const char *memory, size_t size, std::map<Key, Value> *ptr)
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
              if (!serializable<persistence_backend::json, Key>::from_memory(temp_transaction, memory + index, end_index - index, &pair->first))
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
              if (!serializable<persistence_backend::json, Value>::from_memory(temp_transaction, memory + index, end_index - index, &pair->second))
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
        static inline bool from_memory_list(cr::allocation_transaction &transaction, const char *memory, size_t size, std::map<Key, Value> *ptr)
        {
          neam::array_wrapper<std::pair<Key, Value>> tmp(nullptr, 0);
          cr::allocation_transaction temp_transaction;
          if (!serializable<persistence_backend::json, neam::array_wrapper<std::pair<Key, Value>>>::from_memory(temp_transaction, memory, size, &tmp))
          {
            temp_transaction.rollback();
            return false;
          }

          new(ptr) std::map<Key, Value, Compare, Alloc>();
          transaction.register_destructor_call_on_failure(ptr);

          for (size_t i = 0; i < tmp.size; ++i)
            ptr->emplace_hint(ptr->end(), std::move(tmp.array[i].first), std::move(tmp.array[i].second));

          temp_transaction.rollback(); // free the memory allocated by the from_memory call

          return true;
        }


        /// \brief collection serializer (when key is a string)
        static inline bool to_memory_collection(memory_allocator &mem, size_t &size, const std::map<Key, Value> *ptr, size_t indent_level = 0, const char *name = nullptr)
        {
          size_t whole_object_size = 0;
          bool res = true;

          if (!internal::json::_allocate_format_string(mem, whole_object_size, indent_level, name, ""))
            return false;

          if (!internal::json::_allocate_string(mem, whole_object_size, 0, "{\n"))
            return false;

          bool first = true;
          for (auto &it : *ptr)
          {
            if (!first)
            {
              if (!internal::json::_allocate_string(mem, whole_object_size, 0, ",\n"))
                return false;
            }
            size_t sz = 0;
            if (!serializable<persistence_backend::json, Key>::to_memory(mem, sz, &it.first, indent_level + 1))
              return false;
            whole_object_size += sz;
            if (!internal::json::_allocate_string(mem, whole_object_size, indent_level, ":"))
              return false;
            sz = 0;
            if (!serializable<persistence_backend::json, Value>::to_memory(mem, sz, &it.second, indent_level + 1))
              return false;
            whole_object_size += sz;
            first = false;
          }
          if (!internal::json::_allocate_string(mem, whole_object_size, 0, "\n"))
            return false;
          if (!internal::json::_allocate_string(mem, whole_object_size, indent_level, "}"))
            return false;
          size = whole_object_size;
          return res;
        }

        /// \brief Handle the list mode (this is just a copy/pasting of the default from_memory).
        template<typename... Params>
        static inline bool to_memory_list(memory_allocator &mem, size_t &size, const std::map<Key, Value> *ptr, Params &&... p)
        {
          std::vector<std::pair<const Key, Value> *> tmp;
          tmp.reserve(ptr->size());

          for (auto & it : *ptr)
            tmp.emplace_back(const_cast<std::pair<const Key, Value> *>(&it));

          return serializable<persistence_backend::json, std::vector<std::pair<const Key, Value> *>>::to_memory(mem, size, &tmp, std::forward<Params>(p)...);
        }
    };

    /// \brief this serialize complex objects (like classes)
    /// this generate meta-data used to generate code that will fill the object.
    /// \param OffsetTypeList is a list of \code typed_offset <type, offsetof(my_class, member)> \endcode that could be simplified with the macro \code NRP_TYPPED_OFFSET(my_class, member) \endcode
    /// \note you still have to create a \code serializable \endcode specialization for the object, but you could simply use this trick: \code template<>class serializable<my_class> : public serializable_object<...> {}; \endcode
    /// \see constructible_serializable_object
    template<typename... OffsetTypeList>
    class persistence::serializable_object<persistence_backend::json, OffsetTypeList...>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &, void *)
        {
          return false;
        }

        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static inline bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, void *ptr)
        {
          internal::json::types type = internal::json::get_type(memory[0]);
          if (type != internal::json::types::collection)
            return false;
          --size;   // skip the closing brace
          ++memory; // skip the opening brace

          bool ar[sizeof...(OffsetTypeList)] = {false};

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
              size_t key_start_index = index + 1; // skip the quotation mark
              size_t key_size = end_index - 1 - index; // skip the end quotation mark

              // skip the : token
              index = end_index;
              if (!internal::json::advance_next(memory, size, index, ':'))
                break;

              // chain unserialize
              internal::json::types elem_type = internal::json::get_type(memory[index]);
              end_index = index;
              if (!internal::json::get_element_end_index(memory, size, end_index, elem_type))
                return false;
              if (!loop_and_unserialize(ar, transaction, memory + index, end_index - index, ptr, memory + key_start_index, key_size))
                return false;
              index = end_index;


              if (!internal::json::advance_next(memory, size, index, ','))
                break; // can't do much more
            }
          }

          if (!loop_for_default(ar, transaction, ptr))
            return false;

          return true;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, const void *ptr, size_t indent_level = 0, const char *name = nullptr)
        {
          size_t whole_object_size = 0;
          bool res = true;

          int count = 0;
          NEAM_EXECUTE_PACK((res &= to_memory_single<OffsetTypeList>(ptr, whole_object_size, mem, indent_level, name, ++count)));

          // deallocate unused memory
          if (!res)
            return false;
          size = whole_object_size;

          return true;
        }

      private:
        static inline bool loop_and_unserialize(bool *ar, cr::allocation_transaction &transaction, const char *memory, size_t size, void *ptr, const char *key, size_t key_size)
        {
          bool res = true;
          size_t index = 0;
          // to explain a bit what this does:
          //  if compare the key to each of the possible values that are in the OffsetTypeList
          //  if it match, it then unserialize the element by calling from_memory(...)
          NEAM_EXECUTE_PACK((
            res &= (++index && strlen(OffsetTypeList::name) == key_size && !memcmp(key, OffsetTypeList::name, key_size) && !ar[index - 1] && (ar[index - 1] = true)
            ? serializable<persistence_backend::json, typename OffsetTypeList::type>::from_memory(transaction, memory, size, reinterpret_cast<typename OffsetTypeList::type *>(reinterpret_cast<uint8_t *>(ptr) + OffsetTypeList::offset))
            : true
          )));
          return res;
        }

        static inline bool loop_for_default(bool *ar, cr::allocation_transaction &transaction, void *ptr)
        {
          bool res = true;
          size_t index = 0;
          // to explain a bit what this does:
          //  if compare the key to each of the possible values that are in the OffsetTypeList
          //  if it match, it then unserialize the element by calling from_memory(...)
          NEAM_EXECUTE_PACK((
            res &= (++index && !ar[index - 1]
            ? serializable<persistence_backend::json, typename OffsetTypeList::type>::default_initializer(transaction, reinterpret_cast<typename OffsetTypeList::type *>(reinterpret_cast<uint8_t *>(ptr) + OffsetTypeList::offset))
            : true
          )));
          return res;
        }

        template<typename OffsetType>
        static inline bool to_memory_single(const void *ptr, size_t &global_size, memory_allocator &mem, size_t indent_level, const char *name, int count)
        {
          size_t element_size = 0;

          if (count == 1) // the first element, get the object type, print the header
          {
              if (!internal::json::_allocate_format_string(mem, global_size, indent_level, name, ""))
                return false;
              if (!internal::json::_allocate_string(mem, global_size, 0, "{\n"))
                return false;
          }
          else
          {
            if (!internal::json::_allocate_string(mem, global_size, 0, ",\n"))
              return false;
          }

          if (!serializable<persistence_backend::json, typename OffsetType::type>::to_memory(mem, element_size, reinterpret_cast<const typename OffsetType::type *>(reinterpret_cast<const uint8_t *>(ptr) + OffsetType::offset), indent_level + 1, OffsetType::name))
            return false;


          if (count == sizeof...(OffsetTypeList))
          {
            if (!internal::json::_allocate_string(mem, global_size, 0, "\n"))
                return false;
            if (!internal::json::_allocate_string(mem, global_size, indent_level, "}"))
                return false;
          }

          global_size += element_size;
          return true;
        }
    };
  } // namespace r
} // namespace neam

#endif /*__N_331927306113683385_1782336113__SERIALIZABLE_SPECS_JSON_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


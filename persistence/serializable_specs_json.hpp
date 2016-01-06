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

namespace neam
{
  namespace cr
  {
    namespace internal
    {
      // already defined by the verbose backend
      // struct numeric {};

      namespace json
      {
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
              res += "\\x";
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

    /// \brief Pointer serializer
    template<typename Type>
    class persistence::serializable<persistence_backend::json, Type *>
    {
      public:
        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        template<typename... Params>
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

    /// \brief serialize an array and all its elements
    template<typename Type>
    class persistence::serializable<persistence_backend::json, neam::array_wrapper<Type>>
    {
      public:
        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, const neam::array_wrapper<Type> *ptr, size_t indent_level = 0, const char *name = nullptr)
        {
          size_t whole_object_size = 0;
          bool res = true;

          if (!internal::json::_allocate_format_string(mem, whole_object_size, indent_level, name, ""))
            return false;

          if (!internal::json::_allocate_string(mem, whole_object_size, 0, "[\n"))
            return false;

          for (size_t index = 0; index < ptr->size; ++index)
          {
            if (index > 0)
            {
              if (!internal::json::_allocate_string(mem, whole_object_size, 0, ",\n"))
                return false;
            }
            res &= to_memory_single(ptr->array + index, whole_object_size, mem, indent_level, nullptr);
            if (!res)
              return false;
          }
          if (!internal::json::_allocate_string(mem, whole_object_size, 0, "\n"))
            return false;
          if (!internal::json::_allocate_string(mem, whole_object_size, indent_level, "]"))
            return false;

          size = whole_object_size;

          return res;
        }


      private:
        static inline bool to_memory_single(const Type *ptr, size_t &global_size, memory_allocator &mem, size_t indent_level, const char *name)
        {
          size_t element_size = 0;

          if (!serializable<persistence_backend::json, Type>::to_memory(mem, element_size, ptr, indent_level + 1, name))
            return false;

          global_size += element_size;
          return true;
        }
    };

    template<typename Key, typename Value, typename Compare, typename Alloc>
    class persistence::serializable<persistence_backend::json, std::map<Key, Value, Compare, Alloc>>
    {
      public:
        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        template<typename... Params>
        static inline bool to_memory(memory_allocator &mem, size_t &size, const std::map<Key, Value> *ptr, size_t indent_level = 0, const char *name = nullptr)
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


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

#ifndef __N_331927306113683385_1782336113__SERIALIZABLE_SPECS_verbose_HPP__
# define __N_331927306113683385_1782336113__SERIALIZABLE_SPECS_verbose_HPP__

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
      struct numeric {};

      namespace verbose
      {
        static inline bool _allocate_string(memory_allocator &mem, size_t &size, size_t indent_level, const std::string &data)
        {
          void *d = mem.allocate(indent_level * 2 + data.size());

          if (d)
          {
            size_t i = 0;
            for (; i < indent_level * 2; ++i) ((char *)d)[i] = ' ';

            memcpy((char *)d + i, data.data(), data.size());
            size += data.size();
            return true;
          }
          size += 0;
          return false;
        }

        static inline bool _allocate_format_string(memory_allocator &mem, size_t &size, size_t indent_level, const std::string &type, const char *name, const std::string &data)
        {
          std::string res = type + " " + std::string(name ? name : "") + " = ";
          res += data + "\n";

          return _allocate_string(mem, size, indent_level, res);
        }

        template<typename Type>
        static inline bool _allocate_format_string(memory_allocator &mem, size_t &size, size_t indent_level, const char *name, const std::string &data)
        {
          return _allocate_format_string(mem, size, indent_level, neam::demangle<Type>(), name, data);
        }
      } // namespace verbose
    } // namespace internal

    /// \brief boolean serializer
    template<>
    class persistence::serializable<persistence_backend::verbose, bool>
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
            return internal::verbose::_allocate_format_string<bool>(mem, size, indent, name, "true");
          else
            return internal::verbose::_allocate_format_string<bool>(mem, size, indent, name, "false");
        }
    };

    /// \brief the default serializer for numeric types
    template<typename Type>
    class persistence::serializable<persistence_backend::verbose, Type, internal::numeric>
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
          return internal::verbose::_allocate_format_string<Type>(mem, size, indent, name, std::to_string(*ptr));
        }
    };

    // for arithmetic types
    template<> class persistence::serializable<persistence_backend::verbose, char> : public persistence::serializable<persistence_backend::verbose, char, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::verbose, unsigned char> : public persistence::serializable<persistence_backend::verbose, unsigned char, internal::numeric> {};

    template<> class persistence::serializable<persistence_backend::verbose, short> : public persistence::serializable<persistence_backend::verbose, short, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::verbose, unsigned short> : public persistence::serializable<persistence_backend::verbose, unsigned short, internal::numeric> {};

    template<> class persistence::serializable<persistence_backend::verbose, int> : public persistence::serializable<persistence_backend::verbose, int, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::verbose, unsigned int> : public persistence::serializable<persistence_backend::verbose, unsigned int, internal::numeric> {};

    template<> class persistence::serializable<persistence_backend::verbose, long> : public persistence::serializable<persistence_backend::verbose, long, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::verbose, unsigned long> : public persistence::serializable<persistence_backend::verbose, unsigned long, internal::numeric> {};

    template<> class persistence::serializable<persistence_backend::verbose, float> : public persistence::serializable<persistence_backend::verbose, float, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::verbose, double> : public persistence::serializable<persistence_backend::verbose, double, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::verbose, long double> : public persistence::serializable<persistence_backend::verbose, long double, internal::numeric> {};

    /// \brief a special case for C strings
    template<>
    class persistence::serializable<persistence_backend::verbose, char *>
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
            return internal::verbose::_allocate_format_string<char *>(mem, size, indent_level, name, "[nullptr]");
          return internal::verbose::_allocate_format_string<char *>(mem, size, indent_level, name, "\"" + std::string(*ptr) + "\"");
        }
    };

    template<typename First, typename Second>
    class persistence::serializable<persistence_backend::verbose, std::pair<First, Second>>
    {
      public:
        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, const std::pair<First, Second> *ptr, size_t indent_level = 0, const char *name = nullptr)
        {
          if (!internal::verbose::_allocate_format_string<std::pair<First, Second>>(mem, size, indent_level, name, ""))
            return false;
          if (!internal::verbose::_allocate_string(mem, size, indent_level, "{\n"))
            return false;

          bool ret = serializable<persistence_backend::verbose, First>::to_memory(mem, size, &ptr->first, indent_level + 1, "first");
          if (!ret)
            return false;

          ret = serializable<persistence_backend::verbose, Second>::to_memory(mem, size, &ptr->second, indent_level + 1, "second");
          if (!ret)
            return false;

          if (!internal::verbose::_allocate_string(mem, size, indent_level, "}\n"))
            return false;

          return true;
        }
    };

    template<>
    class persistence::serializable<persistence_backend::verbose, raw_data>
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
            return internal::verbose::_allocate_format_string<raw_data>(mem, size, indent_level, name, "size: " + std::to_string(ptr->size) + ": [nullptr]");
          return internal::verbose::_allocate_format_string<raw_data>(mem, size, indent_level, name, "size: " + std::to_string(ptr->size) + ": [...] (stripped out binary data)");
        }
    };

    /// \brief serialize an array and all its elements
    template<typename Type>
    class persistence::serializable<persistence_backend::verbose, neam::array_wrapper<Type>>
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

          if (!internal::verbose::_allocate_format_string<Type[]>(mem, whole_object_size, indent_level, name, "// [" + std::to_string(ptr->size) + "]"))
            return false;

          if (!internal::verbose::_allocate_string(mem, size, indent_level, "{\n"))
            return false;

          for (size_t index = 0; index < ptr->size; ++index)
          {
            res &= to_memory_single(ptr->array + index, whole_object_size, mem, indent_level, nullptr);
            if (!res)
              return false;
          }

          if (!internal::verbose::_allocate_string(mem, size, indent_level, "}\n"))
            return false;

          size = whole_object_size;

          return res;
        }


      private:
        static inline bool to_memory_single(const Type *ptr, size_t &global_size, memory_allocator &mem, size_t indent_level, const char *name)
        {
          size_t element_size = 0;

          if (!serializable<persistence_backend::verbose, Type>::to_memory(mem, element_size, ptr, indent_level + 1, name))
            return false;

          global_size += element_size;
          return true;
        }
    };

    /// \brief this serialize complex objects (like classes)
    /// this generate meta-data used to generate code that will fill the object.
    /// \param OffsetTypeList is a list of \code typed_offset <type, offsetof(my_class, member)> \endcode that could be simplified with the macro \code NRP_TYPPED_OFFSET(my_class, member) \endcode
    /// \note you still have to create a \code serializable \endcode specialization for the object, but you could simply use this trick: \code template<>class serializable<my_class> : public serializable_object<...> {}; \endcode
    /// \see constructible_serializable_object
    template<typename... OffsetTypeList>
    class persistence::serializable_object<persistence_backend::verbose, OffsetTypeList...>
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
              if (!internal::verbose::_allocate_format_string<typename OffsetType::object>(mem, global_size, indent_level, name, "// [" + std::to_string(sizeof...(OffsetTypeList)) + "]"))
                return false;
              if (!internal::verbose::_allocate_string(mem, global_size, indent_level, "{\n"))
                return false;
          }

          if (!serializable<persistence_backend::verbose, typename OffsetType::type>::to_memory(mem, element_size, reinterpret_cast<const typename OffsetType::type *>(reinterpret_cast<const uint8_t *>(ptr) + OffsetType::offset), indent_level + 1, OffsetType::name))
            return false;

          if (count == sizeof...(OffsetTypeList))
          {
            if (!internal::verbose::_allocate_string(mem, global_size, indent_level, "}\n"))
                return false;
          }

          global_size += element_size;
          return true;
        }
    };
  } // namespace r
} // namespace neam

#endif /*__N_331927306113683385_1782336113__SERIALIZABLE_SPECS_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


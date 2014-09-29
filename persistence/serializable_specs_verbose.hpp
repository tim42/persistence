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
        static inline bool _allocate_format_string(memory_allocator &mem, size_t &size, size_t indent_level, const std::string &type, const std::string &data)
        {
          std::string res;

          // 4 spaces indentation with some lines
          /*size_t i = 0;
          for (; i < indent_level && i < indent_level - 1; ++i) res += "|   ";
          for (; i < indent_level; ++i) res += "|---";*/

          // two space indentation
          for (size_t i = 0; i < indent_level; ++i) res += "  ";

          res += type + ": ";
          res += data + "\n";
          void *d = mem.allocate(res.size());

          if (d)
          {
            memcpy(d, res.data(), res.size());
            size = res.size();
            return true;
          }
          size = 0;
          return false;
        }

        template<typename Type>
        static inline bool _allocate_format_string(memory_allocator &mem, size_t &size, size_t indent_level, const std::string &data)
        {
          return _allocate_format_string(mem, size, indent_level, neam::demangle<Type>(), data);
        }
      } // namespace verbose
    } // namespace internal


    /// \brief the default serializer for the \e neam backend
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
        static bool to_memory(memory_allocator &mem, size_t &size, const Type *ptr, size_t indent = 0)
        {
          return internal::verbose::_allocate_format_string<Type>(mem, size, indent, std::to_string(*ptr));
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
        static bool to_memory(memory_allocator &mem, size_t &size, const char **ptr, size_t indent_level = 0)
        {
          if (!*ptr)
            return internal::verbose::_allocate_format_string<char *>(mem, size, indent_level, "[nullptr]");
          return internal::verbose::_allocate_format_string<char *>(mem, size, indent_level, "\"" + std::string(*ptr) + "\"");
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
        static bool to_memory(memory_allocator &mem, size_t &size, const std::pair<First, Second> *ptr, size_t indent_level = 0)
        {
          if (!internal::verbose::_allocate_format_string<std::pair<First, Second>>(mem, size, indent_level, ""))
            return false;
          size_t sz[2] = {0, 0};
          bool ret = serializable<persistence_backend::verbose, First>::to_memory(mem, sz[0], &ptr->first, indent_level + 1);
          if (!ret)
            return false;

          ret = serializable<persistence_backend::verbose, Second>::to_memory(mem, sz[1], &ptr->second, indent_level + 1);
          if (!ret)
            return false;

          size += sz[0] + sz[1];

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
        static bool to_memory(memory_allocator &mem, size_t &size, const raw_data *ptr, size_t indent_level = 0)
        {
          if (!ptr->data)
            return internal::verbose::_allocate_format_string<raw_data>(mem, size, indent_level, "size: " + std::to_string(ptr->size) + ": [nullptr]");
          return internal::verbose::_allocate_format_string<raw_data>(mem, size, indent_level, "size: " + std::to_string(ptr->size) + ": [...] (stripped out binary data)");
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
        static bool to_memory(memory_allocator &mem, size_t &size, const neam::array_wrapper<Type> *ptr, size_t indent_level = 0)
        {
          size_t whole_object_size = 0;
          bool res = true;

          if (!internal::verbose::_allocate_format_string<Type[]>(mem, whole_object_size, indent_level, "[" + std::to_string(ptr->size) + "]"))
            return false;
          for (size_t index = 0; index < ptr->size; ++index)
          {
            res &= to_memory_single(ptr->array + index, whole_object_size, mem, indent_level);
            if (!res)
              return false;
          }

          size = whole_object_size;

          return res;
        }


      private:
        static inline bool to_memory_single(const Type *ptr, size_t &global_size, memory_allocator &mem, size_t indent_level)
        {
          size_t element_size = 0;

          if (!serializable<persistence_backend::verbose, Type>::to_memory(mem, element_size, ptr, indent_level + 1))
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
        static bool to_memory(memory_allocator &mem, size_t &size, const void *ptr, size_t indent_level = 0)
        {
          size_t whole_object_size = 0;
          bool res = true;

          int count = 0;
          NEAM_EXECUTE_PACK((res &= to_memory_single<OffsetTypeList>(ptr, whole_object_size, mem, indent_level, ++count)));

          // deallocate unused memory
          if (!res)
            return false;
          size = whole_object_size;

          return true;
        }

      private:

        template<typename OffsetType>
        static inline bool to_memory_single(const void *ptr, size_t &global_size, memory_allocator &mem, size_t indent_level, int count)
        {
          size_t element_size = 0;

          if (count == 1) // the first element, get the object type, print the header
          {
            if (!internal::verbose::_allocate_format_string<typename OffsetType::object>(mem, global_size, indent_level, "[" + std::to_string(sizeof...(OffsetTypeList)) + "]"))
              return false;
          }

          if (!serializable<persistence_backend::verbose, typename OffsetType::type>::to_memory(mem, element_size, reinterpret_cast<const typename OffsetType::type *>(reinterpret_cast<const uint8_t *>(ptr) + OffsetType::offset), indent_level + 1))
            return false;

          global_size += element_size;
          return true;
        }
    };
  } // namespace r
} // namespace neam

#endif /*__N_331927306113683385_1782336113__SERIALIZABLE_SPECS_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


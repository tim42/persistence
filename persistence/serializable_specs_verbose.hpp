//
// file : serializable_specs.hpp
// in : file:///home/tim/projects/reflective/reflective/persistence/serializable_specs_json.hpp
//
// created by : Timothée Feuillet on linux.site
// date: 21/09/2014 12:52:25
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

#ifndef __N_331927306113683385_1782336113__SERIALIZABLE_SPECS_verbose_HPP__
# define __N_331927306113683385_1782336113__SERIALIZABLE_SPECS_verbose_HPP__

#include <type_traits>
#include <string>
#include "tools/array_wrapper.hpp"
#include "tools/demangle.hpp"
#include "object.hpp" // for my IDE
#include "raw_data.hpp"

namespace neam
{
  namespace cr
  {
    namespace internal
    {
      // already defined by the neam backend
      // struct numeric {};

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
          return _allocate_format_string(mem, size, indent_level, ::neam::demangle<Type>(), name, data);
        }
      } // namespace verbose
    } // namespace internal

    /// \brief Pointer serializer
    template<typename Type>
    class persistence::serializable<persistence_backend::verbose, Type *, typename std::enable_if<!std::is_same<Type, char>::value && !std::is_same<Type, const char>::value, void>::type>
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
            return internal::verbose::_allocate_format_string<Type *>(mem, size, indent, name, "nullptr");
          return serializable<persistence_backend::verbose, Type>::to_memory(mem, size, *ptr, indent, name);
        }
    };

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
        static bool to_memory(memory_allocator &mem, size_t &size, const char *const*ptr, size_t indent_level = 0, const char *name = nullptr)
        {
          if (!*ptr)
            return internal::verbose::_allocate_format_string<char *>(mem, size, indent_level, name, "[nullptr]");
          return internal::verbose::_allocate_format_string<char *>(mem, size, indent_level, name, "\"" + std::string(*ptr) + "\"");
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

    namespace persistence_helper
    {
      /// \brief Helper to [de]serialize list-like objects
      template<typename Type, typename Caller>
      class list_serializable<persistence_backend::verbose, Type, Caller>
      {
        public:
          /// \brief Called to serialize the list-object
          static inline bool to_memory(memory_allocator &mem, size_t &size, const Type *ptr, size_t indent_level = 0, const char *name = nullptr)
          {
            size_t whole_object_size = 0;

            const size_t element_count = Caller::to_memory_get_element_count(ptr);
            auto iterator = Caller::to_memory_get_iterator(ptr);

            if (!internal::verbose::_allocate_format_string<Type>(mem, whole_object_size, indent_level, name, "// [" + std::to_string(element_count) + "]"))
              return false;

            if (!internal::verbose::_allocate_string(mem, size, indent_level, "{\n"))
              return false;

            for (size_t index = 0; index < element_count; ++index)
            {
              size_t element_size = 0;
              if (!Caller::to_memory_single(mem, element_size, iterator, ptr, indent_level + 1))
                return false;
              whole_object_size += element_size;
              if (!Caller::to_memory_increment_iterator(iterator))
                return false;
            }

            if (!Caller::to_memory_end_iterator(iterator))
              return false;

            if (!internal::verbose::_allocate_string(mem, size, indent_level, "}\n"))
              return false;

            size = whole_object_size;

            return true;
          }
      };

      template<typename Type, typename Caller, serializable_mode Mode>
      class collection_serializable<persistence_backend::verbose, Type, Caller, Mode> : public list_serializable<persistence_backend::verbose, Type, Caller, Mode>
      {
      };
    } // namespace persistence_helper

    /// \brief this serialize complex objects (like classes)
    /// this generate meta-data used to generate code that will fill the object.
    /// \param OffsetTypeList is a list of \code typed_offset <type, offsetof(my_class, member)> \endcode that could be simplified with the macro \code NRP_TYPPED_OFFSET(my_class, member) \endcode
    /// \note you still have to create a \code serializable \endcode specialization for the object, but you could simply use this trick: \code template<>class serializable<my_class> : public serializable_object<...> {}; \endcode
    /// \see constructible_serializable_object
    template<typename Type, typename... OffsetTypeList>
    class persistence::serializable_object<persistence_backend::verbose, Type, OffsetTypeList...>
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


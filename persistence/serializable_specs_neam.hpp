//
// file : serializable_specs.hpp
// in : file:///home/tim/projects/reflective/reflective/persistence/serializable_specs.hpp
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

#ifndef __N_331927306113683385_1782336113__SERIALIZABLE_SPECS_HPP__
# define __N_331927306113683385_1782336113__SERIALIZABLE_SPECS_HPP__

#include <type_traits>
#include <tools/array_wrapper.hpp>
#include "object.hpp" // for my IDE
#include "raw_data.hpp"

namespace neam
{
  namespace cr
  {
    /// \brief the default serializer for the \e neam backend
    template<typename Type>
    class persistence::serializable<persistence_backend::neam, Type, raw>
    {
      static_assert(std::is_arithmetic<Type>::value, "only arithmetic types here !!!");
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static inline bool from_memory(const char *memory, size_t size, Type *ptr)
        {
          if (size != sizeof(Type))
            return false;
          *ptr = *reinterpret_cast<const Type *>(memory);
          return true;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static inline bool to_memory(memory_allocator &mem, size_t &size, const Type *ptr)
        {
          size = sizeof(Type);
          char *memory = reinterpret_cast<char *>(mem.allocate(size));
          if (!memory)
            return false;
          *reinterpret_cast<Type *>(memory) = *ptr;
          return true;
        }
    };

    // for arithmetic types
    template<> class persistence::serializable<persistence_backend::neam, char> : public persistence::serializable<persistence_backend::neam, char, raw> {};
    template<> class persistence::serializable<persistence_backend::neam, unsigned char> : public persistence::serializable<persistence_backend::neam, unsigned char, raw> {};

    template<> class persistence::serializable<persistence_backend::neam, short> : public persistence::serializable<persistence_backend::neam, short, raw> {};
    template<> class persistence::serializable<persistence_backend::neam, unsigned short> : public persistence::serializable<persistence_backend::neam, unsigned short, raw> {};

    template<> class persistence::serializable<persistence_backend::neam, int> : public persistence::serializable<persistence_backend::neam, int, raw> {};
    template<> class persistence::serializable<persistence_backend::neam, unsigned int> : public persistence::serializable<persistence_backend::neam, unsigned int, raw> {};

    template<> class persistence::serializable<persistence_backend::neam, long> : public persistence::serializable<persistence_backend::neam, long, raw> {};
    template<> class persistence::serializable<persistence_backend::neam, unsigned long> : public persistence::serializable<persistence_backend::neam, unsigned long, raw> {};

    template<> class persistence::serializable<persistence_backend::neam, float> : public persistence::serializable<persistence_backend::neam, float, raw> {};
    template<> class persistence::serializable<persistence_backend::neam, double> : public persistence::serializable<persistence_backend::neam, double, raw> {};
    template<> class persistence::serializable<persistence_backend::neam, long double> : public persistence::serializable<persistence_backend::neam, long double, raw> {};


    template<>
    class persistence::serializable<persistence_backend::neam, raw_data>
    {
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static inline bool from_memory(const char *memory, size_t size, raw_data *ptr)
        {
          if (size < sizeof(uint32_t))
            return false;
          raw_data o(*reinterpret_cast<const uint32_t *>(memory), reinterpret_cast<int8_t *>(const_cast<char *>(memory + sizeof(uint32_t))));
          if (o.size + sizeof(uint32_t) != size)
            return false;
          if (!o.size)
            o.data = nullptr;
          new(ptr) raw_data(o, neam::force_duplicate);
          return true;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static inline bool to_memory(memory_allocator &mem, size_t &size, const raw_data *ptr)
        {
          char *memory = reinterpret_cast<char *>(mem.allocate(ptr->size + sizeof(uint32_t)));
          if (!memory)
            return false;
          *reinterpret_cast<uint32_t *>(memory) = ptr->size;
          memcpy(memory + sizeof(uint32_t), ptr->data, ptr->size);
          size = ptr->size + sizeof(uint32_t);
          return true;
        }
    };

    /// \brief a special case for C strings
    template<>
    class persistence::serializable<persistence_backend::neam, char *>
    {
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static inline bool from_memory(const char *memory, size_t size, char **ptr)
        {
          *ptr = reinterpret_cast<char *>(operator new(size + 1, std::nothrow));
          if (*ptr)
          {
            memcpy(*ptr, memory, size);
            (*ptr)[size] = 0;
          }
          return !!*ptr;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        /// \note the stored string doesn't have the null byte stored (as we store its size instead)
        static inline bool to_memory(memory_allocator &mem, size_t &size, const char **ptr)
        {
          size = strlen(*ptr);
          char *memory = reinterpret_cast<char *>(mem.allocate(size));
          if (memory)
            memcpy(memory, *ptr, size);
          return !!memory;
        }
    };

    template<typename First, typename Second>
    class persistence::serializable<persistence_backend::neam, std::pair<First, Second>>
    {
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static inline bool from_memory(const char *memory, size_t size, std::pair<First, Second> *ptr)
        {
          if (size < 2 * sizeof(uint32_t))
            return false;

          uint32_t sz[2];
          sz[0] = *reinterpret_cast<const uint32_t *>(memory);
          if (size < sz[0] + 2 * sizeof(uint32_t))
            return false;
          sz[1] = *reinterpret_cast<const uint32_t *>(memory + sz[0] + sizeof(uint32_t));
          if (size < sz[0] + 2 * sizeof(uint32_t) + sz[1])
            return false;

          bool ret = serializable<persistence_backend::neam, First>::from_memory(memory + sizeof(uint32_t), sz[0], &ptr->first);
          if (!ret)
            return false;
          ret &= serializable<persistence_backend::neam, Second>::from_memory(memory + 2 * sizeof(uint32_t) + sz[0], sz[1], &ptr->second);
          if (!ret)
          {
            ptr->first.~First();
            return false;
          }

          return ret;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static inline bool to_memory(memory_allocator &mem, size_t &size, const std::pair<First, Second> *ptr)
        {
          size_t sz[2] = {0, 0};


          uint32_t *sz_mem = reinterpret_cast<uint32_t *>(mem.allocate(sizeof(uint32_t)));
          bool ret = serializable<persistence_backend::neam, First>::to_memory(mem, sz[0], &ptr->first);
          if (!ret)
            return false;
          *sz_mem = sz[0];

          sz_mem = reinterpret_cast<uint32_t *>(mem.allocate(sizeof(uint32_t)));
          ret = serializable<persistence_backend::neam, Second>::to_memory(mem, sz[1], &ptr->second);
          if (!ret)
            return false;
          *sz_mem = sz[1];

          size = sz[0] + sz[1] + 2 * sizeof(uint32_t);

          return true;
        }
    };

    /// \brief serialize an array and all its elements
    template<typename Type>
    class persistence::serializable<persistence_backend::neam, neam::array_wrapper<Type>>
    {
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static inline bool from_memory(const char *memory, size_t size, neam::array_wrapper<Type> *ptr)
        {
          if (size < sizeof(uint32_t))
            return false;

          ptr->size = *reinterpret_cast<const uint32_t *>(memory);
          ptr->array = reinterpret_cast<Type *>(operator new(ptr->size * sizeof(Type), std::nothrow));

          if (!ptr->array)
            return false;

          size_t offset = sizeof(uint32_t);
          size_t index = 0;
          for (; index < ptr->size; ++index)
          {
            if (offset + sizeof(uint32_t) > size)
              break;

            size_t elem_size = *reinterpret_cast<const uint32_t *>(memory + offset);
            offset += sizeof(uint32_t);

            if (offset + elem_size > size)
              break;
            if (!(serializable<persistence_backend::neam, Type>::from_memory(memory + offset, elem_size, ptr->array + index)))
              break;

            offset += elem_size;
          }

          if (index != ptr->size)
          {
            // call destructors to free allocated memory...
            for (size_t i = 0; i < index; ++i)
            {
              ptr->array[i].~Type();
            }

            delete reinterpret_cast<char *>(ptr->array);
            return false;
          }

          return true;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static inline bool to_memory(memory_allocator &mem, size_t &size, const neam::array_wrapper<Type> *ptr)
        {
          uint32_t whole_object_size = sizeof(uint32_t);

          *reinterpret_cast<uint32_t *>(mem.allocate(sizeof(uint32_t))) = ptr->size;

          for (size_t index = 0; index < ptr->size; ++index)
          {
            if (!to_memory_single(ptr->array + index, whole_object_size, mem))
              return false;
          }

          size = whole_object_size;

          return true;
        }


      private:
        static inline bool to_memory_single(const Type *ptr, uint32_t &global_size, memory_allocator &mem)
        {
          size_t element_size = 0;

          uint32_t *size_memory = reinterpret_cast<uint32_t *>(mem.allocate(sizeof(uint32_t)));
          if (!serializable<persistence_backend::neam, Type>::to_memory(mem, element_size, ptr))
            return false;

          *size_memory = element_size;
          global_size += element_size + sizeof(uint32_t);
          return true;
        }
    };
  } // namespace r
} // namespace neam

#endif /*__N_331927306113683385_1782336113__SERIALIZABLE_SPECS_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


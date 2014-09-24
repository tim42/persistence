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

#include <vector>
#include <string>
#include <map>
#include <unordered_map>

#include <tools/array_wrapper.hpp>
#include "object.hpp" // for my IDE
#include "raw_data.hpp"

namespace neam
{
  namespace cr
  {
    template<typename Type>
    class persistence::serializable<const Type>
    {
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static bool from_memory(const char *memory, size_t size, const Type *ptr)
        {
          return serializable<Type>::from_memory(memory, size, const_cast<Type *>(ptr));
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, const Type *ptr)
        {
          return serializable<Type>::to_memory(mem, size, (ptr));
        }
    };

    template<>
    class persistence::serializable<raw_data>
    {
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static bool from_memory(const char *memory, size_t size, raw_data *ptr)
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
        static bool to_memory(memory_allocator &mem, size_t &size, const raw_data *ptr)
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
    class persistence::serializable<char *>
    {
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static bool from_memory(const char *memory, size_t size, char **ptr)
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
        static bool to_memory(memory_allocator &mem, size_t &size, const char **ptr)
        {
          size = strlen(*ptr);
          char *memory = reinterpret_cast<char *>(mem.allocate(size));
          if (memory)
            memcpy(memory, *ptr, size);
          return !!memory;
        }
    };


    template<typename Type>
    class persistence::serializable<Type *>
    {
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static bool from_memory(const char *memory, size_t size, Type **ptr)
        {
          // handle the null pointer case
          if (!size)
          {
            *ptr = nullptr;
            return true;
          }

          Type *tptr = reinterpret_cast<Type *>(operator new(sizeof(Type), std::nothrow));
          if (!tptr)
            return false;
          *ptr = tptr;
          return serializable<Type>::from_memory(memory, size, tptr);
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, const Type* const* ptr)
        {
          // handle the null pointer case
          if (!*ptr)
          {
            size = 0;
            return true;
          }
          return serializable<Type>::to_memory(mem, size, *ptr);
        }
    };

    template<typename First, typename Second>
    class persistence::serializable<std::pair<First, Second>>
    {
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static bool from_memory(const char *memory, size_t size, std::pair<First, Second> *ptr)
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

          bool ret = serializable<First>::from_memory(memory + sizeof(uint32_t), sz[0], &ptr->first);
          ret &= serializable<Second>::from_memory(memory + 2 * sizeof(uint32_t) + sz[0], sz[1], &ptr->second);

          return ret;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, const std::pair<First, Second> *ptr)
        {
          const size_t original_sz = mem.size();
          size_t sz[2] = {0, 0};


          uint32_t *sz_mem = reinterpret_cast<uint32_t *>(mem.allocate(sizeof(uint32_t)));
          bool ret = serializable<First>::to_memory(mem, sz[0], &ptr->first);
          if (!ret)
          {
            mem.pop(mem.size() - original_sz);
            return false;
          }
          *sz_mem = sz[0];

          sz_mem = reinterpret_cast<uint32_t *>(mem.allocate(sizeof(uint32_t)));
          ret = serializable<Second>::to_memory(mem, sz[1], &ptr->second);
          if (!ret)
          {
            mem.pop(mem.size() - original_sz);
            return false;
          }
          *sz_mem = sz[1];

          size = sz[0] + sz[1] + 2 * sizeof(uint32_t);

          return true;
        }
    };

    template<typename Type>
    class persistence::serializable<std::vector<Type>>
    {
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static bool from_memory(const char *memory, size_t size, std::vector<Type> *ptr)
        {
          array_wrapper<Type> o(nullptr, 0);
          if (serializable<neam::array_wrapper<Type>>::from_memory(memory, size, &o))
          {
            new(ptr) std::vector<Type>();
            ptr->reserve(o.size);
            ptr->insert(ptr->begin(), o.array, o.array + o.size);
            delete o.array;
            return true;
          }
          return false;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, const std::vector<Type> *ptr)
        {
          array_wrapper<Type> o(const_cast<Type *>(ptr->data()), ptr->size());
          return serializable<neam::array_wrapper<Type>>::to_memory(mem, size, &o);
        }
    };

    template<>
    class persistence::serializable<std::string>
    {
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static bool from_memory(const char *memory, size_t size, std::string *ptr)
        {
          char *str = nullptr;
          if (serializable<char *>::from_memory(memory, size, &str))
          {
            new(ptr) std::string(str);
            delete str;
            return true;
          }
          return false;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, const std::string *ptr)
        {
          const char *str = const_cast<const char *>(ptr->data());
          return serializable<char *>::to_memory(mem, size, &str);
        }
    };

    template<typename Key, typename Value>
    class persistence::serializable<std::map<Key, Value>>
    {
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static bool from_memory(const char *memory, size_t size, std::map<Key, Value> *ptr)
        {
          neam::array_wrapper<std::pair<Key, Value>> tmp(nullptr, 0);
          if (!serializable<neam::array_wrapper<std::pair<Key, Value>>>::from_memory(memory, size, &tmp))
          {
            return false;
          }

          new(ptr) std::map<Key, Value>();

          for (size_t i = 0; i < tmp.size; ++i)
            ptr->emplace(std::move(tmp.array[i].first), std::move(tmp.array[i].second));

          delete tmp.array;

          return true;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, const std::map<Key, Value> *ptr)
        {
          std::vector<std::pair<const Key, Value> *> tmp;
          tmp.reserve(ptr->size());

          for (auto & it : *ptr)
            tmp.emplace_back(const_cast<std::pair<const Key, Value> *>(&it));

          return serializable<std::vector<std::pair<const Key, Value> *>>::to_memory(mem, size, &tmp);
        }
    };

    template<typename Key, typename Value>
    class persistence::serializable<std::unordered_map<Key, Value>>
    {
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static bool from_memory(const char *memory, size_t size, std::unordered_map<Key, Value> *ptr)
        {
          neam::array_wrapper<std::pair<Key, Value>> tmp(nullptr, 0);
          if (!serializable<neam::array_wrapper<std::pair<Key, Value>>>::from_memory(memory, size, &tmp))
          {
            return false;
          }

          new(ptr) std::unordered_map<Key, Value>();

          for (size_t i = 0; i < tmp.size; ++i)
            ptr->emplace(std::move(tmp.array[i].first), std::move(tmp.array[i].second));

          delete tmp.array;

          return true;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, const std::unordered_map<Key, Value> *ptr)
        {
          std::vector<std::pair<const Key, Value> *> tmp;
          tmp.reserve(ptr->size());

          for (auto & it : *ptr)
            tmp.emplace_back(const_cast<std::pair<const Key, Value> *>(&it));

          return serializable<std::vector<std::pair<const Key, Value> *>>::to_memory(mem, size, &tmp);
        }
    };

    /// \brief serialize an array and all its elements
    template<typename Type>
    class persistence::serializable<neam::array_wrapper<Type>>
    {
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static bool from_memory(const char *memory, size_t size, neam::array_wrapper<Type> *ptr)
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
            if (!(serializable<Type>::from_memory(memory + offset, elem_size, ptr->array + index)))
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
        static bool to_memory(memory_allocator &mem, size_t &size, const neam::array_wrapper<Type> *ptr)
        {
          uint32_t whole_object_size = sizeof(uint32_t);
          bool res = true;

          size_t original_sz = mem.size();

          uint32_t *obj_count = reinterpret_cast<uint32_t *>(mem.allocate(sizeof(uint32_t)));
          if (!obj_count)
            return false;
          *obj_count = ptr->size;

          for (size_t index = 0; index < ptr->size; ++index)
          {
            res &= to_memory_single(ptr->array + index, whole_object_size, mem);
            if (!res)
            {
              mem.pop(mem.size() - original_sz);
              return false;
            }
          }

          size = whole_object_size;

          return res;
        }


      private:
        static inline bool to_memory_single(const Type *ptr, uint32_t &global_size, memory_allocator &mem)
        {
          size_t element_size = 0;

          uint32_t *size_memory = reinterpret_cast<uint32_t *>(mem.allocate(sizeof(uint32_t)));
          if (!serializable<Type>::to_memory(mem, element_size, ptr))
          {
            mem.pop(sizeof(uint32_t));
            return false;
          }

          *size_memory = element_size;
          global_size += element_size + sizeof(uint32_t);
          return true;
        }
    };
  } // namespace r
} // namespace neam

#endif /*__N_331927306113683385_1782336113__SERIALIZABLE_SPECS_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


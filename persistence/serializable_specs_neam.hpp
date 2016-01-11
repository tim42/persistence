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
#include <tools/endianness.hpp>
#include "object.hpp" // for my IDE
#include "raw_data.hpp"

namespace neam
{
  namespace cr
  {
    namespace internal
    {
      struct numeric {};
      struct raw {};
    } // namespace internal

    /// \brief A dump serializer for the \e neam backend
    template<typename Type>
    class persistence::serializable<persistence_backend::neam, Type, internal::raw>
    {
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static inline bool from_memory(cr::allocation_transaction &, const char *memory, size_t size, Type *ptr)
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

    /// \brief the default number serializer for the \e neam backend. Handle endianness.
    template<typename Type>
    class persistence::serializable<persistence_backend::neam, Type, internal::numeric>
    {
      static_assert(std::is_arithmetic<Type>::value, "only arithmetic types here !!!");
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static inline bool from_memory(cr::allocation_transaction &, const char *memory, size_t size, Type *ptr)
        {
          if (size > sizeof(uint64_t) || (size & 1) != 0)
            return false;
          if (size == sizeof(Type))
            *ptr = ct::letoh(*reinterpret_cast<const Type *>(memory));
          else
          {
            if (size == sizeof(uint8_t)) // no 1 byte FP
              *ptr = ct::letoh(*reinterpret_cast<const typename std::conditional<std::is_unsigned<Type>::value, uint8_t, int8_t>::type *>(memory));
            else if (size == sizeof(uint16_t)) // no 2 bytes FP
              *ptr = ct::letoh(*reinterpret_cast<const typename std::conditional<std::is_unsigned<Type>::value, uint16_t, int16_t>::type *>(memory));
            else if (size == sizeof(uint32_t))
            {
              if (std::is_floating_point<Type>())
                *ptr = ct::letoh(*reinterpret_cast<const float *>(memory));
              else
                *ptr = ct::letoh(*reinterpret_cast<const typename std::conditional<std::is_unsigned<Type>::value, uint32_t, int32_t>::type *>(memory));
            }
            else if (size == sizeof(uint64_t))
            {
              if (std::is_floating_point<Type>())
                *ptr = ct::letoh(*reinterpret_cast<const double *>(memory));
              else
                *ptr = ct::letoh(*reinterpret_cast<const typename std::conditional<std::is_unsigned<Type>::value, uint64_t, int64_t>::type *>(memory));
            }
          }
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
          *reinterpret_cast<Type *>(memory) = ct::htole(*ptr);
          return true;
        }
    };

    // for arithmetic types
    template<> class persistence::serializable<persistence_backend::neam, int8_t> : public persistence::serializable<persistence_backend::neam, int8_t, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::neam, uint8_t> : public persistence::serializable<persistence_backend::neam, uint8_t, internal::numeric> {};

    template<> class persistence::serializable<persistence_backend::neam, int16_t> : public persistence::serializable<persistence_backend::neam, int16_t, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::neam, uint16_t> : public persistence::serializable<persistence_backend::neam, uint16_t, internal::numeric> {};

    template<> class persistence::serializable<persistence_backend::neam, int32_t> : public persistence::serializable<persistence_backend::neam, int32_t, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::neam, uint32_t> : public persistence::serializable<persistence_backend::neam, uint32_t, internal::numeric> {};

    template<> class persistence::serializable<persistence_backend::neam, int64_t> : public persistence::serializable<persistence_backend::neam, int64_t, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::neam, uint64_t> : public persistence::serializable<persistence_backend::neam, uint64_t, internal::numeric> {};

    template<> class persistence::serializable<persistence_backend::neam, float> : public persistence::serializable<persistence_backend::neam, float, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::neam, double> : public persistence::serializable<persistence_backend::neam, double, internal::numeric> {};
    // TODO:
    // template<> class persistence::serializable<persistence_backend::neam, long double> : public persistence::serializable<persistence_backend::neam, long double, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::neam, bool> : public persistence::serializable<persistence_backend::neam, int8_t, internal::numeric> {};


    template<>
    class persistence::serializable<persistence_backend::neam, raw_data>
    {
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static inline bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, raw_data *ptr)
        {
          if (size < sizeof(uint32_t))
            return false;
          raw_data o(*reinterpret_cast<const uint32_t *>(memory), reinterpret_cast<int8_t *>(const_cast<char *>(memory + sizeof(uint32_t))));
          if (o.size + sizeof(uint32_t) != size)
            return false;
          if (!o.size)
            o.data = nullptr;
          new(ptr) raw_data(o, neam::force_duplicate);
          transaction.register_destructor_call_on_failure(ptr);
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
        static inline bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, char **ptr)
        {
          *ptr = reinterpret_cast<char *>(transaction.allocate_raw(size + 1));
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

    /// \brief Helper to [de]serialize list-like & collection-like objects
    namespace persistence_helper
    {
      template<typename Type, typename Caller>
      class list_serializable<persistence_backend::neam, Type, Caller>
      {
        public:
          /// \brief Called to deserialize the list-object
          static inline bool from_memory(allocation_transaction &transaction, const char *memory, size_t size, Type *ptr)
          {
            if (size < sizeof(uint32_t))
              return false;

            const size_t element_count = *reinterpret_cast<const uint32_t *>(memory);
            if (!element_count)
              return Caller::from_memory_null(transaction, ptr);

            if (!Caller::from_memory_allocate(transaction, element_count, ptr))
              return false;

            size_t offset = sizeof(uint32_t);

            for (size_t index = 0; index < element_count; ++index)
            {
              if (offset + sizeof(uint32_t) > size)
                return false;

              const size_t elem_size = *reinterpret_cast<const uint32_t *>(memory + offset);
              offset += sizeof(uint32_t);

              if (offset + elem_size > size)
                return false;
              if (!Caller::from_memory_single(transaction, ptr, memory + offset, elem_size, index))
                return false;

              offset += elem_size;
            }

            return true;
          }

          /// \brief Called to serialize the list-object
          static inline bool to_memory(memory_allocator &mem, size_t &size, const Type *ptr)
          {
            uint32_t whole_object_size = sizeof(uint32_t);

            const size_t element_count = Caller::to_memory_get_element_count(ptr);
            *reinterpret_cast<uint32_t *>(mem.allocate(sizeof(uint32_t))) = element_count;

            auto iterator = Caller::to_memory_get_iterator(ptr);

            for (size_t index = 0; index < element_count; ++index)
            {
              size_t tmp_size = 0;
              uint32_t *size_memory = reinterpret_cast<uint32_t *>(mem.allocate(sizeof(uint32_t)));
              if (!Caller::to_memory_single(mem, tmp_size, iterator, ptr))
                return false;
              *size_memory = tmp_size;
              whole_object_size += tmp_size + sizeof(uint32_t);
              if (!Caller::to_memory_increment_iterator(iterator))
                return false;
            }

            size = whole_object_size;
            if (!Caller::to_memory_end_iterator(iterator))
              return false;
            return true;
          }

        private:
      };

      /// \brief Helper to [de]serialize collection-like objects
      /// In this backend, this is exactly as the list serializer. (code re-use ftw).
      template<typename Type, typename Caller>
      class collection_serializable<persistence_backend::neam, Type, Caller> : public list_serializable<persistence_backend::neam, Type, Caller> {};
    } // namespace persistence_helper
  } // namespace r
} // namespace neam

#endif /*__N_331927306113683385_1782336113__SERIALIZABLE_SPECS_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


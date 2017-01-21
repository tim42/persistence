//
// file : serializable_specs.hpp
// in : file:///home/tim/projects/reflective/reflective/persistence/serializable_specs.hpp
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

#ifndef __N_331927306113683385_1782336113__SERIALIZABLE_SPECS_HPP__
# define __N_331927306113683385_1782336113__SERIALIZABLE_SPECS_HPP__

#include <type_traits>
#include "tools/enable_if.hpp"
#include "tools/array_wrapper.hpp"
#include "tools/endianness.hpp"
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
          if (size == sizeof(Type))
            *ptr = ct::letoh(*reinterpret_cast<const Type *>(memory));
          else
          {
            switch (size)
            {
              case sizeof(uint8_t):
                *ptr = ct::letoh(*reinterpret_cast<const typename std::conditional<std::is_unsigned<Type>::value, uint8_t, int8_t>::type *>(memory));
                break;
              case sizeof(uint16_t):
                *ptr = ct::letoh(*reinterpret_cast<const typename std::conditional<std::is_unsigned<Type>::value, uint16_t, int16_t>::type *>(memory));
                break;
              case sizeof(uint32_t):
                if (std::is_floating_point<Type>())
                  *ptr = ct::letoh(*reinterpret_cast<const float *>(memory));
                else
                  *ptr = ct::letoh(*reinterpret_cast<const typename std::conditional<std::is_unsigned<Type>::value, uint32_t, int32_t>::type *>(memory));
                break;
              case sizeof(uint64_t):
                if (std::is_floating_point<Type>())
                  *ptr = ct::letoh(*reinterpret_cast<const double *>(memory));
                else
                  *ptr = ct::letoh(*reinterpret_cast<const typename std::conditional<std::is_unsigned<Type>::value, uint64_t, int64_t>::type *>(memory));
              default:
                return false;
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
          Type *memory = reinterpret_cast<Type *>(mem.allocate(size));
          if (!memory)
            return false;
          *memory = ct::htole(*ptr);
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
#if not defined(__GNUC__)
    template<> class persistence::serializable<persistence_backend::neam, unsigned long> : public persistence::serializable<persistence_backend::neam, unsigned long, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::neam, long> : public persistence::serializable<persistence_backend::neam, long, internal::numeric> {};
#endif
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
          *reinterpret_cast<uint32_t *>(memory) = uint32_t(ptr->size);
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
          if (!size)
          {
            *ptr = nullptr;
            return true;
          }

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
        static inline bool to_memory(memory_allocator &mem, size_t &size, const char *const*ptr)
        {
          if (!*ptr)
          {
            size = 0;
            return true;
          }

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
      template<typename Type, typename Caller, serializable_mode Mode>
      class list_serializable<persistence_backend::neam, Type, Caller, Mode>
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

            int8_t temp_memory[sizeof(typename Caller::single_instance_t)];
            typename Caller::single_instance_t *temp_memory_ptr = reinterpret_cast<typename Caller::single_instance_t *>(temp_memory);

            size_t offset = sizeof(uint32_t);

            for (size_t index = 0; index < element_count; ++index)
            {
              if (offset + sizeof(uint32_t) > size)
                return false;

              const size_t elem_size = *reinterpret_cast<const uint32_t *>(memory + offset);
              offset += sizeof(uint32_t);

              if (offset + elem_size > size)
                return false;
              if (!Caller::from_memory_single(transaction, ptr, temp_memory_ptr, memory + offset, elem_size, index))
                return false;

              offset += elem_size;
            }

            return Caller::from_memory_end(transaction, ptr);
          }


          /// \brief Called to serialize the list-object
          static inline bool to_memory(NCR_ENABLE_IF(!(Mode & to_memory_compiletime), memory_allocator) &mem, size_t &size, const Type *ptr)
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

          static inline bool to_memory(NCR_ENABLE_IF((Mode & to_memory_compiletime) != 0, memory_allocator) &mem, size_t &size, const Type *ptr)
          {
            size_t whole_object_size = sizeof(uint32_t);

            constexpr size_t element_count = Caller::compile_time_t::size;
            *reinterpret_cast<uint32_t *>(mem.allocate(sizeof(uint32_t))) = element_count;

            // This is way faster: no loops, type agnostic, ...
            if (!ct_to_memory_loop(gen_seq<element_count>(), mem, whole_object_size, ptr))
              return false;

            size = whole_object_size;
            return true;
          }

        private: // compile time thingies:
          template<size_t... Indexes>
          static inline bool ct_to_memory_loop(seq<Indexes...>, NCR_ENABLE_IF((Mode & to_memory_compiletime) != 0, memory_allocator) &mem, size_t &size, const Type *ptr)
          {
            bool res = true;
            NEAM_EXECUTE_PACK(
              (res &= ct_to_memory_single<Indexes>(mem, size, ptr))
            );
            return res;
          }

          template<size_t Index>
          static inline bool ct_to_memory_single(NCR_ENABLE_IF((Mode & to_memory_compiletime) != 0, memory_allocator) &mem, size_t &size, const Type *ptr)
          {
            size_t tmp_size = 0;
            uint32_t *size_memory = reinterpret_cast<uint32_t *>(mem.allocate(sizeof(uint32_t)));
            if (!Caller::compile_time_t::template get_type<Index>::to_memory_single(mem, tmp_size, ptr))
              return false;
            *size_memory = tmp_size;
            size += tmp_size + sizeof(uint32_t);
            return true;
          }
      };

      /// \brief Helper to [de]serialize collection-like objects
      template<typename Type, typename Caller, serializable_mode Mode>
      class collection_serializable<persistence_backend::neam, Type, Caller, Mode> : public list_serializable<persistence_backend::neam, Type, Caller, Mode>
      {
      };
    } // namespace persistence_helper
  } // namespace r
} // namespace neam

#endif /*__N_331927306113683385_1782336113__SERIALIZABLE_SPECS_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


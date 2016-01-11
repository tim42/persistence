//
// file : serializable_wrappers.hpp
// in : file:///home/tim/projects/reflective/reflective/persistence/serializable_wrappers.hpp
//
// created by : Timothée Feuillet on linux.site
// date: 21/09/2014 21:17:54
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

#ifndef __N_1932589144340359591_702583911__SERIALIZABLE_WRAPPERS_HPP__
# define __N_1932589144340359591_702583911__SERIALIZABLE_WRAPPERS_HPP__

#include <new>
#include "object.hpp"

namespace neam
{
  namespace cr
  {
    /// \brief add a magic number and check it
    template<typename Type> class magic {};

    /// \brief add a checksum and check it
    template<typename Type> class checksum {};

    /// \brief Xor the data
    template<typename Type, uint64_t Seed = 0xA1A598773F70B5DB> class xor_data {};

    template<typename Type>
    class persistence::serializable<persistence_backend::neam, magic<Type>>
    {
      private:
        constexpr static uint32_t magic_number = 0x07081992;

      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, Type *ptr)
        {
          if (size < sizeof(uint32_t))
            return false;
          uint32_t magic = *reinterpret_cast<const uint32_t *>(memory);

          if (magic != magic_number)
            return false;

          return serializable<persistence_backend::neam, Type>::from_memory(transaction, memory + sizeof(uint32_t), size - sizeof(uint32_t), const_cast<Type *>(ptr));
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, Type *ptr)
        {
          size_t o_size;

          uint32_t *magic = reinterpret_cast<uint32_t *>(mem.allocate(sizeof(uint32_t)));
          if (!magic)
            return false;

          if (serializable<persistence_backend::neam, Type>::to_memory(mem, o_size, const_cast<Type *>(ptr)))
          {
            size = o_size + sizeof(uint32_t);
            *magic = magic_number;
            return true;
          }
          return false;
        }
    };

    template<typename Backend, typename Type>
    class persistence::serializable<Backend, checksum<Type>>
    {
      private:
        constexpr static uint64_t init_checksum = 0x10F41A0995AA52F1;
        constexpr static size_t iteration_count = 13;

      private:
        /// \brief compute a simple, home-made checksum
        /// \note this checksum function does not intend to be without any weaknesses, bu to be simple enough to be in ~20 lines of code.
        /// \note yep, the number of iterations depends of the data AND its size.
        static uint64_t simple_checksum(const char *memory, size_t size)
        {
          uint64_t cc_checksum = init_checksum;

          // fill the 64b block
          for (size_t i = 0; i < size; ++i)
          {
            uint64_t t = static_cast<uint64_t>(memory[i]);
            cc_checksum ^= (t << ((i * 97) % (8 * sizeof(uint64_t)))) | (~t << ((i * 67) % (8 * sizeof(uint64_t))));
          }

          // run some iterations
          uint64_t t = ~init_checksum;
          for (size_t i = 0; i < iteration_count * (size % 503 + 1) + (t % 43); ++i)
          {
            t -= static_cast<uint64_t>(memory[(i * 37) % (size)]) << ((i * 97) % (8 * sizeof(uint64_t)));

            uint64_t f = ~(t * cc_checksum) | (t * (t >> ((i * 5) % (8))) * ~cc_checksum) >> (5 + (i * 13) % 32);
            cc_checksum = cc_checksum ^ f ^ t;
          }

          return cc_checksum;
        }

      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        template<typename... Params>
        static bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, Type *ptr, Params && ... p)
        {
          if (size < sizeof(uint64_t))
            return false;
          uint64_t checksum = *reinterpret_cast<const uint64_t *>(memory);


          if (checksum != simple_checksum(memory + sizeof(uint64_t), size - sizeof(uint64_t)))
            return false;

          return serializable<Backend, Type>::from_memory(transaction, memory + sizeof(uint64_t), size - sizeof(uint64_t), (ptr), std::forward<Params>(p)...);
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        template<typename... Params>
        static bool to_memory(memory_allocator &mem, size_t &size, const Type *ptr, Params && ... p)
        {
          size_t o_size = 0;

          size_t index = mem.size();
          uint64_t *checksum = reinterpret_cast<uint64_t *>(mem.allocate(sizeof(uint64_t)));
          if (!checksum)
            return false;


          if (serializable<Backend, Type>::to_memory(mem, o_size, (ptr)))
          {
            size = o_size + sizeof(uint64_t);

            checksum = reinterpret_cast<uint64_t *>(reinterpret_cast<uint8_t *>(mem.get_contiguous_data()) + index);
            *checksum = simple_checksum(reinterpret_cast<char *>(mem.get_contiguous_data()) + index + sizeof(uint64_t), o_size, std::forward<Params>(p)...);
            return true;
          }
          return false;
        }
    };

    /// \brief Xor wrapper. It simply xor the data by random number generated by a seedable PRNG
    template<typename Backend, typename Type, uint64_t Seed>
    class persistence::serializable<Backend, xor_data<Type, Seed>>
    {
      private:
        static inline uint8_t generator(uint64_t &seed)
        {
          seed += (seed * seed) | 5;
          uint32_t res = (seed >> 32); // ?
          return ((res & 0xFF) ^ ((res >> 8) & 0xFF)) ^ ~(((res >> 16) & 0xFF) ^ ((res >> 24) & 0xFF));
        }

        /// \brief This version does things in-place
        static inline char *xor_all_those_bytes(char *memory, size_t size)
        {
          uint64_t seed = Seed;
          for (size_t i = 0; i < size; ++i)
            memory[i] = memory[i] ^ generator(seed);
          return memory;
        }

        /// \brief This version works on another memory area
        static inline char *xor_all_those_bytes(const char *memory, size_t size)
        {
          char *res = reinterpret_cast<char *>(operator new(size, std::nothrow));
          if (!res)
            return nullptr;
          uint64_t seed = Seed;
          for (size_t i = 0; i < size; ++i)
            res[i] = memory[i] ^ generator(seed);
          return res;
        }

      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
//         template<typename... Params>
        static bool from_memory(allocation_transaction &transaction, const char *memory, size_t size, Type *ptr/*, Params && ... p*/)
        {
          const char *xored_memory;
          if (!(xored_memory = xor_all_those_bytes(memory, size)))
            return false;

          bool res;
          try
          {
            res = serializable<persistence_backend::neam, Type>::from_memory(transaction, xored_memory, size, ptr/*, std::forward<Params>(p)...*/);
          } catch (...)
          {
            delete xored_memory;
            throw;
          }
            delete xored_memory;
          return res;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        template<typename... Params>
        static bool to_memory(memory_allocator &mem, size_t &size, const Type *ptr, Params && ... p)
        {
          size_t o_size = 0;
          size_t index = mem.size();

          if (serializable<persistence_backend::neam, Type>::to_memory(mem, o_size, const_cast<Type *>(ptr), std::forward<Params>(p)...))
          {
            size = o_size;

            xor_all_those_bytes(reinterpret_cast<char *>(mem.get_contiguous_data()) + index, o_size);
            return true;
          }
          return false;
        }
    };
  } // namespace cr
} // namespace neam

#endif /*__N_1932589144340359591_702583911__SERIALIZABLE_WRAPPERS_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on;


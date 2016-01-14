//
// file : list_serializable.hpp
// in : file:///home/tim/projects/persistence/persistence/list_serializable.hpp
//
// created by : Timothée Feuillet on linux-vnd3.site
// date: 09/01/2016 14:46:01
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

#ifndef __N_13003974861317666095_479525736__LIST_SERIALIZABLE_HPP__
# define __N_13003974861317666095_479525736__LIST_SERIALIZABLE_HPP__

#include <cstdint>
#include <tools/allocation_transaction.hpp>
#include <tools/memory_allocator.hpp>

namespace neam
{
  namespace cr
  {
    namespace persistence_helper
    {
      enum serializable_mode
      {
        runtime = 0, // default
        from_memory_compiletime = 1 << 0,
        to_memory_compiletime =   1 << 1,
        compiletime = to_memory_compiletime | from_memory_compiletime
      };

      /// \brief Serialize / Deserialize some data that could be interpreted as a list of elements
      /// (C arrays, std::list/vector/array/tuple/..., array_wrapper, ...)
      /// \note don't specialize on either Type or Caller (except you have a \e really good reason to do so)
      template<typename Backend, typename Type, typename Caller, serializable_mode Mode = serializable_mode::runtime>
      class list_serializable
      {
        public:
          /// \brief Called to deserialize the list-object
          /// It must call some Caller methods at some specific time (hooks):
          ///   bool Caller::from_memory_null(cr::allocation_transaction &transaction, Type *ptr) // When the size is 0 or no data is given
          ///   bool Caller::from_memory_allocate(cr::allocation_transaction &transaction, size_t size, Type *ptr) // When the number of element is known, to allocate the memory for them / call the constructor
          ///   bool Caller::from_memory_single(cr::allocation_transaction &transaction, Type *ptr, const char *sub_memory, size_t sub_size, size_t index, Params &&...p) // Handle the deserialization of a single element
          ///   bool Caller::from_memory_end(cr::allocation_transaction &transaction, Type *ptr) // always called at the very end
          template<typename... Params>
          static inline bool from_memory(allocation_transaction &transaction, const char *memory, size_t size, Type *ptr, Params && ... )
          {
            (void)transaction;
            (void)memory;
            (void)size;
            (void)ptr;
            return false;
          }

          /// \brief Called to serialize the list-object
          /// It must call some Caller methods at some specific time (hooks):
          ///   Caller::IteratorType Caller::to_memory_get_iterator(const Type *ptr)
          ///   size_t Caller::to_memory_get_element_count(const Type *ptr) // facultative
          ///   bool Caller::to_memory_increment_iterator(Caller::IteratorType &it)
          ///   bool Caller::to_memory_single(memory_allocator &mem, size_t &size, Caller::IteratorType &it, const Type *ptr, Params && ... p)
          ///   bool Caller::to_memory_end_iterator(Caller::IteratorType &it)
          template<typename... Params>
          static inline bool to_memory(memory_allocator &mem, size_t &size, const Type *ptr, Params && ... )
          {
            static_assert(std::is_same<Caller, void>::value, "Well, that type is not valid");
            (void)mem;
            (void)size;
            (void)ptr;
            return false;
          }
      };
    } // namespace persistence_internal
  } // namespace cr
} // namespace neam

#endif /*__N_13003974861317666095_479525736__LIST_SERIALIZABLE_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


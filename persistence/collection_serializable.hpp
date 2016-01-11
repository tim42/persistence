//
// file : collection_serializable.hpp
// in : file:///home/tim/projects/persistence/persistence/collection_serializable.hpp
//
// created by : Timothée Feuillet on linux-vnd3.site
// date: 10/01/2016 11:57:03
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

#ifndef __N_929154267943137924_1343703518__COLLECTION_SERIALIZABLE_HPP__
# define __N_929154267943137924_1343703518__COLLECTION_SERIALIZABLE_HPP__

#include <cstdint>
#include <tools/allocation_transaction.hpp>
#include <tools/memory_allocator.hpp>

namespace neam
{
  namespace cr
  {
    namespace persistence_helper
    {
      /// \brief Serialize / Deserialize some data that could be interpreted as a collection of elements
      /// (dictionary, std::map/unordered_map, maybe even objects)
      /// The problem with that kind of thing is that the destination format can impose restrictions on the key type
      /// and thus, the system have to support both a JSON like {"key": [value], ...} structure and a more generic array of std::pair< Key, value >
      /// if the key type can't be used as a key in the output format.
      template<typename Backend, typename Type, typename Caller>
      class collection_serializable
      {
        public:
          /// \brief Called to deserialize the collection-object
          /// It must call some Caller methods at some specific time (hooks):
          ///   bool Caller::from_memory_null(cr::allocation_transaction &transaction, Type *ptr) // When the size is 0 or no data is given
          ///   bool Caller::from_memory_allocate(cr::allocation_transaction &transaction, size_t, Type *ptr)
          ///     One of those two functions/group of function may be called:
          ///   bool Caller::from_memory_single(cr::allocation_transaction &transaction, Type *ptr, const char *sub_memory, size_t sub_size, Params &&...p)
          ///     or
          ///   size_t Caller::from_memory_get_kv_pair_size(Type *ptr) // this return the size of the memory needed to hold both the key and the value (could be a std::pair)
          ///   bool Caller::from_memory_single_key(cr::allocation_transaction &transaction, Type *ptr, void *pair, const char *k_memory, size_t k_size, Params &&...k_p)
          ///   bool Caller::from_memory_single_value(cr::allocation_transaction &transaction, Type *ptr, void *pair, const char *v_memory, size_t v_size, Params &&...v_p)
          ///   bool Caller::from_memory_single_push_kv(cr::allocation_transaction &transaction, Type *ptr, void *pair) // always called last: push the generated pair to the collection
          template<typename... Params>
          static inline bool from_memory(allocation_transaction &transaction, const char *memory, size_t size, Type *ptr, Params && ... p)
          {
            (void)transaction;
            (void)memory;
            (void)size;
            (void)ptr;
            return false;
          }

          /// \brief Called to serialize the collection-object
          /// It must call some Caller methods at some specific time (hooks):
          ///   Caller::IteratorType Caller::to_memory_get_iterator(const Type *ptr)
          ///   bool Caller::to_memory_increment_iterator(Caller::IteratorType &it)
          ///   bool Caller::to_memory_single(memory_allocator &mem, size_t &size, Caller::IteratorType &it, const Type *ptr, Params && ... p) // serialize a std::pair< Key, Value >
          ///     to_memory_single() could be replaced by the two following calls if Caller::ShouldBeSerializedAsCollection is true
          ///   bool Caller::to_memory_single_key(memory_allocator &mem, size_t &size, Caller::IteratorType &it, const Type *ptr, Params && ... p)
          ///   bool Caller::to_memory_single_value(memory_allocator &mem, size_t &size, Caller::IteratorType &it, const Type *ptr, Params && ... p)
          ///     then
          ///   bool Caller::to_memory_end_iterator(Caller::IteratorType &it)
          template<typename... Params>
          static inline bool to_memory(memory_allocator &mem, size_t &size, const Type *ptr, Params && ... p)
          {
            (void)mem;
            (void)size;
            (void)ptr;
            return false;
          }
      };

    } // namespace persistence_internal
  } // namespace cr
} // namespace neam

#endif /*__N_929154267943137924_1343703518__COLLECTION_SERIALIZABLE_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


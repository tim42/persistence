//
// file : collection_serializable.hpp
// in : file:///home/tim/projects/persistence/persistence/collection_serializable.hpp
//
// created by : Timothée Feuillet on linux-vnd3.site
// date: 10/01/2016 11:57:03
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

#ifndef __N_929154267943137924_1343703518__COLLECTION_SERIALIZABLE_HPP__
# define __N_929154267943137924_1343703518__COLLECTION_SERIALIZABLE_HPP__

#include <type_traits>
#include <cstdint>
#include "tools/allocation_transaction.hpp"
#include "tools/memory_allocator.hpp"

#include "list_serializable.hpp"

namespace neam
{
  namespace cr
  {
    namespace persistence_helper
    {
      /// \brief Is the given type will give a string if it is used as a key
      /// \note this is specialisable on the backend to allow fine grained, per backend, control.
      /// The default is having everything giving a false result
      template<typename Backend, typename Type> struct should_be_serialized_as_collection : public std::false_type {};

      /// \brief Serialize / Deserialize some data that could be interpreted as a collection of elements
      /// (dictionary, std::map/unordered_map, maybe even objects)
      /// The problem with that kind of thing is that the destination format can impose restrictions on the key type
      /// and thus, the system have to support both a JSON like {"key": [value], ...} structure and a more generic array of std::pair< Key, value >
      /// if the key type can't be used as a key in the output format.
      template<typename Backend, typename Type, typename Caller, serializable_mode Mode = serializable_mode::runtime>
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
          static inline bool to_memory(memory_allocator &mem, size_t &size, const Type *ptr, Params && ... )
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


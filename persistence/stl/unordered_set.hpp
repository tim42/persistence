//
// file : unordered_set.hpp
// in : file:///home/tim/projects/persistence/persistence/stl/unordered_set.hpp
//
// created by : Timothée Feuillet
// date: Sun Jul 10 2016 14:47:38 GMT+0200 (CEST)
//
//
// Copyright (c) 2016 Timothée Feuillet
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

#ifndef __N_15733213241602228936_799732757_UNORDERED_SET_HPP__
#define __N_15733213241602228936_799732757_UNORDERED_SET_HPP__

#include <unordered_set>
#include "../object.hpp"

namespace neam
{
  namespace cr
  {
    template<typename Backend, typename Key, typename Hash, typename KeyEqual, typename Alloc>
    class persistence::serializable<Backend, std::unordered_set<Key, Hash, KeyEqual, Alloc>>
          : public persistence_helper::collection_serializable<Backend, std::unordered_set<Key, Hash, KeyEqual, Alloc>, persistence::serializable<Backend, std::unordered_set<Key, Hash, KeyEqual, Alloc>>>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &transaction, std::unordered_set<Key, Hash, KeyEqual, Alloc> *ptr)
        {
          new(ptr) std::unordered_set<Key, Hash, KeyEqual, Alloc>();
          transaction.register_destructor_call_on_failure(ptr);
          return true;
        }

        static inline bool from_memory_null(cr::allocation_transaction &transaction, std::unordered_set<Key, Hash, KeyEqual, Alloc> *ptr) // When the size is 0 or no data is given
        {
          return default_initializer(transaction, ptr);
        }

        static inline bool from_memory_allocate(cr::allocation_transaction &transaction, size_t, std::unordered_set<Key, Hash, KeyEqual, Alloc> *ptr)
        {
          // can't preallocate memory on std::unorderer_set
          return default_initializer(transaction, ptr);
        }

        using single_instance_t = Key;

        template<typename... Params>
        static inline bool from_memory_single(cr::allocation_transaction &, std::unordered_set<Key, Hash, KeyEqual, Alloc> *ptr, single_instance_t *data, const char *sub_memory, size_t sub_size, size_t, Params &&...p)
        {
          cr::allocation_transaction temp_transaction;
          if (persistence::serializable<Backend, Key>::from_memory(temp_transaction, sub_memory, sub_size, data, std::forward<Params>(p)...))
          {
            // The hint will always be true if the data is serialized by persistence.
            // So we insert in amortized O(1)
            ptr->emplace_hint(ptr->end(), std::move(*data));
            temp_transaction.rollback();
            return true;
          }
          temp_transaction.rollback();
          return false;
        }

        using kv_instance_t = Key;
        static constexpr bool can_construct_inplace = false;

        template<typename... Params>
        static inline bool from_memory_single_key(cr::allocation_transaction &transaction, std::unordered_set<Key, Hash, KeyEqual, Alloc> *, kv_instance_t *key, const char *k_memory, size_t k_size, Params &&...k_p)
        {
          if (persistence::serializable<Backend, Key>::from_memory(transaction, k_memory, k_size, (key), std::forward<Params>(k_p)...))
            return true;
          return false;
        }

        template<typename... Params>
        static inline bool from_memory_single_value(cr::allocation_transaction &, std::unordered_set<Key, Hash, KeyEqual, Alloc> *, kv_instance_t *, const char *, size_t , Params &&...)
        {
          return true;
        }

        static inline bool from_memory_single_push_kv(cr::allocation_transaction &, std::unordered_set<Key, Hash, KeyEqual, Alloc> *ptr, kv_instance_t *key)
        {
          ptr->emplace_hint(ptr->end(), std::move(*key));
          return true;
        }

        static inline bool from_memory_end(cr::allocation_transaction &, std::unordered_set<Key, Hash, KeyEqual, Alloc> *)
        {
          return true;
        }


        using iterator_t = typename std::unordered_set<Key, Hash, KeyEqual, Alloc>::const_iterator;
        static constexpr bool should_be_serialized_as_collection = false;

        static inline iterator_t to_memory_get_iterator(const std::unordered_set<Key, Hash, KeyEqual, Alloc> *ptr)
        {
          return ptr->begin();
        }

        static inline size_t to_memory_get_element_count(const std::unordered_set<Key, Hash, KeyEqual, Alloc> *ptr)
        {
          return ptr->size();
        }

        static inline bool to_memory_increment_iterator(iterator_t &it)
        {
          ++it;
          return true;
        }

        template<typename... Params>
        static inline bool to_memory_single(memory_allocator &mem, size_t &size, iterator_t &it, const std::unordered_set<Key, Hash, KeyEqual, Alloc> *ptr, Params && ... p)
        {
          if (it == ptr->end())
            return false;
          return persistence::serializable<Backend, const Key>::to_memory(mem, size, &(*it), std::forward<Params>(p)...);
        }

        template<typename... Params>
        static inline bool to_memory_single_key(memory_allocator &mem, size_t &size, iterator_t &it, const std::unordered_set<Key, Hash, KeyEqual, Alloc> *ptr, Params && ... p)
        {
          if (it == ptr->end())
            return false;
          return persistence::serializable<Backend, Key>::to_memory(mem, size, &(*it), std::forward<Params>(p)...);
        }

        template<typename... Params>
        static inline bool to_memory_single_value(memory_allocator &, size_t &, iterator_t &, const std::unordered_set<Key, Hash, KeyEqual, Alloc> *, Params && ... )
        {
          return true;
        }

        static inline bool to_memory_end_iterator(iterator_t &)
        {
          return true;
        }
    };
  } // namespace cr
} // namespace neam

#endif // __N_15733213241602228936_799732757_UNORDERED_SET_HPP__


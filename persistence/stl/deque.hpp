//
// file : deque.hpp
// in : file:///home/tim/projects/persistence/persistence/stl/deque.hpp
//
// created by : Timothée Feuillet
// date: Sun Jul 10 2016 13:49:03 GMT+0200 (CEST)
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

#ifndef __N_2190818608955430901_1410128734_DEQUE_HPP__
#define __N_2190818608955430901_1410128734_DEQUE_HPP__

#include <deque>
#include "../object.hpp"

namespace neam
{
  namespace cr
  {
    template<typename Backend, typename Type, typename Alloc>
    class persistence::serializable<Backend, std::deque<Type, Alloc>>
          : public persistence_helper::list_serializable<Backend, std::deque<Type, Alloc>, persistence::serializable<Backend, std::deque<Type, Alloc>>>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &transaction, std::deque<Type, Alloc> *ptr)
        {
          new(ptr) std::deque<Type, Alloc>();
          transaction.register_destructor_call_on_failure(ptr);
          return true;
        }

        static inline bool from_memory_null(cr::allocation_transaction &transaction, std::deque<Type, Alloc> *ptr) // When the size is 0 or no data is given
        {
          return default_initializer(transaction, ptr);
        }

        static inline bool from_memory_allocate(cr::allocation_transaction &transaction, size_t, std::deque<Type, Alloc> *ptr)
        {
          return default_initializer(transaction, ptr);
        }

        using single_instance_t = Type;

        template<typename... Params>
        static inline bool from_memory_single(cr::allocation_transaction &, std::deque<Type, Alloc> *ptr, single_instance_t *data, const char *sub_memory, size_t sub_size, size_t, Params &&...p)
        {
          cr::allocation_transaction temp_transaction;
          if (persistence::serializable<Backend, Type>::from_memory(temp_transaction, sub_memory, sub_size, data, std::forward<Params>(p)...))
          {
            ptr->emplace_back(std::move(*data));
            temp_transaction.rollback();
            return true;
          }
          temp_transaction.rollback();
          return false;
        }

        static inline bool from_memory_end(cr::allocation_transaction &, std::deque<Type, Alloc> *)
        {
          return true;
        }


        static inline size_t to_memory_get_iterator(const std::deque<Type, Alloc> *)
        {
          return 0;
        }

        static inline size_t to_memory_get_element_count(const std::deque<Type, Alloc> *ptr)
        {
          return ptr->size();
        }

        static inline bool to_memory_increment_iterator(size_t &it)
        {
          ++it;
          return true;
        }

        template<typename... Params>
        static inline bool to_memory_single(memory_allocator &mem, size_t &size, size_t &it, const std::deque<Type, Alloc> *ptr, Params && ... p)
        {
          if (it >= ptr->size())
            return false;
          return persistence::serializable<Backend, Type>::to_memory(mem, size, &((*ptr)[it]), std::forward<Params>(p)...);
        }

        static inline bool to_memory_end_iterator(size_t &)
        {
          return true;
        }
    };
  } // namespace cr
} // namespace neam

#endif // __N_2190818608955430901_1410128734_DEQUE_HPP__

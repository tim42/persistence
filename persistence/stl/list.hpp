//
// file : list.hpp
// in : file:///home/tim/projects/persistence/persistence/stl/list.hpp
//
// created by : Timothée Feuillet
// date: Sun Jul 10 2016 14:17:40 GMT+0200 (CEST)
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

#ifndef __N_10773127262057626843_3096714911_LIST_HPP__
#define __N_10773127262057626843_3096714911_LIST_HPP__

#include <list>
#include "../object.hpp"

namespace neam
{
  namespace cr
  {
    template<typename Backend, typename Type, typename Alloc>
    class persistence::serializable<Backend, std::list<Type, Alloc>>
          : public persistence_helper::list_serializable<Backend, std::list<Type, Alloc>, persistence::serializable<Backend, std::list<Type, Alloc>>>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &transaction, std::list<Type, Alloc> *ptr)
        {
          new(ptr) std::list<Type, Alloc>();
          transaction.register_destructor_call_on_failure(ptr);
          return true;
        }

        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        template<typename... Params>
        static inline bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, std::list<Type, Alloc> *ptr, Params &&... p)
        {
          array_wrapper<Type> o(nullptr, 0);
          cr::allocation_transaction temp_transaction;
          if (serializable<Backend, neam::array_wrapper<Type>>::from_memory(temp_transaction, memory, size, &o, std::forward<Params>(p)...))
          {
            new(ptr) std::list<Type, Alloc>();
            transaction.register_destructor_call_on_failure(ptr);
            ptr->insert(ptr->begin(), o.array, o.array + o.size);
//             for (size_t i = 0; i < o.size; ++i)
//               o.array[i].~Type();
            temp_transaction.rollback(); // free up the temporary memory
            return true;
          }
          return false;
        }

        using iterator_type = typename std::list<Type, Alloc>::const_iterator;

        static inline iterator_type to_memory_get_iterator(const std::list<Type, Alloc> *ptr)
        {
          return ptr->cbegin();
        }

        static inline size_t to_memory_get_element_count(const std::list<Type, Alloc> *ptr)
        {
          return ptr->size();
        }

        static inline bool to_memory_increment_iterator(iterator_type &it)
        {
          ++it;
          return true;
        }

        template<typename... Params>
        static inline bool to_memory_single(memory_allocator &mem, size_t &size, iterator_type &it, const std::list<Type, Alloc> *ptr, Params && ... p)
        {
          if (it == ptr->end())
            return false;
          return persistence::serializable<Backend, Type>::to_memory(mem, size, &*it, std::forward<Params>(p)...);
        }

        static inline bool to_memory_end_iterator(iterator_type &)
        {
          return true;
        }
    };
  } // namespace cr
} // namespace neam

#endif // __N_10773127262057626843_3096714911_LIST_HPP__


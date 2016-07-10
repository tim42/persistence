//
// file : array.hpp
// in : file:///home/tim/projects/persistence/persistence/stl/array.hpp
//
// created by : Timothée Feuillet
// date: Sun Jul 10 2016 13:53:57 GMT+0200 (CEST)
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

#ifndef __N_32394286571051232015_60701401_ARRAY_HPP__
#define __N_32394286571051232015_60701401_ARRAY_HPP__

#include <array>
#include "../object.hpp"

namespace neam
{
  namespace cr
  {
    /// \note std::array have the particularity to wrap a C array and ONLY a C array,
    ///       so we can avoid to call the constructor and perform in-place deserialization
    ///       as for C arrays
    template<typename Backend, typename Type, size_t Size>
    class persistence::serializable<Backend, std::array<Type, Size>>
      : public persistence_helper::list_serializable<Backend, std::array<Type, Size>, persistence::serializable<Backend, std::array<Type, Size>>>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &transaction, std::array<Type, Size> *array)
        {
          new (array) std::array<Type, Size>();
          transaction.register_destructor_call_on_failure(array);
          return true;
        }

        static inline bool from_memory_null(cr::allocation_transaction &transaction, std::array<Type, Size> *array) // When the size is 0 or no data is given
        {
          return default_initializer(transaction, array);
        }

        static inline bool from_memory_allocate(cr::allocation_transaction &, size_t size, std::array<Type, Size> *)
        {
          return size == Size;
        }

        using single_instance_t = int8_t;

        template<typename... Params>
        static inline bool from_memory_single(cr::allocation_transaction &transaction, std::array<Type, Size> *array, int8_t *, const char *sub_memory, size_t sub_size, size_t index, Params &&...p)
        {
          if (index >= Size)
            return false;

          return persistence::serializable<Backend, Type>::from_memory(transaction, sub_memory, sub_size, &((*array)[index]), std::forward<Params>(p)...);
        }

        static inline bool from_memory_end(cr::allocation_transaction &, std::array<Type, Size> *)
        {
          return true;
        }


        static inline size_t to_memory_get_iterator(const std::array<Type, Size> *)
        {
          return 0;
        }

        static inline size_t to_memory_get_element_count(const std::array<Type, Size> *)
        {
          return Size;
        }

        static inline bool to_memory_increment_iterator(size_t &it)
        {
          ++it;
          return true;
        }

        template<typename... Params>
        static inline bool to_memory_single(memory_allocator &mem, size_t &size, size_t &it, const std::array<Type, Size> *array, Params && ... p)
        {
          if (it >= Size)
            return false;
          return persistence::serializable<Backend, Type>::to_memory(mem, size, &((*array)[it]), std::forward<Params>(p)...);
        }

        static inline bool to_memory_end_iterator(size_t &)
        {
          return true;
        }
    };
  } // namespace cr
} // namespace neam

#endif // __N_32394286571051232015_60701401_ARRAY_HPP__


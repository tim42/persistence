//
// file : vector.hpp
// in : file:///home/tim/projects/persistence/persistence/stl/vector.hpp
//
// created by : Timothée Feuillet
// date: Sun Jul 10 2016 13:44:31 GMT+0200 (CEST)
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

#ifndef __N_1661224151227918917_2842922057_VECTOR_HPP__
#define __N_1661224151227918917_2842922057_VECTOR_HPP__

#include <vector>

#include "../object.hpp"
#include "../tools/array_wrapper.hpp"

namespace neam
{
  namespace cr
  {
    /// \note I now this is super weird, but this version is FASTER than the "correct way" to do it (element by element).
    /// It is (maybe) 'cause it does one batch of insertions...
    template<typename Backend, typename Type, typename Alloc>
    class persistence::serializable<Backend, std::vector<Type, Alloc>>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &transaction, std::vector<Type, Alloc> *ptr)
        {
          new(ptr) std::vector<Type, Alloc>();
          transaction.register_destructor_call_on_failure(ptr);
          return true;
        }

        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        template<typename... Params>
        static inline bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, std::vector<Type, Alloc> *ptr, Params &&... p)
        {
          array_wrapper<Type> o(nullptr, 0);
          cr::allocation_transaction temp_transaction;
          if (serializable<Backend, neam::array_wrapper<Type>>::from_memory(temp_transaction, memory, size, &o, std::forward<Params>(p)...))
          {
            new(ptr) std::vector<Type, Alloc>();
            transaction.register_destructor_call_on_failure(ptr);
            ptr->reserve(o.size + 1);
            ptr->insert(ptr->begin(), o.array, o.array + o.size);
            temp_transaction.rollback(); // free up the temporary memory
            return true;
          }
          return false;
        }

        /// \brief serialize the object
        /// \param[out] mem the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        template<typename... Params>
        static inline bool to_memory(memory_allocator &mem, size_t &size, const std::vector<Type, Alloc> *ptr, Params &&... p)
        {
          array_wrapper<Type> o(const_cast<Type *>(ptr->data()), ptr->size());
          return serializable<Backend, neam::array_wrapper<Type>>::to_memory(mem, size, &o, std::forward<Params>(p)...);
        }
    };
  } // namespace cr
} // namespace neam

#endif // __N_1661224151227918917_2842922057_VECTOR_HPP__


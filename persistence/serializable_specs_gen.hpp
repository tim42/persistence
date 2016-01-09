//
// file : serializable_specs_gen.hpp
// in : file:///home/tim/projects/persistence/persistence/serializable_specs_gen.hpp
//
// created by : Timothée Feuillet on linux.site
// date: 24/09/2014 16:21:02
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

#ifndef __N_17535733811409347418_794184241__SERIALIZABLE_SPECS_GEN_HPP__
# define __N_17535733811409347418_794184241__SERIALIZABLE_SPECS_GEN_HPP__

#include <vector>
#include <string>
#include <map>
#include <unordered_map>

#include <tools/array_wrapper.hpp>
#include "object.hpp" // for my IDE


// backend independent specialisations
namespace neam
{
  namespace cr
  {
    template<typename Backend, typename Type>
    class persistence::serializable<Backend, const Type>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &transaction, const Type *ptr)
        {
          return serializable<Backend, Type>::default_initializer(transaction, const_cast<Type *>(ptr));
        }

        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        template<typename... Params>
        static inline bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, const Type *ptr, Params &&... p)
        {
          return serializable<Backend, Type>::from_memory(transaction, memory, size, const_cast<Type *>(ptr), std::forward<Params>(p)...);
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        template<typename... Params>
        static inline bool to_memory(memory_allocator &mem, size_t &size, const Type *ptr, Params &&... p)
        {
          return serializable<Backend, Type>::to_memory(mem, size, (ptr), std::forward<Params>(p)...);
        }
    };

    template<typename Backend, typename Type>
    class persistence::serializable<Backend, volatile Type>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &transaction, volatile Type *ptr)
        {
          return serializable<Backend, Type>::default_initializer(transaction, reinterpret_cast<Type *>(ptr));
        }

        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        template<typename... Params>
        static inline bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, volatile Type *ptr, Params &&... p)
        {
          return serializable<Backend, Type>::from_memory(transaction, memory, size, reinterpret_cast<Type *>(ptr), std::forward<Params>(p)...);
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        template<typename... Params>
        static inline bool to_memory(memory_allocator &mem, size_t &size, const volatile Type *ptr, Params &&... p)
        {
          return serializable<Backend, Type>::to_memory(mem, size, (const Type *)(ptr), std::forward<Params>(p)...);
        }
    };

    template<typename Backend, typename Type>
    class persistence::serializable<Backend, Type *>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &, Type **ptr)
        {
          *ptr = nullptr;
          return true;
        }

        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        template<typename... Params>
        static inline bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, Type **ptr, Params &&... p)
        {
          // handle the null pointer case
          if (!size)
          {
            *ptr = nullptr;
            return true;
          }

          Type *tptr = reinterpret_cast<Type *>(transaction.allocate_raw(sizeof(Type)));
          if (!tptr)
            return false;
          *ptr = tptr;
          return serializable<Backend, Type>::from_memory(transaction, memory, size, tptr, std::forward<Params>(p)...);
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        template<typename... Params>
        static inline bool to_memory(memory_allocator &mem, size_t &size, const Type* const* ptr, Params &&... p)
        {
          // handle the null pointer case
          if (!*ptr)
          {
            size = 0;
            return true;
          }
          return serializable<Backend, Type>::to_memory(mem, size, *ptr, std::forward<Params>(p)...);
        }
    };

    template<typename Backend, typename Type>
    class persistence::serializable<Backend, Type &>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &, Type *&ptr)
        {
          return false;
        }

        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        template<typename... Params>
        static inline bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, Type *&ptr, Params &&... p)
        {
          Type *tptr = reinterpret_cast<Type *>(transaction.allocate_raw(sizeof(Type)));
          if (!tptr)
            return false;
          *ptr = tptr;
          return serializable<Backend, Type>::from_memory(transaction, memory, size, tptr, std::forward<Params>(p)...);
        }

        /// \brief deserialize the object and return it
        /// \param[in,out] mem the allocator used to allocate the object
        /// \return *ptr. always.
        template<typename... Params>
        static inline Type &from_memory(cr::allocation_transaction &transaction, memory_allocator &mem, Params &&... p)
        {
          return serializable<Backend, Type>::from_memory(transaction, mem, std::forward<Params>(p)...);
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        template<typename... Params>
        static inline bool to_memory(memory_allocator &mem, size_t &size, const Type* const& ptr, Params &&... p)
        {
          return serializable<Backend, Type>::to_memory(mem, size, *ptr, std::forward<Params>(p)...);
        }
    };

    template<typename Backend, typename Type, typename Alloc>
    class persistence::serializable<Backend, std::vector<Type, Alloc>>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &transaction, std::vector<Type> *ptr)
        {
          new(ptr)  std::vector<Type>();
          transaction.register_destructor_call_on_failure(ptr);
          return true;
        }

        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        template<typename... Params>
        static inline bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, std::vector<Type> *ptr, Params &&... p)
        {
          array_wrapper<Type> o(nullptr, 0);
          cr::allocation_transaction temp_transaction;
          if (serializable<Backend, neam::array_wrapper<Type>>::from_memory(temp_transaction, memory, size, &o, std::forward<Params>(p)...))
          {
            new(ptr) std::vector<Type, Alloc>();
            transaction.register_destructor_call_on_failure(ptr);
            ptr->reserve(o.size);
            for (size_t i = 0; i < o.size; ++i)
              ptr->emplace_back(std::move(o.array[i]));
            temp_transaction.rollback();
            return true;
          }
          temp_transaction.rollback();
          return false;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        template<typename... Params>
        static inline bool to_memory(memory_allocator &mem, size_t &size, const std::vector<Type> *ptr, Params &&... p)
        {
          array_wrapper<Type> o(const_cast<Type *>(ptr->data()), ptr->size());
          return serializable<Backend, neam::array_wrapper<Type>>::to_memory(mem, size, &o, std::forward<Params>(p)...);
        }
    };

    template<typename Backend, typename Type, size_t Size>
    class persistence::serializable<Backend, Type[Size]>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &, Type (*array)[Size])
        {
          // TODO
          return false;
        }

        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        template<typename... Params>
        static inline bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, Type (*array)[Size], Params &&... p)
        {
          array_wrapper<Type> o(nullptr, 0);
          o.size = Size;
          o.array = reinterpret_cast<Type *>(array);
          if (!serializable<Backend, neam::array_wrapper<Type>>::from_memory_preallocated(transaction, memory, size, &o, std::forward<Params>(p)...))
            return false;
          return true;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        template<typename... Params>
        static inline bool to_memory(memory_allocator &mem, size_t &size, const Type (*array)[Size], Params &&... p)
        {
          array_wrapper<Type> o(const_cast<Type *>(reinterpret_cast<const Type *>(array)), Size);
          return serializable<Backend, neam::array_wrapper<Type>>::to_memory(mem, size, &o, std::forward<Params>(p)...);
        }
    };

    template<typename Backend, typename CharT, typename Traits, typename Alloc>
    class persistence::serializable<Backend, std::basic_string<CharT, Traits, Alloc>>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &transaction, std::basic_string<CharT, Traits, Alloc> *ptr)
        {
          new(ptr) std::basic_string<CharT, Traits, Alloc>();
          transaction.register_destructor_call_on_failure(ptr);
          return true;
        }

        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        template<typename... Params>
        static inline bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, std::basic_string<CharT, Traits, Alloc> *ptr, Params &&... p)
        {
          char *str = nullptr;
          cr::allocation_transaction temp_transaction;
          if (serializable<Backend, char *>::from_memory(temp_transaction, memory, size, &str, std::forward<Params>(p)...))
          {
            new(ptr) std::basic_string<CharT, Traits, Alloc>(str);
            temp_transaction.rollback();
            transaction.register_destructor_call_on_failure(ptr);
            return true;
          }
          temp_transaction.rollback();
          return false;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        template<typename... Params>
        static inline bool to_memory(memory_allocator &mem, size_t &size, const std::basic_string<CharT, Traits, Alloc> *ptr, Params &&... p)
        {
          const char *str = const_cast<const char *>(ptr->data());
          return serializable<Backend, char *>::to_memory(mem, size, &str, std::forward<Params>(p)...);
        }
    };

    template<typename Backend, typename Key, typename Value, typename Compare, typename Alloc>
    class persistence::serializable<Backend, std::map<Key, Value, Compare, Alloc>>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &transaction, std::map<Key, Value> *ptr)
        {
          new(ptr) std::map<Key, Value>();
          transaction.register_destructor_call_on_failure(ptr);
          return true;
        }

        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        template<typename... Params>
        static inline bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, std::map<Key, Value> *ptr, Params &&... p)
        {
          neam::array_wrapper<std::pair<Key, Value>> tmp(nullptr, 0);
          cr::allocation_transaction temp_transaction;
          if (!serializable<Backend, neam::array_wrapper<std::pair<Key, Value>>>::from_memory(temp_transaction, memory, size, &tmp, std::forward<Params>(p)...))
          {
            temp_transaction.rollback();
            return false;
          }

          new(ptr) std::map<Key, Value, Compare, Alloc>();
          transaction.register_destructor_call_on_failure(ptr);

          for (size_t i = 0; i < tmp.size; ++i)
            ptr->emplace_hint(ptr->end(), std::move(tmp.array[i].first), std::move(tmp.array[i].second));

          temp_transaction.rollback(); // free the memory allocated by the from_memory call

          return true;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        template<typename... Params>
        static inline bool to_memory(memory_allocator &mem, size_t &size, const std::map<Key, Value> *ptr, Params &&... p)
        {
          std::vector<std::pair<const Key, Value> *> tmp;
          tmp.reserve(ptr->size());

          for (auto & it : *ptr)
            tmp.emplace_back(const_cast<std::pair<const Key, Value> *>(&it));

          return serializable<Backend, std::vector<std::pair<const Key, Value> *>>::to_memory(mem, size, &tmp, std::forward<Params>(p)...);
        }
    };

    template<typename Backend, typename Key, typename Value, typename Hash, typename KeyEqual, typename Alloc>
    class persistence::serializable<Backend, std::unordered_map<Key, Value, Hash, KeyEqual, Alloc>>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &transaction, std::unordered_map<Key, Value> *ptr)
        {
          new(ptr) std::unordered_map<Key, Value>();
          transaction.register_destructor_call_on_failure(ptr);
          return true;
        }

        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        template<typename... Params>
        static inline bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, std::unordered_map<Key, Value> *ptr, Params &&... p)
        {
          neam::array_wrapper<std::pair<Key, Value>> tmp(nullptr, 0);
          cr::allocation_transaction temp_transaction;
          if (!serializable<Backend, neam::array_wrapper<std::pair<Key, Value>>>::from_memory(temp_transaction, memory, size, &tmp, std::forward<Params>(p)...))
          {
            temp_transaction.rollback();
            return false;
          }

          new(ptr) std::unordered_map<Key, Value, Hash, KeyEqual, Alloc>();
          transaction.register_destructor_call_on_failure(ptr);

          for (size_t i = 0; i < tmp.size; ++i)
            ptr->emplace(std::move(tmp.array[i].first), std::move(tmp.array[i].second));

          temp_transaction.rollback(); // free the memory allocated by the from_memory call

          return true;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        template<typename... Params>
        static inline bool to_memory(memory_allocator &mem, size_t &size, const std::unordered_map<Key, Value> *ptr, Params &&... p)
        {
          std::vector<std::pair<const Key, Value> *> tmp;
          tmp.reserve(ptr->size());

          for (auto & it : *ptr)
            tmp.emplace_back(const_cast<std::pair<const Key, Value> *>(&it));

          return serializable<Backend, std::vector<std::pair<const Key, Value> *>>::to_memory(mem, size, &tmp, std::forward<Params>(p)...);
        }
    };
  } // namespace cr
} // namespace neam

#endif /*__N_17535733811409347418_794184241__SERIALIZABLE_SPECS_GEN_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 

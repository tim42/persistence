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
        static inline bool default_initializer(cr::allocation_transaction &, Type *&)
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
          : public persistence_helper::list_serializable<Backend, std::vector<Type, Alloc>, persistence::serializable<Backend, std::vector<Type, Alloc>>>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &transaction, std::vector<Type, Alloc> *ptr)
        {
          new(ptr) std::vector<Type, Alloc>();
          transaction.register_destructor_call_on_failure(ptr);
          return true;
        }

        static inline bool from_memory_null(cr::allocation_transaction &transaction, std::vector<Type, Alloc> *ptr) // When the size is 0 or no data is given
        {
          return default_initializer(transaction, ptr);
        }

        static inline bool from_memory_allocate(cr::allocation_transaction &transaction, size_t size, std::vector<Type, Alloc> *ptr)
        {
          if (default_initializer(transaction, ptr))
          {
            ptr->reserve(size);
            return true;
          }
          return false;
        }

        template<typename... Params>
        static inline bool from_memory_single(cr::allocation_transaction &transaction, std::vector<Type, Alloc> *ptr, const char *sub_memory, size_t sub_size, size_t index, Params &&...p)
        {
          int8_t data[sizeof(Type)];
          Type *dptr = reinterpret_cast<Type *>(data);

          if (index != ptr->size())
            return false;

          if (persistence::serializable<Backend, Type>::from_memory(transaction, sub_memory, sub_size, dptr, std::forward<Params>(p)...))
          {
            ptr->emplace_back(std::move(*dptr));
            return true;
          }
          return false;
        }

        static inline bool from_memory_end(cr::allocation_transaction &, std::vector<Type, Alloc> *)
        {
          return true;
        }


        static inline size_t to_memory_get_iterator(const std::vector<Type, Alloc> *)
        {
          return 0;
        }

        static inline size_t to_memory_get_element_count(const std::vector<Type, Alloc> *ptr)
        {
          return ptr->size();
        }

        static inline bool to_memory_increment_iterator(size_t &it)
        {
          ++it;
          return true;
        }

        template<typename... Params>
        static inline bool to_memory_single(memory_allocator &mem, size_t &size, size_t &it, const std::vector<Type, Alloc> *ptr, Params && ... p)
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

    template<typename Backend, typename Type, size_t Size>
    class persistence::serializable<Backend, Type[Size]>
      : public persistence_helper::list_serializable<Backend, Type[Size], persistence::serializable<Backend, Type[Size]>>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &, Type (*array)[Size])
        {
          // TODO ?
          (void)array;
          return false;
        }

        static inline bool from_memory_null(cr::allocation_transaction &transaction, Type (*array)[Size]) // When the size is 0 or no data is given
        {
          return default_initializer(transaction, array);
        }

        static inline bool from_memory_allocate(cr::allocation_transaction &, size_t size, Type (*)[Size])
        {
          return size == Size;
        }

        template<typename... Params>
        static inline bool from_memory_single(cr::allocation_transaction &transaction, Type (*array)[Size], const char *sub_memory, size_t sub_size, size_t index, Params &&...p)
        {
          if (index >= Size)
            return false;

          return persistence::serializable<Backend, Type>::from_memory(transaction, sub_memory, sub_size, &((*array)[index]), std::forward<Params>(p)...);
        }

        static inline bool from_memory_end(cr::allocation_transaction &, Type (*)[Size])
        {
          return true;
        }


        static inline size_t to_memory_get_iterator(const Type (*)[Size])
        {
          return 0;
        }

        static inline size_t to_memory_get_element_count(const Type (*)[Size])
        {
          return Size;
        }

        static inline bool to_memory_increment_iterator(size_t &it)
        {
          ++it;
          return true;
        }

        template<typename... Params>
        static inline bool to_memory_single(memory_allocator &mem, size_t &size, size_t &it, const Type (*array)[Size], Params && ... p)
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

    template<typename Backend, typename Type>
    class persistence::serializable<Backend, neam::array_wrapper<Type>>
          : public persistence_helper::list_serializable<Backend, neam::array_wrapper<Type>, persistence::serializable<Backend, neam::array_wrapper<Type>>>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &transaction, neam::array_wrapper<Type> *ptr)
        {
          new(ptr) neam::array_wrapper<Type>(nullptr, 0);
          transaction.register_destructor_call_on_failure(ptr);
          return true;
        }

        static inline bool from_memory_null(cr::allocation_transaction &transaction, neam::array_wrapper<Type> *ptr) // When the size is 0 or no data is given
        {
          return default_initializer(transaction, ptr);
        }

        static inline bool from_memory_allocate(cr::allocation_transaction &transaction, size_t size, neam::array_wrapper<Type> *ptr)
        {
          Type *array = reinterpret_cast<Type *>(transaction.allocate_raw(sizeof(Type) * size));
          if (!array)
            return false;
          new(ptr) neam::array_wrapper<Type>(array, size);
          transaction.register_destructor_call_on_failure(ptr);
          return true;
        }

        template<typename... Params>
        static inline bool from_memory_single(cr::allocation_transaction &transaction, neam::array_wrapper<Type> *ptr, const char *sub_memory, size_t sub_size, size_t index, Params &&...p)
        {
          if (index >= ptr->size)
            return false;
          return persistence::serializable<Backend, Type>::from_memory(transaction, sub_memory, sub_size, &(ptr->array[index]), std::forward<Params>(p)...);
        }

        static inline bool from_memory_end(cr::allocation_transaction &, neam::array_wrapper<Type> *)
        {
          return true;
        }


        static inline size_t to_memory_get_iterator(const neam::array_wrapper<Type> *)
        {
          return 0;
        }

        static inline size_t to_memory_get_element_count(const neam::array_wrapper<Type> *ptr)
        {
          return ptr->size;
        }

        static inline bool to_memory_increment_iterator(size_t &it)
        {
          ++it;
          return true;
        }

        template<typename... Params>
        static inline bool to_memory_single(memory_allocator &mem, size_t &size, size_t &it, const neam::array_wrapper<Type> *ptr, Params && ... p)
        {
          if (it >= ptr->size)
            return false;
          return persistence::serializable<Backend, Type>::to_memory(mem, size, &(ptr->array[it]), std::forward<Params>(p)...);
        }

        static inline bool to_memory_end_iterator(size_t &)
        {
          return true;
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
          : public persistence_helper::collection_serializable<Backend, std::map<Key, Value, Compare, Alloc>, persistence::serializable<Backend, std::map<Key, Value, Compare, Alloc>>>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &transaction, std::map<Key, Value, Compare, Alloc> *ptr)
        {
          new(ptr) std::map<Key, Value, Compare, Alloc>();
          transaction.register_destructor_call_on_failure(ptr);
          return true;
        }

        static inline bool from_memory_null(cr::allocation_transaction &transaction, std::map<Key, Value, Compare, Alloc> *ptr) // When the size is 0 or no data is given
        {
          return default_initializer(transaction, ptr);
        }

        static inline bool from_memory_allocate(cr::allocation_transaction &transaction, size_t, std::map<Key, Value, Compare, Alloc> *ptr)
        {
          // can't preallocate memory on std::map
          return default_initializer(transaction, ptr);
        }

        template<typename... Params>
        static inline bool from_memory_single(cr::allocation_transaction &transaction, std::map<Key, Value, Compare, Alloc> *ptr, const char *sub_memory, size_t sub_size, size_t, Params &&...p)
        {
          int8_t data[sizeof(std::pair<Key, Value>)];
          std::pair<Key, Value> *dptr = reinterpret_cast<std::pair<Key, Value> *>(data);
          if (persistence::serializable<Backend, std::pair<Key, Value>>::from_memory(transaction, sub_memory, sub_size, dptr, std::forward<Params>(p)...))
          {
            // The hint will always be true if the data is serialized by persistence.
            // So we insert in amortized O(1)
            ptr->emplace_hint(ptr->end(), std::move(*dptr));
            return true;
          }
          return false;
        }

        using kv_instance_t = std::pair<Key, Value>;

        template<typename... Params>
        static inline bool from_memory_single_key(cr::allocation_transaction &transaction, std::map<Key, Value, Compare, Alloc> *, kv_instance_t *pair, const char *k_memory, size_t k_size, Params &&...k_p)
        {
          if (persistence::serializable<Backend, Key>::from_memory(transaction, k_memory, k_size, &(pair->first), std::forward<Params>(k_p)...))
            return true;
          return false;
        }

        template<typename... Params>
        static inline bool from_memory_single_value(cr::allocation_transaction &transaction, std::map<Key, Value, Compare, Alloc> *, kv_instance_t *pair, const char *v_memory, size_t v_size, Params &&...v_p)
        {
          if (persistence::serializable<Backend, Value>::from_memory(transaction, v_memory, v_size, &(pair->second), std::forward<Params>(v_p)...))
            return true;
          return false;
        }

        static inline bool from_memory_single_push_kv(cr::allocation_transaction &, std::map<Key, Value, Compare, Alloc> *ptr, kv_instance_t *pair)
        {
          ptr->emplace_hint(ptr->end(), std::move(*pair));
          return true;
        }

        static inline bool from_memory_end(cr::allocation_transaction &, std::map<Key, Value, Compare, Alloc> *)
        {
          return true;
        }


        static constexpr bool should_be_serialized_as_collection = neam::cr::persistence_helper::should_be_serialized_as_collection<Backend, Key>::value;

        static inline typename std::map<Key, Value, Compare, Alloc>::const_iterator to_memory_get_iterator(const std::map<Key, Value, Compare, Alloc> *ptr)
        {
          return ptr->begin();
        }

        static inline size_t to_memory_get_element_count(const std::map<Key, Value, Compare, Alloc> *ptr)
        {
          return ptr->size();
        }

        static inline bool to_memory_increment_iterator(typename std::map<Key, Value, Compare, Alloc>::const_iterator &it)
        {
          ++it;
          return true;
        }

        template<typename... Params>
        static inline bool to_memory_single(memory_allocator &mem, size_t &size, typename std::map<Key, Value, Compare, Alloc>::const_iterator &it, const std::map<Key, Value, Compare, Alloc> *ptr, Params && ... p)
        {
          if (it == ptr->end())
            return false;
          return persistence::serializable<Backend, std::pair<const Key, Value>>::to_memory(mem, size, &(*it), std::forward<Params>(p)...);
        }

        template<typename... Params>
        static inline bool to_memory_single_key(memory_allocator &mem, size_t &size, typename std::map<Key, Value, Compare, Alloc>::const_iterator &it, const std::map<Key, Value, Compare, Alloc> *ptr, Params && ... p)
        {
          if (it == ptr->end())
            return false;
          return persistence::serializable<Backend, Key>::to_memory(mem, size, &(it->first), std::forward<Params>(p)...);
        }

        template<typename... Params>
        static inline bool to_memory_single_value(memory_allocator &mem, size_t &size, typename std::map<Key, Value, Compare, Alloc>::const_iterator &it, const std::map<Key, Value, Compare, Alloc> *ptr, Params && ... p)
        {
          if (it == ptr->end())
            return false;
          return persistence::serializable<Backend, Value>::to_memory(mem, size, &(it->second), std::forward<Params>(p)...);
        }

        static inline bool to_memory_end_iterator(typename std::map<Key, Value, Compare, Alloc>::const_iterator &)
        {
          return true;
        }
    };

    template<typename Backend, typename Key, typename Value, typename Hash, typename KeyEqual, typename Alloc>
    class persistence::serializable<Backend, std::unordered_map<Key, Value, Hash, KeyEqual, Alloc>>
          : public persistence_helper::collection_serializable<Backend, std::unordered_map<Key, Value, Hash, KeyEqual, Alloc>, persistence::serializable<Backend, std::unordered_map<Key, Value, Hash, KeyEqual, Alloc>>>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &transaction, std::unordered_map<Key, Value, Hash, KeyEqual, Alloc> *ptr)
        {
          new(ptr) std::unordered_map<Key, Value, Hash, KeyEqual, Alloc>();
          transaction.register_destructor_call_on_failure(ptr);
          return true;
        }

        static inline bool from_memory_null(cr::allocation_transaction &transaction, std::unordered_map<Key, Value, Hash, KeyEqual, Alloc> *ptr) // When the size is 0 or no data is given
        {
          return default_initializer(transaction, ptr);
        }

        static inline bool from_memory_allocate(cr::allocation_transaction &transaction, size_t, std::unordered_map<Key, Value, Hash, KeyEqual, Alloc> *ptr)
        {
          // can't preallocate memory on std::unordered_map
          return default_initializer(transaction, ptr);
        }

        template<typename... Params>
        static inline bool from_memory_single(cr::allocation_transaction &transaction, std::unordered_map<Key, Value, Hash, KeyEqual, Alloc> *ptr, const char *sub_memory, size_t sub_size, size_t, Params &&...p)
        {
          int8_t data[sizeof(std::pair<Key, Value>)];
          std::pair<Key, Value> *dptr = reinterpret_cast<std::pair<Key, Value> *>(data);

          if (persistence::serializable<Backend, std::pair<Key, Value>>::from_memory(transaction, sub_memory, sub_size, dptr, std::forward<Params>(p)...))
          {
            ptr->emplace(std::move(dptr->first), std::move(dptr->second));
            return true;
          }
          return false;
        }

        using kv_instance_t = std::pair<Key, Value>;

        template<typename... Params>
        static inline bool from_memory_single_key(cr::allocation_transaction &transaction, std::unordered_map<Key, Value, Hash, KeyEqual, Alloc> *, kv_instance_t *pair, const char *k_memory, size_t k_size, Params &&...k_p)
        {
          if (persistence::serializable<Backend, Key>::from_memory(transaction, k_memory, k_size, &(pair->first), std::forward<Params>(k_p)...))
            return true;
          return false;
        }

        template<typename... Params>
        static inline bool from_memory_single_value(cr::allocation_transaction &transaction, std::unordered_map<Key, Value, Hash, KeyEqual, Alloc> *, kv_instance_t *pair, const char *v_memory, size_t v_size, Params &&...v_p)
        {
          if (persistence::serializable<Backend, Key>::from_memory(transaction, v_memory, v_size, &(pair->second), std::forward<Params>(v_p)...))
            return true;
          return false;
        }

        static inline bool from_memory_single_push_kv(cr::allocation_transaction &, std::unordered_map<Key, Value, Hash, KeyEqual, Alloc> *ptr, kv_instance_t *pair)
        {
          ptr->emplace(std::move(*pair));
          return true;
        }

        static inline bool from_memory_end(cr::allocation_transaction &, std::unordered_map<Key, Value, Hash, KeyEqual, Alloc> *)
        {
          return true;
        }


        static constexpr bool should_be_serialized_as_collection = neam::cr::persistence_helper::should_be_serialized_as_collection<Backend, Key>::value;

        static inline typename std::unordered_map<Key, Value, Hash, KeyEqual, Alloc>::const_iterator to_memory_get_iterator(const std::unordered_map<Key, Value, Hash, KeyEqual, Alloc> *ptr)
        {
          return ptr->begin();
        }

        static inline size_t to_memory_get_element_count(const std::unordered_map<Key, Value, Hash, KeyEqual, Alloc> *ptr)
        {
          return ptr->size();
        }

        static inline bool to_memory_increment_iterator(typename std::unordered_map<Key, Value, Hash, KeyEqual, Alloc>::const_iterator &it)
        {
          ++it;
          return true;
        }

        template<typename... Params>
        static inline bool to_memory_single(memory_allocator &mem, size_t &size, typename std::unordered_map<Key, Value, Hash, KeyEqual, Alloc>::const_iterator &it, const std::unordered_map<Key, Value, Hash, KeyEqual, Alloc> *ptr, Params && ... p)
        {
          if (it == ptr->end())
            return false;
          return persistence::serializable<Backend, std::pair<const Key, Value>>::to_memory(mem, size, &(*it), std::forward<Params>(p)...);
        }

        template<typename... Params>
        static inline bool to_memory_single_key(memory_allocator &mem, size_t &size, typename std::unordered_map<Key, Value, Hash, KeyEqual, Alloc>::const_iterator &it, const std::unordered_map<Key, Value, Hash, KeyEqual, Alloc> *ptr, Params && ... p)
        {
          if (it == ptr->end())
            return false;
          return persistence::serializable<Backend, const Key>::to_memory(mem, size, &(it->first), std::forward<Params>(p)...);
        }

        template<typename... Params>
        static inline bool to_memory_single_value(memory_allocator &mem, size_t &size, typename std::unordered_map<Key, Value, Hash, KeyEqual, Alloc>::const_iterator &it, const std::unordered_map<Key, Value, Hash, KeyEqual, Alloc> *ptr, Params && ... p)
        {
          if (it == ptr->end())
            return false;
          return persistence::serializable<Backend, Value>::to_memory(mem, size, &(it->second), std::forward<Params>(p)...);
        }

        static inline bool to_memory_end_iterator(typename std::unordered_map<Key, Value, Hash, KeyEqual, Alloc>::const_iterator &)
        {
          return true;
        }
    };


    /// \brief Handle the std::pair< X, Y > case
    /// \note This is how you handle classes with templates parameters
    NCRP_DECLARE_NAME(_std__pair, first);
    NCRP_DECLARE_NAME(_std__pair, second);
    template<typename Backend, typename First, typename Second>
    class persistence::serializable<Backend, std::pair<First, Second>> : public persistence::serializable_object
    <
      Backend,

      std::pair<First, Second>,

      NCRP_NAMED_TYPED_OFFSET(NCRP_TEMPLATE_CLASS(std::pair<First, Second>), first, names::_std__pair::first),
      NCRP_NAMED_TYPED_OFFSET(NCRP_TEMPLATE_CLASS(std::pair<First, Second>), second, names::_std__pair::second)
    > {};
  } // namespace cr
} // namespace neam

#endif /*__N_17535733811409347418_794184241__SERIALIZABLE_SPECS_GEN_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 

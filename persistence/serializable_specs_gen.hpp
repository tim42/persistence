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
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static bool from_memory(const char *memory, size_t size, const Type *ptr)
        {
          return serializable<Backend, Type>::from_memory(memory, size, const_cast<Type *>(ptr));
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, const Type *ptr)
        {
          return serializable<Backend, Type>::to_memory(mem, size, (ptr));
        }
    };

    template<typename Backend, typename Type>
    class persistence::serializable<Backend, Type *>
    {
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static bool from_memory(const char *memory, size_t size, Type **ptr)
        {
          // handle the null pointer case
          if (!size)
          {
            *ptr = nullptr;
            return true;
          }

          Type *tptr = reinterpret_cast<Type *>(operator new(sizeof(Type), std::nothrow));
          if (!tptr)
            return false;
          *ptr = tptr;
          return serializable<Backend, Type>::from_memory(memory, size, tptr);
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, const Type* const* ptr)
        {
          // handle the null pointer case
          if (!*ptr)
          {
            size = 0;
            return true;
          }
          return serializable<Backend, Type>::to_memory(mem, size, *ptr);
        }
    };

    template<typename Backend, typename Type>
    class persistence::serializable<Backend, std::vector<Type>>
    {
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static bool from_memory(const char *memory, size_t size, std::vector<Type> *ptr)
        {
          array_wrapper<Type> o(nullptr, 0);
          if (serializable<Backend, neam::array_wrapper<Type>>::from_memory(memory, size, &o))
          {
            new(ptr) std::vector<Type>();
            ptr->reserve(o.size + 1);
            ptr->insert(ptr->begin(), o.array, o.array + o.size);
            delete o.array;
            return true;
          }
          return false;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, const std::vector<Type> *ptr)
        {
          array_wrapper<Type> o(const_cast<Type *>(ptr->data()), ptr->size());
          return serializable<Backend, neam::array_wrapper<Type>>::to_memory(mem, size, &o);
        }
    };

    template<typename Backend>
    class persistence::serializable<Backend, std::string>
    {
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static bool from_memory(const char *memory, size_t size, std::string *ptr)
        {
          char *str = nullptr;
          if (serializable<Backend, char *>::from_memory(memory, size, &str))
          {
            new(ptr) std::string(str);
            delete str;
            return true;
          }
          return false;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, const std::string *ptr)
        {
          const char *str = const_cast<const char *>(ptr->data());
          return serializable<Backend, char *>::to_memory(mem, size, &str);
        }
    };

    template<typename Backend, typename Key, typename Value>
    class persistence::serializable<Backend, std::map<Key, Value>>
    {
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static bool from_memory(const char *memory, size_t size, std::map<Key, Value> *ptr)
        {
          neam::array_wrapper<std::pair<Key, Value>> tmp(nullptr, 0);
          if (!serializable<Backend, neam::array_wrapper<std::pair<Key, Value>>>::from_memory(memory, size, &tmp))
          {
            return false;
          }

          new(ptr) std::map<Key, Value>();

          for (size_t i = 0; i < tmp.size; ++i)
          {
            ptr->emplace_hint(ptr->end(), std::move(tmp.array[i].first), std::move(tmp.array[i].second));
            tmp.array[i].~pair();
           }

          delete reinterpret_cast<char *>(tmp.array);

          return true;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, const std::map<Key, Value> *ptr)
        {
          std::vector<std::pair<const Key, Value> *> tmp;
          tmp.reserve(ptr->size());

          for (auto & it : *ptr)
            tmp.emplace_back(const_cast<std::pair<const Key, Value> *>(&it));

          return serializable<Backend, std::vector<std::pair<const Key, Value> *>>::to_memory(mem, size, &tmp);
        }
    };

    template<typename Backend, typename Key, typename Value>
    class persistence::serializable<Backend, std::unordered_map<Key, Value>>
    {
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static bool from_memory(const char *memory, size_t size, std::unordered_map<Key, Value> *ptr)
        {
          neam::array_wrapper<std::pair<Key, Value>> tmp(nullptr, 0);
          if (!serializable<Backend, neam::array_wrapper<std::pair<Key, Value>>>::from_memory(memory, size, &tmp))
          {
            return false;
          }

          new(ptr) std::unordered_map<Key, Value>();

          for (size_t i = 0; i < tmp.size; ++i)
            ptr->emplace(std::move(tmp.array[i].first), std::move(tmp.array[i].second));

          delete tmp.array;

          return true;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, const std::unordered_map<Key, Value> *ptr)
        {
          std::vector<std::pair<const Key, Value> *> tmp;
          tmp.reserve(ptr->size());

          for (auto & it : *ptr)
            tmp.emplace_back(const_cast<std::pair<const Key, Value> *>(&it));

          return serializable<Backend, std::vector<std::pair<const Key, Value> *>>::to_memory(mem, size, &tmp);
        }
    };
  } // namespace cr
} // namespace neam

#endif /*__N_17535733811409347418_794184241__SERIALIZABLE_SPECS_GEN_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 

/*
          // toto
          ...
    .     serializable_object<...>::from_memory_single()    // for the std::map<..., titi>
    |     serializable<neam::cr::persistence_backend::neam, std::map<std::string, titi, ...>>
    |     serializable<neam::cr::persistence_backend::neam, neam::array_wrapper<std::pair<std::string, titi> >>
    v     serializable<neam::cr::persistence_backend::neam, std::pair<std::string, titi>>

          // titi
          serializable_object<...>::from_memory
          serializable_object<...>::from_memory_single      // for the vector
          serializable<neam::cr::persistence_backend::neam, std::vector<unsigned long>

          // tha vector
          std::vector<unsigned long, std::allocator<unsigned long> >::reserve()
          ....


==804==    at 0x4C28EA0: operator new(unsigned long) (in /usr/lib64/valgrind/vgpreload_memcheck-amd64-linux.so)
==804==    by 0x41847D: __gnu_cxx::new_allocator<unsigned long>::allocate(unsigned long, void const*) (new_allocator.h:104)
==804==    by 0x417728: std::_Vector_base<unsigned long, std::allocator<unsigned long> >::_M_allocate(unsigned long) (in /home/tim/projects/persistence/build/benchmark-persistence-0.0.1)
==804==    by 0x418D17: unsigned long* std::vector<unsigned long, std::allocator<unsigned long> >::_M_allocate_and_copy<std::move_iterator<unsigned long*> >(unsigned long, std::move_iterator<unsigned long*>, std::move_iterator<unsigned long*>) (stl_vector.h:1138)
==804==    by 0x4180A8: std::vector<unsigned long, std::allocator<unsigned long> >::reserve(unsigned long) (vector.tcc:75)
==804==    by 0x417384: neam::cr::persistence::serializable<neam::cr::persistence_backend::neam, std::vector<unsigned long, std::allocator<unsigned long> >>::from_memory(char const*, unsigned long, std::vector<unsigned long, std::allocator<unsigned long> >*) (serializable_specs_gen.hpp:125)
==804==    by 0x416248: bool neam::cr::persistence::serializable_object<neam::cr::persistence_backend::neam, neam::cr::persistence::typed_offset<int, 0ul>, neam::cr::persistence::typed_offset<double, 8ul>, neam::cr::persistence::typed_offset<char, 40ul>, neam::cr::persistence::typed_offset<std::vector<unsigned long, std::allocator<unsigned long> >, 16ul> >::from_memory_single<neam::cr::persistence::typed_offset<std::vector<unsigned long, std::allocator<unsigned long> >, 16ul> >(char const*, unsigned long, void*, unsigned long&) (object.hpp:207)
==804==    by 0x415269: neam::cr::persistence::serializable_object<neam::cr::persistence_backend::neam, neam::cr::persistence::typed_offset<int, 0ul>, neam::cr::persistence::typed_offset<double, 8ul>, neam::cr::persistence::typed_offset<char, 40ul>, neam::cr::persistence::typed_offset<std::vector<unsigned long, std::allocator<unsigned long> >, 16ul> >::from_memory(char const*, unsigned long, void*) (object.hpp:165)
==804==    by 0x418EA6: neam::cr::persistence::serializable<neam::cr::persistence_backend::neam, std::pair<std::string, titi>>::from_memory(char const*, unsigned long, std::pair<std::string, titi>*) (serializable_specs_neam.hpp:190)
==804==    by 0x4182B3: neam::cr::persistence::serializable<neam::cr::persistence_backend::neam, neam::array_wrapper<std::pair<std::string, titi> >>::from_memory(char const*, unsigned long, neam::array_wrapper<std::pair<std::string, titi> >*) (serializable_specs_neam.hpp:263)
==804==    by 0x41747D: neam::cr::persistence::serializable<neam::cr::persistence_backend::neam, std::map<std::string, titi, std::less<std::string>, std::allocator<std::pair<std::string const, titi> > >>::from_memory(char const*, unsigned long, std::map<std::string, titi, std::less<std::string>, std::allocator<std::pair<std::string const, titi> > >*) (serializable_specs_gen.hpp:190)
==804==    by 0x416516: bool neam::cr::persistence::serializable_object<neam::cr::persistence_backend::neam, neam::cr::persistence::typed_offset<titi, 0ul>, neam::cr::persistence::typed_offset<int, 48ul>, neam::cr::persistence::typed_offset<double, 56ul>, neam::cr::persistence::typed_offset<std::map<std::string, titi, std::less<std::string>, std::allocator<std::pair<std::string const, titi> > >, 64ul>, neam::cr::persistence::typed_offset<titi, 112ul>, neam::cr::persistence::typed_offset<char, 160ul>, neam::cr::persistence::typed_offset<float, 164ul>, neam::cr::persistence::typed_offset<titi, 168ul> >::from_memory_single<neam::cr::persistence::typed_offset<std::map<std::string, titi, std::less<std::string>, std::allocator<std::pair<std::string const, titi> > >, 64ul> >(char const*, unsigned long, void*, unsigned long&) (object.hpp:207)
==804== 
==804== 807,309,848 bytes in 67,231 blocks are definitely lost in loss record 3 of 3
==804==    at 0x4C28EA0: operator new(unsigned long) (in /usr/lib64/valgrind/vgpreload_memcheck-amd64-linux.so)
==804==    by 0x41847D: __gnu_cxx::new_allocator<unsigned long>::allocate(unsigned long, void const*) (new_allocator.h:104)
==804==    by 0x417728: std::_Vector_base<unsigned long, std::allocator<unsigned long> >::_M_allocate(unsigned long) (in /home/tim/projects/persistence/build/benchmark-persistence-0.0.1)
==804==    by 0x418D17: unsigned long* std::vector<unsigned long, std::allocator<unsigned long> >::_M_allocate_and_copy<std::move_iterator<unsigned long*> >(unsigned long, std::move_iterator<unsigned long*>, std::move_iterator<unsigned long*>) (stl_vector.h:1138)
==804==    by 0x4180A8: std::vector<unsigned long, std::allocator<unsigned long> >::reserve(unsigned long) (vector.tcc:75)
==804==    by 0x417384: neam::cr::persistence::serializable<neam::cr::persistence_backend::neam, std::vector<unsigned long, std::allocator<unsigned long> >>::from_memory(char const*, unsigned long, std::vector<unsigned long, std::allocator<unsigned long> >*) (serializable_specs_gen.hpp:125)
==804==    by 0x416248: bool neam::cr::persistence::serializable_object<neam::cr::persistence_backend::neam, neam::cr::persistence::typed_offset<int, 0ul>, neam::cr::persistence::typed_offset<double, 8ul>, neam::cr::persistence::typed_offset<char, 40ul>, neam::cr::persistence::typed_offset<std::vector<unsigned long, std::allocator<unsigned long> >, 16ul> >::from_memory_single<neam::cr::persistence::typed_offset<std::vector<unsigned long, std::allocator<unsigned long> >, 16ul> >(char const*, unsigned long, void*, unsigned long&) (object.hpp:207)
==804==    by 0x415269: neam::cr::persistence::serializable_object<neam::cr::persistence_backend::neam, neam::cr::persistence::typed_offset<int, 0ul>, neam::cr::persistence::typed_offset<double, 8ul>, neam::cr::persistence::typed_offset<char, 40ul>, neam::cr::persistence::typed_offset<std::vector<unsigned long, std::allocator<unsigned long> >, 16ul> >::from_memory(char const*, unsigned long, void*) (object.hpp:165)
==804==    by 0x418EA6: neam::cr::persistence::serializable<neam::cr::persistence_backend::neam, std::pair<std::string, titi>>::from_memory(char const*, unsigned long, std::pair<std::string, titi>*) (serializable_specs_neam.hpp:190)
==804==    by 0x4182B3: neam::cr::persistence::serializable<neam::cr::persistence_backend::neam, neam::array_wrapper<std::pair<std::string, titi> >>::from_memory(char const*, unsigned long, neam::array_wrapper<std::pair<std::string, titi> >*) (serializable_specs_neam.hpp:263)
==804==    by 0x41747D: neam::cr::persistence::serializable<neam::cr::persistence_backend::neam, std::map<std::string, titi, std::less<std::string>, std::allocator<std::pair<std::string const, titi> > >>::from_memory(char const*, unsigned long, std::map<std::string, titi, std::less<std::string>, std::allocator<std::pair<std::string const, titi> > >*) (serializable_specs_gen.hpp:190)
==804==    by 0x416516: bool neam::cr::persistence::serializable_object<neam::cr::persistence_backend::neam, neam::cr::persistence::typed_offset<titi, 0ul>, neam::cr::persistence::typed_offset<int, 48ul>, neam::cr::persistence::typed_offset<double, 56ul>, neam::cr::persistence::typed_offset<std::map<std::string, titi, std::less<std::string>, std::allocator<std::pair<std::string const, titi> > >, 64ul>, neam::cr::persistence::typed_offset<titi, 112ul>, neam::cr::persistence::typed_offset<char, 160ul>, neam::cr::persistence::typed_offset<float, 164ul>, neam::cr::persistence::typed_offset<titi, 168ul> >::from_memory_single<neam::cr::persistence::typed_offset<std::map<std::string, titi, std::less<std::string>, std::allocator<std::pair<std::string const, titi> > >, 64ul> >(char const*, unsigned long, void*, unsigned long&) (object.hpp:207)
*/
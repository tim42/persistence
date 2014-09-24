//
// file : object.hpp
// in : file:///home/tim/projects/reflective/reflective/persistence/object.hpp
//
// created by : Timothée Feuillet on linux.site
// date: 20/09/2014 19:37:15
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

#ifndef __N_2006814652382068822_103083989__OBJECT_HPP__
# define __N_2006814652382068822_103083989__OBJECT_HPP__

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>
#include <vector>
#include <tools/execute_pack.hpp>
#include <tools/embed.hpp>              // embed data as types (for use in templates)
#include <tools/constructor_call.hpp>   // embed constructor in templates :)
#include <tools/memory_allocator.hpp>

#include "raw_data.hpp"

#include <iostream>
#include <tools/demangle.hpp>

/// \file object.hpp
/// \brief this file provide generic way to serialize / deserialize C++ objects (\e recursively) with a minimum of runtime impact

namespace neam
{
  namespace cr
  {

    /// \brief a way to now if your constructor is called from the serialization process
    struct from_serialization_t {constexpr from_serialization_t() {}};

    /// \brief this type could be used directly in a N_CALL_CONSTRUCTOR or a subsequent call o N_EMBED_OBJECT
    using from_serialization = N_EMBED_OBJECT(from_serialization_t);

    /// \brief serialize data
    /// originally the class persistence was a namespace, but this caused some problems with private members
    /// (as in the private member would appear as a template parameter of \e serializable_object and \e constructible_serializable_object).
    /// Making the \e persistence namespace a class allowed one to use \code friend class neam::cr::persistence; \endcode to allow access of private members.
    struct persistence
    {

        /// \brief a simple function that goes through all the serialization process and returns a \e raw_data struct that holds (and have the ownership) of the serialized object
        /// \return an empty \e raw_data instance (data = nullptr and size = 0) when the process has failed
        template<typename Type>
        static raw_data serialize(const Type &obj)
        {
          raw_data rdt;

          neam::cr::memory_allocator mem;
          size_t size = 0;

          if (!serializable<Type>::to_memory(mem, size, &obj))
            return rdt;

          return std::move(rdt.set(size, reinterpret_cast<int8_t *>(mem.give_up_data()), neam::assume_ownership));
        }

        /// \brief deserialize a class
        /// \return nullptr when it has failed
        /// \note It's up to you to \b delete the returned object !!!
        template<typename Type>
        static Type *deserialize(const raw_data &serialized_data)
        {
          Type *ptr = reinterpret_cast<Type *>(operator new(sizeof(Type)));

          if (!serializable<Type>::from_memory(reinterpret_cast<const char *>(serialized_data.data), serialized_data.size, ptr))
          {
            delete ptr;
            return nullptr;
          }

          return ptr;
        }

        /// \brief the is the contract for a serializable object.
        /// you \e simply have to specialize it with the type requested and serialization for this type will work.
        /// \note you should only specialize this class for basic types. (floating points, integers, strings, arrays, ...)
        /// \see serializable_object
        /// \see constructable_serializable_object
        /// \note this version if for dumping the memory of the object directly
        /// \note this is a recursive solution: \b avoid \b \e reference \b cycles in your constructs !!! (else: \e stack-overflow).
        template<typename Type, typename... Args>
        class serializable
        {
          public:
            /// \brief deserialize the object
            /// \param[in] memory the serialized object
            /// \param[in] size the size of the memory area
            /// \param[out] ptr a pointer to the object (the one that the function will fill)
            /// \return true if successful
            static bool from_memory(const char *memory, size_t size, Type *ptr)
            {
              if (size != sizeof(Type))
                return false;
              *ptr = *reinterpret_cast<const Type *>(memory);
              return true;
            }

            /// \brief serialize the object
            /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
            /// \param[out] size the size of the memory area
            /// \param[in] ptr a pointer to the object (the one that the function will serialize)
            /// \return true if successful
            static bool to_memory(memory_allocator &mem, size_t &size, const Type *ptr)
            {
              size = sizeof(Type);
              char *memory = reinterpret_cast<char *>(mem.allocate(size));
              if (!memory)
                return false;
              *reinterpret_cast<Type *>(memory) = *ptr;
              return true;
            }
        };

        /// \brief this serialize complex objects (like classes)
        /// this generate meta-data used to generate code that will fill the object.
        /// \param OffsetTypeList is a list of \code typed_offset <type, offsetof(my_class, member)> \endcode that could be simplified with the macro \code NRP_TYPPED_OFFSET(my_class, member) \endcode
        /// \note you still have to create a \code serializable \endcode specialization for the object, but you could simply use this trick: \code template<>class serializable<my_class> : public serializable_object<...> {}; \endcode
        /// \see constructible_serializable_object
        template<typename... OffsetTypeList>
        class serializable_object
        {
          public:
            /// \brief deserialize the object
            /// \param[in] memory the serialized object
            /// \param[in] size the size of the memory area
            /// \param[out] ptr a pointer to the object (the one that the function will fill)
            /// \return true if successful
            static bool from_memory(const char *memory, size_t size, void *ptr)
            {
              size_t mem_offset = 0;
              bool res = true;
              NEAM_EXECUTE_PACK((res &= from_memory_single<OffsetTypeList>(memory, size, ptr, mem_offset))); // the magic
              return res;
            }

            /// \brief serialize the object
            /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
            /// \param[out] size the size of the memory area
            /// \param[in] ptr a pointer to the object (the one that the function will serialize)
            /// \return true if successful
            static bool to_memory(memory_allocator &mem, size_t &size, const void *ptr)
            {
              uint32_t whole_object_size = 0;
              bool res = true;

              size_t sz = mem.size();

              NEAM_EXECUTE_PACK((res &= to_memory_single<OffsetTypeList>(ptr, whole_object_size, mem)));

              // deallocate unused memory
              if (!res)
              {
                mem.pop(mem.size() - sz);
                return false;
              }
              size = whole_object_size;

              return true;
            }

          private:
            template<typename OffsetType>
            static inline bool from_memory_single(const char *memory, size_t size, void *ptr, size_t &offset)
            {
              if (offset + sizeof(uint32_t) > size)
                return false;

              uint32_t element_size = *reinterpret_cast<const uint32_t *>(memory + offset);

              offset += sizeof(uint32_t);
              if (offset + element_size > size)
                return false;

              bool res = serializable<typename OffsetType::type>::from_memory(memory + offset, element_size, reinterpret_cast<typename OffsetType::type *>(reinterpret_cast<uint8_t *>(ptr) + OffsetType::offset));

              offset += element_size;
              return res;
            }

            template<typename OffsetType>
            static inline bool to_memory_single(const void *ptr, uint32_t &global_size, memory_allocator &mem)
            {
              size_t element_size = 0;

              uint32_t *size_memory = reinterpret_cast<uint32_t *>(mem.allocate(sizeof(uint32_t)));
              if (!size_memory)
                return false;

              if (!serializable<typename OffsetType::type>::to_memory(mem, element_size, reinterpret_cast<const typename OffsetType::type *>(reinterpret_cast<const uint8_t *>(ptr) + OffsetType::offset)))
              {
                mem.pop(sizeof(uint32_t));
                return false;
              }

              *size_memory = element_size;
              global_size += element_size + sizeof(uint32_t);
              return true;
            }
        };


        /// \brief construct an object and call its constructor
        /// behave exactly like the class serializable_object.
        /// For an example, see the README.md at the root of the project.
        /// \see class serializable_object
        /// \see neam::ct::constructor
        /// \see N_CALL_CONSTRUCTOR (in tools/constructor_call.hpp)
        template<typename ConstructorCall, typename... OffsetTypeList>
        class constructible_serializable_object
        {
          public:
            /// \brief deserialize the object
            /// \param[in] memory the serialized object
            /// \param[in] size the size of the memory area
            /// \param[out] ptr a pointer to the object (the one that the function will fill)
            /// \return true if successful
            static bool from_memory(const char *memory, size_t size, void *ptr)
            {
              if (!serializable_object<OffsetTypeList...>::from_memory(memory, size, ptr))
                return false;

              // call the constructor of the newly-created object (magic ?)
              ConstructorCall::on(reinterpret_cast<typename ConstructorCall::type *>(ptr));

              return true;
            }

            /// \brief serialize the object
            /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
            /// \param[out] size the size of the memory area
            /// \param[in] ptr a pointer to the object (the one that the function will serialize)
            /// \return true if successful
            static bool to_memory(memory_allocator &mem, size_t &size, const void *ptr)
            {
              // simply forward to serializable_object as we don't have to do anything here.
              return serializable_object<OffsetTypeList...>::to_memory(mem, size, ptr);
            }
        };



        /// \see serializable_object
        template<typename Type, size_t Offset>
        struct typed_offset
        {
          using type = Type;
          constexpr static size_t offset = Offset;
        };

/// \brief this is quite "crade", isn't it ?? (this is the version with a pointer to member of the so famous C macro)
#define N__OFFSETOF(class, member)                              reinterpret_cast<size_t>(&(reinterpret_cast<class *>(0)->*(&class::member)))

/// \brief compute the type and the offset of the \e member of \e class
/// \see class serializable_object
/// \see class constructible_serializable_object
#define NCRP_TYPED_OFFSET(class, member)                        neam::cr::persistence::typed_offset<decltype(class::member), N__OFFSETOF(class, member)>
/// \brief same as \b NCRP_TYPED_OFFSET, but the user also specify the type
#define NCRP_OFFSET(type, class, member)                        neam::cr::persistence::typed_offset<type, N__OFFSETOF(class, member)>

// G++ complain... :/
// #define NCRP_TYPED_OFFSET(class, member)                 neam::cr::persistence::typed_offset<decltype(class::member), offsetof(class, member)>
// #define NCRP_OFFSET(type, class, member)                 neam::cr::persistence::typed_offset<type, offsetof(class, member)>

/// \brief like NCRP_TYPED_OFFSET, but surround the computed type with a wrapper (like \e checksum or \e magic)
#define NCRP_WRAPPED_TYPED_OFFSET(class, member, wrapper, ...)  neam::cr::persistence::typed_offset<wrapper<decltype(class::member), ##__VAARGS__>, N__OFFSETOF(class, member)>

        // no one could instanciate this class.
        // NO ONE.
        persistence() = delete;
        ~persistence() = delete;
    }; // class persistence
  } // namespace cr
} // namespace neam

#include "serializable_specs.hpp"
#include "serializable_wrappers.hpp"

#endif /*__N_2006814652382068822_103083989__OBJECT_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on;


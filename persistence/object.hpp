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
#include <iostream>

#include "tools/execute_pack.hpp"
#include "tools/embed.hpp"              // embed data as types (for use in templates)
#include "tools/constructor_call.hpp"   // embed constructor in templates :)
#include "tools/memory_allocator.hpp"
#include "tools/allocation_transaction.hpp"
#include "tools/ct_string.hpp"

#include "tools/demangle.hpp"

#include "raw_data.hpp"
#include "list_serializable.hpp"
#include "collection_serializable.hpp"

/// \file object.hpp
/// \brief this file provide generic way to serialize / deserialize C++ objects (\e recursively) with a minimum of runtime impact

namespace neam
{
  namespace cr
  {

    /// \brief the list of defaultly available backend
    namespace persistence_backend
    {
      struct neam {}; // the "default" and faster backend.
      struct verbose {}; // a verbose backend (serialization only)
      struct json {};    // a JSON backend (serialization only, TODO: deserialization)
    } // namespace persitence_backend

    /// \brief serialize data
    /// originally the class persistence was a namespace, but this caused some problems with private members
    /// (as in the private member would appear as a template parameter of \e serializable_object and \e constructible_serializable_object).
    /// Making the \e persistence namespace a class allowed one to use \code friend class neam::cr::persistence; \endcode to allow access of private members.
    struct persistence
    {
        /// \brief a simple function that goes through all the serialization process and returns a \e raw_data struct that holds (and have the ownership) of the serialized object
        /// \return an empty \e raw_data instance (data = nullptr and size = 0) when the process has failed
        template<typename Backend, typename Type, typename... Params>
        static raw_data serialize(const Type &obj, Params... p)
        {
          raw_data rdt;

          neam::cr::memory_allocator mem;
          size_t size = 0;

          if (!serializable<Backend, Type>::to_memory(mem, size, &obj, std::forward<Params>(p)...) || mem.has_failed())
          {
            return rdt;
          }

          *reinterpret_cast<char *>(mem.allocate(1)) = 0;
          size = mem.size();

          return std::move(rdt.set(size, reinterpret_cast<int8_t *>(mem.give_up_data()), neam::assume_ownership));
        }

        /// \brief deserialize a class
        /// \return nullptr when it has failed
        /// \note It's up to you to \b delete the returned object !!!
        template<typename Backend, typename Type, typename... Params>
        static Type *deserialize(const raw_data &serialized_data, Params... p)
        {
          cr::allocation_transaction transaction;

          Type *ptr = reinterpret_cast<Type *>(transaction.allocate_raw(sizeof(Type)));

          size_t size = serialized_data.size; // here because of the JSON serializer: it needs a non-const reference !.
          if (!serializable<Backend, Type>::from_memory(transaction, reinterpret_cast<const char *>(serialized_data.data), size, ptr, std::forward<Params>(p)...))
          {
            transaction.rollback();
            return nullptr;
          }

          transaction.complete();

          return ptr;
        }

        /// \brief deserialize a class, uses an already existing object
        /// \return nullptr when it has failed, else return the pointer in \e ptr
        template<typename Backend, typename Type, typename... Params>
        static Type *deserialize(const raw_data &serialized_data, Type *ptr, Params... p)
        {
          cr::allocation_transaction transaction;

          size_t size = serialized_data.size; // here because of the JSON serializer: it needs a non-const reference !.
          if (!serializable<Backend, Type>::from_memory(transaction, reinterpret_cast<const char *>(serialized_data.data), size, ptr, std::forward<Params>(p)...))
          {
            transaction.rollback();
            return nullptr;
          }
          transaction.complete();

          return ptr;
        }


        /// \brief the is the contract for a serializable object.
        /// you \e simply have to specialize it with the type requested and serialization for this type will work.
        /// \note you should only specialize this class for basic types. (floating points, integers, strings, arrays, ...)
        /// \see serializable_object
        /// \see constructable_serializable_object
        /// \note this version if for dumping the memory of the object directly
        /// \note this is a recursive solution: \b avoid \b \e reference \b cycles in your constructs !!! (else: \e stack-overflow).
        template<typename Backend, typename Type, typename Fuck = void, typename... Args>
        class serializable
        {
          public:
            /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
            static inline bool default_initializer(cr::allocation_transaction &, Type *)
            {
              return false;
            }

            /// \brief deserialize the object
            /// \param[in] memory the serialized object
            /// \param[in] size the size of the memory area
            /// \param[out] ptr a pointer to the object (the one that the function will fill)
            /// \return true if successful
            static bool from_memory(cr::allocation_transaction &, const char *, size_t , Type *)
            {
              return false;
            }

            /// \brief serialize the object
            /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
            /// \param[out] size the size of the memory area
            /// \param[in] ptr a pointer to the object (the one that the function will serialize)
            /// \return true if successful
            static bool to_memory(memory_allocator &, size_t &, const Type *)
            {
              return false;
            }
        };

        /// \brief this serialize objects (like classes) property per property
        /// this generate meta-data used to generate code that will fill the object.
        /// \param OffsetTypeList is a list of \code typed_offset <type, offsetof(my_class, member)> \endcode that could be simplified with the macro \code NRP_TYPPED_OFFSET(my_class, member) \endcode
        /// \see constructible_serializable_object
        /// \note This is the generic implementation: a backend agnostic implementation that plugs into a collection_serializable
        template<typename Backend, typename Type, typename... OffsetTypeList>
        class serializable_object
          : public persistence_helper::collection_serializable<Backend, Type, serializable_object<Backend, Type, OffsetTypeList...>, persistence_helper::compiletime>
        {
          public:
            /// \brief We're using this 'cause this allow us to forward the OffsetTypeList to the collection_serializable specification at compile time
            template<typename OffsetType>
            struct sub_compile_time_t
            {
              using type_t = typename OffsetType::type;

              template<typename... Params>
              static inline bool from_memory_single(cr::allocation_transaction &transaction, Type *ptr, const char *sub_memory, size_t sub_size, Params && ...p)
              {
                return from_memory_single_value(transaction, ptr, sub_memory, sub_size, std::forward<Params>(p)...);
              }

              template<typename... Params>
              static inline bool from_memory_single_key(cr::allocation_transaction &, Type *, const char *, size_t , Params && ...)
              {
                return true; // Well.
              }

              template<typename... Params>
              static inline bool from_memory_single_value(cr::allocation_transaction &transaction, Type *ptr, const char *v_memory, size_t v_size, Params && ...v_p)
              {
                return persistence::serializable<Backend, type_t>::from_memory(transaction, v_memory, v_size, reinterpret_cast<type_t *>(reinterpret_cast<uint8_t *>(ptr) + OffsetType::offset), std::forward<Params>(v_p)...);
              }


              template<typename... Params>
              static inline bool to_memory_single(memory_allocator &mem, size_t &size, const Type *ptr, Params && ... p)
              {
                return to_memory_single_value(mem, size, ptr, std::forward<Params>(p)...);
              }

              template<typename... Params>
              static inline bool to_memory_single_key(memory_allocator &mem, size_t &size, const Type *, Params && ... p)
              {
                return persistence::serializable<Backend, char *>::to_memory(mem, size, &OffsetType::name, std::forward<Params>(p)...);
              }

              template<typename... Params>
              static inline bool to_memory_single_value(memory_allocator &mem, size_t &size, const Type *ptr, Params && ... p)
              {
                return persistence::serializable<Backend, type_t>::to_memory(mem, size, reinterpret_cast<const type_t *>(reinterpret_cast<const uint8_t *>(ptr) + OffsetType::offset), std::forward<Params>(p)...);
              }
            };

            /// \brief The compile time struct
            using compile_time_t = neam::ct::type_list<sub_compile_time_t<OffsetTypeList>...>;

            static bool default_initializer(cr::allocation_transaction &, Type *)
            {
              return false;
//               bool res = true;
//               NEAM_EXECUTE_PACK(
//                 res &= (
//                   persistence::serializable<Backend, typename OffsetTypeList::type>::default_initializer(transaction, reinterpret_cast<typename OffsetTypeList::type *>(reinterpret_cast<uint8_t *>(ptr) + OffsetTypeList::offset))
//                 )
//               );
//               return res;
            }

            static inline bool from_memory_null(cr::allocation_transaction &transaction, Type *ptr) // When the size is 0 or no data is given
            {
              return default_initializer(transaction, ptr);
            }

            static inline bool from_memory_allocate(cr::allocation_transaction &, size_t, Type *)
            {
              // it's an object !
              return true;
            }

            using single_instance_t = uint8_t;

            // Here we do something else: we just retrieve the property and uses its index to retrieve its position
            template<typename... Params>
            static inline bool from_memory_single(cr::allocation_transaction &transaction, Type *ptr, uint8_t *, const char *sub_memory, size_t sub_size, size_t index, Params && ...p)
            {
              bool res = true;
              size_t i = 0;

              // Sorry.
              NEAM_EXECUTE_PACK(
                res &= ((res && i++ == index) ?
                  persistence::serializable<Backend, typename OffsetTypeList::type>::from_memory(transaction, sub_memory, sub_size, (typename OffsetTypeList::type *)(reinterpret_cast<uint8_t *>(ptr) + OffsetTypeList::offset), std::forward<Params>(p)...)
                : true)
              );

              return res;
            }

            using kv_instance_t = int;
            static constexpr bool can_construct_inplace = true;

            template<typename... Params>
            static inline bool from_memory_single_key(cr::allocation_transaction &, Type *, void *pair, const char *k_memory, size_t k_size, Params && ...k_p)
            {
              int *index = reinterpret_cast<int *>(pair);
              *index = -1; // not found
              char *name = nullptr;
              cr::allocation_transaction temp_transaction;
              if (persistence::serializable<Backend, char *>::from_memory(temp_transaction, k_memory, k_size, &name, std::forward<Params>(k_p)...))
              {
                size_t key_size = strlen(name);

                // GET INDEX
                size_t i = 0;
                // Sorry.
                NEAM_EXECUTE_PACK(
                  ((*index == -1 && ++i && OffsetTypeList::name_len == key_size && !memcmp(name, OffsetTypeList::name, key_size)) ?
                    *index = int(i) - 1
                  : 0)
                );
                temp_transaction.rollback();
                return true;
              }
              temp_transaction.rollback();
              return false;
            }

            template<typename... Params>
            static inline bool from_memory_single_value(cr::allocation_transaction &transaction, Type *ptr, void *pair, const char *v_memory, size_t v_size, Params && ...v_p)
            {
              int *index = reinterpret_cast<int *>(pair);
              if (*index != -1)
                return from_memory_single(transaction, ptr, nullptr, v_memory, v_size, *index, std::forward<Params>(v_p)...);
              return true;
            }

            static inline bool from_memory_single_push_kv(cr::allocation_transaction &, Type *, void *)
            {
              return true;
            }

            static inline bool from_memory_end(cr::allocation_transaction &, Type *)
            {
              // TODO: default initialize fields
              return true;
            }


            static constexpr bool should_be_serialized_as_collection = neam::cr::persistence_helper::should_be_serialized_as_collection<Backend, const char *>::value;

            static inline size_t to_memory_get_iterator(const Type *)
            {
              return 0;
            }

            static inline size_t to_memory_get_element_count(const Type *)
            {
              return sizeof...(OffsetTypeList);
            }

            static inline bool to_memory_increment_iterator(size_t &it)
            {
              ++it;
              return true;
            }

            template<typename... Params>
            static inline bool to_memory_single(memory_allocator &mem, size_t &size, size_t &it, const Type *ptr, Params && ... p)
            {
              if (it >= sizeof...(OffsetTypeList))
                return false;
              bool res = true;
              size_t i = 0;
              // Sorry.
              NEAM_EXECUTE_PACK(
                res &= ((res && i++ == it) ?
                  persistence::serializable<Backend, typename OffsetTypeList::type>::to_memory(mem, size, reinterpret_cast<const typename OffsetTypeList::type *>(reinterpret_cast<const uint8_t *>(ptr) + OffsetTypeList::offset), std::forward<Params>(p)...)
                : true)
              );
              return res;
            }

            template<typename... Params>
            static inline bool to_memory_single_key(memory_allocator &mem, size_t &size, size_t &it, const Type *, Params && ... p)
            {
              if (it >= sizeof...(OffsetTypeList))
                return false;
              bool res = true;
              size_t i = 0;
              // Sorry.
              NEAM_EXECUTE_PACK(
                res &= ((res && i++ == it) ?
                  persistence::serializable<Backend, char *>::to_memory(mem, size, &OffsetTypeList::name, std::forward<Params>(p)...)
                : true)
              );
              return res;
            }

            template<typename... Params>
            static inline bool to_memory_single_value(memory_allocator &mem, size_t &size, size_t &it, const Type *ptr, Params && ... p)
            {
              return to_memory_single(mem, size, it, ptr, std::forward<Params>(p)...);
            }

            static inline bool to_memory_end_iterator(size_t &)
            {
              // TODO: default init
              return true;
            }
        };


        /// \brief construct an object and then call a method called \e post_deserialization
        /// behave exactly like the class serializable_object.
        /// For an example, see the README.md at the root of the project.
        /// \see class serializable_object
        /// \see neam::ct::constructor
        /// \see N_CALL_POST_FUNCTION
        template<typename Backend, typename Type, typename ConstructorCall, typename... OffsetTypeList>
        class constructible_serializable_object
        {
          private:
            using object_t = Type;

          public:
            /// \brief deserialize the object
            /// \param[in] memory the serialized object
            /// \param[in] size the size of the memory area
            /// \param[out] ptr a pointer to the object (the one that the function will fill)
            /// \return true if successful
            template<typename... Params>
            static bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, Type *ptr, Params... p)
            {
              allocation_transaction temp_transaction;
              if (!serializable_object<Backend, Type, OffsetTypeList...>::from_memory(temp_transaction, memory, size, ptr, std::forward<Params>(p)...))
              {
                temp_transaction.rollback();
                return false;
              }

              try
              {
                // call the post-deserialization callbacl of the newly-initialized object (magic ?)
                ConstructorCall::forward_to(reinterpret_cast<typename ConstructorCall::type *>(ptr), &ConstructorCall::type::post_deserialization);
              }
              catch (...)
              {
                temp_transaction.rollback();
                return false;
              }

              temp_transaction.complete();
              transaction.register_destructor_call_on_failure(ptr);

              return true;
            }

            /// \brief serialize the object
            /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
            /// \param[out] size the size of the memory area
            /// \param[in] ptr a pointer to the object (the one that the function will serialize)
            /// \return true if successful
            template<typename... Params>
            static bool to_memory(memory_allocator &mem, size_t &size, const Type *ptr, Params... p)
            {
              // simply forward to serializable_object as we don't have to do anything here.
              return serializable_object<Backend, Type, OffsetTypeList...>::to_memory(mem, size, ptr, std::forward<Params>(p)...);
            }
        };

#define N_CALL_POST_FUNCTION(ObjectType, ...)             N_CT_CONSTRUCTOR_CALL_PLACEMENT(ObjectType, ##__VA_ARGS__)


        /// \see serializable_object
        /// \note \p OffsetIsAbsolute is used to determine whether or not the offset is relative to the this pointer or absolute.
        template<typename Type, typename Object, size_t Offset, const char *Name = nullptr, bool AbsoluteOffset = false>
        struct typed_offset
        {
          using type = Type;
          using object = Object;
          constexpr static size_t offset = Offset;
          constexpr static const char *name = Name;
          constexpr static bool absolute_offset = AbsoluteOffset;
          constexpr static size_t name_len = neam::ct::strlen(Name);
        };

// NOTE: GCC issues a warning for offsetof() usage on some objects (but still, the code works).
//       Clang issues an error for the GCC solution, so, this is the reason for that switch
#if defined(__GNUC__) && !defined(__clang__)
#define N__OFFSETOF(class, member)                              reinterpret_cast<size_t>(&(reinterpret_cast<class *>(0)->member))
#else
#define N__OFFSETOF(class, member)                              offsetof(class, member)
#endif

/// \brief an helper for template classes (that CPP doesn't like as macro argument...)
#ifdef IN_IDE_PARSER // sorry :(
// only in the IDE, in order to keep syntactic coloration working...
#define NCRP_TEMPLATE_CLASS(...)                                neam::cr::persistence
#else
// this one is for the C++ compiler
#define NCRP_TEMPLATE_CLASS(...)                                decltype(__VA_ARGS__(/*...*/))
#endif
/// \brief compute the type and the offset of the \e member of \e class
/// \see class serializable_object
/// \see class constructible_serializable_object
#define NCRP_TYPED_OFFSET(class, member)                        neam::cr::persistence::typed_offset<decltype(class::member), class, N__OFFSETOF(class, member)>

/// \brief same as \b NCRP_TYPED_OFFSET, but with an absolute offset (not relative to a this pointer. For static properties and member methods).
#define NCRP_TYPED_ABSOLUTE_OFFSET(class, member)               neam::cr::persistence::typed_offset<decltype(class::member), class, N__OFFSETOF(class, member), true>

/// \brief same as \b NCRP_TYPED_OFFSET, but the user also specify the name
/// \note \p name MUST be a (extern) constexpr neam::string_t variable !
#define NCRP_NAMED_TYPED_OFFSET(class, member, name)            neam::cr::persistence::typed_offset<decltype(class::member), class, N__OFFSETOF(class, member), name>

/// \brief same as \b NCRP_NAMED_TYPED_OFFSET, but with an absolute offset (not relative to a this pointer. For static properties and member methods).
#define NCRP_NAMED_TYPED_ABSOLUTE_OFFSET(class, member, name)   neam::cr::persistence::typed_offset<decltype(class::member), class, N__OFFSETOF(class, member), name, true>

/// \brief same as \b NCRP_TYPED_OFFSET, but the user also specify the type
#define NCRP_OFFSET(type, class, member)                        neam::cr::persistence::typed_offset<type, class, N__OFFSETOF(class, member)>

/// \brief same as \b NCRP_OFFSET, but with an absolute offset (not relative to a this pointer. For static properties and member methods).
#define NCRP_ABSOLUTE_OFFSET(type, class, member)               neam::cr::persistence::typed_offset<type, class, N__OFFSETOF(class, member), nullptr, true>

/// \brief same as \b NCRP_OFFSET, but the user also specify the name of the field
/// \note \p name MUST be a (extern) constexpr neam::string_t variable !
#define NCRP_NAMED_OFFSET(type, class, member, name)            neam::cr::persistence::typed_offset<type, class, N__OFFSETOF(class, member), name>

/// \brief same as \b NCRP_NAMED_OFFSET, but with an absolute offset (not relative to a this pointer. For static properties and member methods).
#define NCRP_NAMED_ABSOLUTE_OFFSET(type, class, member, name)   neam::cr::persistence::typed_offset<type, class, N__OFFSETOF(class, member), name, true>

/// \brief declare a variable that will hold the name of the field
/// \note be careful with the namespaces !
#define NCRP_DECLARE_NAME(class, name)                          namespace names{namespace class{ constexpr neam::string_t name __attribute__((used)) = #name; }}

/// \brief like NCRP_TYPED_OFFSET, but surround the computed type with a wrapper (like \e checksum or \e magic)
#define NCRP_WRAPPED_TYPED_OFFSET(class, member, wrapper, ...)  neam::cr::persistence::typed_offset<wrapper<decltype(class::member), ##__VA_ARGS__>, class, N__OFFSETOF(class, member)>

        // no one could instanciate this class.
        // NO ONE.
        persistence() = delete;
        ~persistence() = delete;
    }; // class persistence

    // TODO: find a better alternative to this...
    template<typename Type, typename Object, size_t Offset, const char *Name, bool AbsoluteOffset>
    constexpr const char *persistence::typed_offset<Type, Object, Offset, Name, AbsoluteOffset>::name;

  } // namespace cr
} // namespace neam

#include "serializable_specs_gen.hpp"

#include "serializable_wrappers.hpp"

#include "serializable_specs_neam.hpp"
#include "serializable_specs_verbose.hpp"
#include "json_backend/serializable_specs_json.hpp"

#endif /*__N_2006814652382068822_103083989__OBJECT_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on;


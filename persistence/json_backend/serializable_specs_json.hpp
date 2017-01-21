//
// file : serializable_specs.hpp
// in : file:///home/tim/projects/reflective/reflective/persistence/serializable_specs_json.hpp
//
// created by : Timothée Feuillet on linux.site
// date: 21/09/2014 12:52:25
//
//
// Copyright (c) 2014-2016 Timothée Feuillet
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

#ifndef __N_331927306113683385_1782336113__SERIALIZABLE_SPECS_JSON_HPP__
# define __N_331927306113683385_1782336113__SERIALIZABLE_SPECS_JSON_HPP__

#include <type_traits>
#include <string>
#include <sstream>
#include <iomanip>
#include "../tools/array_wrapper.hpp"
#include "../tools/demangle.hpp"
#include "../object.hpp" // for my IDE
#include "../raw_data.hpp"

#include "serializable_specs_json_internal.hpp"

namespace neam
{
  namespace cr
  {
    /// \brief Pointer serializer
    template<typename Type>
    class persistence::serializable<persistence_backend::json, Type *, typename std::enable_if<!std::is_same<Type, char>::value && !std::is_same<Type, const char>::value, void>::type>
    {
      public:
        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        template<typename... Params>
        static inline bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, Type **ptr, Params &&... p)
        {
          // handle the null pointer case
          if (size == 4 && memcmp(memory, "null", 4) == 0)
          {
            *ptr = nullptr;
            return true;
          }

          Type *tptr = reinterpret_cast<Type *>(transaction.allocate_raw(sizeof(Type)));
          if (!tptr)
            return false;
          *ptr = tptr;
          return serializable<persistence_backend::json, Type>::from_memory(transaction, memory, size, tptr, std::forward<Params>(p)...);
        }

        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &, Type **ptr)
        {
          *ptr = nullptr;
          return true;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static inline bool to_memory(memory_allocator &mem, size_t &size, const Type* const* ptr, size_t indent = 0)
        {
          // handle the null pointer case
          if (!*ptr)
            return internal::json::_allocate_format_string(mem, size, 0, nullptr, "null");
          return serializable<persistence_backend::json, Type>::to_memory(mem, size, *ptr, indent);
        }
    };

    /// \brief boolean serializer
    template<>
    class persistence::serializable<persistence_backend::json, bool>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &, bool *ptr)
        {
          *ptr = false;
          return true;
        }

        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static inline bool from_memory(cr::allocation_transaction &, const char *memory, size_t size, bool *ptr)
        {
          internal::json::types type = internal::json::get_type(memory[0]);
          if (type != internal::json::types::other)
            return false;
          // handle the true pointer case
          if (size == 4 && memcmp(memory, "true", 4) == 0)
          {
            *ptr = true;
            return true;
          }
          // handle the false pointer case
          if (size == 5 && memcmp(memory, "false", 5) == 0)
          {
            *ptr = false;
            return true;
          }

          return false;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, const bool *ptr, size_t = 0)
        {
          if (*ptr)
            return internal::json::_allocate_format_string(mem, size, 0, nullptr, "true");
          else
            return internal::json::_allocate_format_string(mem, size, 0, nullptr, "false");
        }
    };

    /// \brief the default serializer for numeric types
    template<typename Type>
    class persistence::serializable<persistence_backend::json, Type, internal::numeric>
    {
      static_assert(std::is_arithmetic<Type>::value, "only arithmetic types here !!!");
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &, Type *ptr)
        {
          *ptr = 0;
          return true;
        }

        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static inline bool from_memory(cr::allocation_transaction &, const char *memory, size_t, Type *ptr)
        {
          internal::json::types type = internal::json::get_type(memory[0]);
          if (type != internal::json::types::number)
            return false;
          return internal::json::number_from_string(memory, *ptr);
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, const Type *ptr, size_t = 0)
        {
          std::ostringstream os;
          if (std::is_floating_point<Type>::value && std::numeric_limits<Type>::max_digits10 > 0)
            os << std::setprecision(std::numeric_limits<Type>::max_digits10);
          os << (*ptr);
          return internal::json::_allocate_format_string(mem, size, 0, nullptr, os.str());
        }
    };

    // for arithmetic types
    template<> class persistence::serializable<persistence_backend::json, char> : public persistence::serializable<persistence_backend::json, char, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::json, unsigned char> : public persistence::serializable<persistence_backend::json, unsigned char, internal::numeric> {};

    template<> class persistence::serializable<persistence_backend::json, short> : public persistence::serializable<persistence_backend::json, short, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::json, unsigned short> : public persistence::serializable<persistence_backend::json, unsigned short, internal::numeric> {};

    template<> class persistence::serializable<persistence_backend::json, int> : public persistence::serializable<persistence_backend::json, int, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::json, unsigned int> : public persistence::serializable<persistence_backend::json, unsigned int, internal::numeric> {};

    template<> class persistence::serializable<persistence_backend::json, long> : public persistence::serializable<persistence_backend::json, long, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::json, unsigned long> : public persistence::serializable<persistence_backend::json, unsigned long, internal::numeric> {};

    template<> class persistence::serializable<persistence_backend::json, float> : public persistence::serializable<persistence_backend::json, float, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::json, double> : public persistence::serializable<persistence_backend::json, double, internal::numeric> {};
    template<> class persistence::serializable<persistence_backend::json, long double> : public persistence::serializable<persistence_backend::json, long double, internal::numeric> {};

    /// \brief a special case for C strings
    template<>
    class persistence::serializable<persistence_backend::json, char *>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &, char **ptr)
        {
          *ptr = nullptr;
          return true;
        }

        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static inline bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, char **ptr)
        {
          internal::json::types type = internal::json::get_type(memory[0]);
          // handle the null pointer case
          if (type == internal::json::types::other && size == 4 && memcmp(memory, "null", 4) == 0)
          {
            *ptr = nullptr;
            return true;
          }

          if (type != internal::json::types::string)
            return false;

          std::string un = internal::json::unescape_string(memory + 1, size - 1);

          // We have to remove the two quotation marks from the memory area
          *ptr = reinterpret_cast<char *>(transaction.allocate_raw(un.size() + 1));
          if (*ptr)
          {
            memcpy(*ptr, un.data(), un.size());
            (*ptr)[un.size()] = 0;
          }
          return !!*ptr;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        /// \note the stored string doesn't have the null byte stored (as we store its size instead)
        static bool to_memory(memory_allocator &mem, size_t &size, const char *const*const ptr, size_t = 0)
        {
          if (!*ptr)
            return internal::json::_allocate_format_string(mem, size, 0, nullptr, "null");
          return internal::json::_allocate_format_string(mem, size, 0, nullptr, "\"" + internal::json::escape_string(*ptr) + "\"");
        }
    };

    template<>
    class persistence::serializable<persistence_backend::json, raw_data>
    {
      public:
        /// \brief The default initializer, if nothing is provided to initialize this field in the JSON
        static inline bool default_initializer(cr::allocation_transaction &transaction, raw_data *ptr)
        {
          new(ptr) raw_data(); // call the placement new for the default constructor
          transaction.register_destructor_call_on_failure(ptr);
          return true;
        }

        /// \brief deserialize the object
        /// \param[in] memory the serialized object
        /// \param[in] size the size of the memory area
        /// \param[out] ptr a pointer to the object (the one that the function will fill)
        /// \return true if successful
        static inline bool from_memory(cr::allocation_transaction &transaction, const char *memory, size_t size, raw_data *ptr)
        {
          // handle the null pointer case
          internal::json::types type = internal::json::get_type(memory[0]);
          if (type == internal::json::types::other && size == 4 && memcmp(memory, "null", 4) == 0)
          {
            new(ptr) raw_data(); // call the placement new for the default constructor
            transaction.register_destructor_call_on_failure(ptr);
            return true;
          }

          if (type != internal::json::types::string)
            return false;
          ++memory;

          // We have to remove the two quotation marks from the memory area
          uint8_t *data = reinterpret_cast<uint8_t *>(transaction.allocate_raw(size - 1));
          if (data)
          {
            // unescape the string
            size_t i = 0;
            size_t shift = 0;
            for (; i + shift < size - 1; ++i)
            {
              if (memory[i + shift] == '\\')
              {
                ++shift;
                if (i + shift >= size - 1)
                  return false;
                switch (memory[i + shift])
                {
                  case '\\':
                  case '"': data[i] = memory[i + shift]; break;
                  case 'n': data[i] = '\n'; break;
                  case 't': data[i] = '\t'; break;
                  case 'b': data[i] = '\b'; break;
                  case 'r': data[i] = '\r'; break;
                  case 'f': data[i] = '\f'; break;
                  case 'u': // convert the following 4 [0-9A-F] code to binary data
                    ++shift;
                    if (i + shift + 3 >= size - 1)
                      return false;
                    char sdata[] = {memory[i + shift + 0], memory[i + shift + 1], memory[i + shift + 2], memory[i + shift + 3], 0};
                    shift += 3;
                    char *_u = sdata;
                    uint16_t t = strtoul(sdata, &_u, 16);
                    if (t & 0xFF00)
                    {
                      data[i++] = t >> 8;
                      --shift; // compensate the incrementation of i
                    }
                    data[i] = t & 0xFF;
                }
              }
              else
                data[i] = memory[i + shift];
            }
            ptr->data = reinterpret_cast<int8_t *>(data);
            ptr->size = i;
            ptr->ownership = true;
            // WE DON'T REGISTER THE DESTRUCTOR IN THE TRANSACTION: WE ALREADY HAVE THE MEMORY REGISTERED TO BE DELETED
            return true;
          }
          return false;
        }

        /// \brief serialize the object
        /// \param[out] memory the serialized object (don't forget to \b free that memory !!!)
        /// \param[out] size the size of the memory area
        /// \param[in] ptr a pointer to the object (the one that the function will serialize)
        /// \return true if successful
        static bool to_memory(memory_allocator &mem, size_t &size, const raw_data *ptr, size_t = 0)
        {
          if (!ptr->data)
            return internal::json::_allocate_format_string(mem, size, 0, nullptr, "null");
          std::string data;
          data.reserve(ptr->size * 8 + 2);
          data += "\"";
          for (size_t i = 0; i < ptr->size; ++i)
          {
            if (((std::isgraph(ptr->data[i]) || ptr->data[i] > ' ') && ptr->data[i] != '"' && ptr->data[i] != '\\') || ptr->data[i] == ' ')
              data += ptr->data[i];
            else if(ptr->data[i] != '"' && ptr->data[i] != '\\')
            {
              switch (ptr->data[i])
              {
                case '\b': data += "\\b"; break;
                case '\f': data += "\\f"; break;
                case '\n': data += "\\n"; break;
                case '\r': data += "\\r"; break;
                case '\t': data += "\\t"; break;
                default:
                  data += "\\u00";
                  data += internal::json::hex_alphabet[(ptr->data[i] & 0xF0) >> 4];
                  data += internal::json::hex_alphabet[ptr->data[i] & 0x0F];
              }
            }
            else
            {
              data += "\\";
              data += ptr->data[i];
            }
          }
          data += "\"";
          return internal::json::_allocate_format_string(mem, size, 0, nullptr, data);
        }
    };

    /// \brief Helper to [de]serialize list-like objects
    namespace persistence_helper
    {
      template<typename Type, typename Caller, serializable_mode Mode>
      class list_serializable<persistence_backend::json, Type, Caller, Mode>
      {
        public:
          /// \brief Called to deserialize the list-object
          static inline bool from_memory(allocation_transaction &transaction, const char *memory, size_t size, Type *ptr)
          {
            internal::json::types type = internal::json::get_type(memory[0]);

            // handle the null pointer case
            if (type == internal::json::types::other && size == 4 && memcmp(memory, "null", 4) == 0)
              return Caller::from_memory_null(transaction, ptr);

            if (type != internal::json::types::list)
              return false;

            ++memory; // skip the opening bracket
            --size;

            int8_t temp_memory[sizeof(typename Caller::single_instance_t)];
            typename Caller::single_instance_t *temp_memory_ptr = reinterpret_cast<typename Caller::single_instance_t *>(temp_memory);

            size_t index = 0;
            if (!std::isspace(memory[0]) || internal::json::advance_next(memory, size, index))
            {
              const size_t element_count = get_slot_count(memory + index, size);
              if (element_count)
              {
                if (!Caller::from_memory_allocate(transaction, element_count, ptr))
                  return false;

                if (element_count)
                {
                  size_t element_index = 0;
                  while (true)
                  {
                    size_t end_index = index;

                    internal::json::types elem_type = internal::json::get_type(memory[index]);
                    if (!internal::json::get_element_end_index(memory, size, end_index, elem_type))
                      return false;

                    // chain unserialize
                    if (!Caller::from_memory_single(transaction, ptr, temp_memory_ptr, memory + index, end_index - index, element_index))
                      return false;

                    index = end_index;
                    ++element_index;

                    if (!internal::json::advance_next(memory, size, index, ','))
                      break; // can't do much more
                  }
                }
              }
              else
                return Caller::from_memory_null(transaction, ptr);
            }
            return Caller::from_memory_end(transaction, ptr);
          }

          /// \brief Called to serialize the list-object
          static inline bool to_memory(memory_allocator &mem, size_t &size, const Type *ptr, size_t indent_level = 0)
          {
            size_t whole_object_size = 0;

            const size_t element_count = Caller::to_memory_get_element_count(ptr);
            auto iterator = Caller::to_memory_get_iterator(ptr);

            if (!internal::json::_allocate_string(mem, whole_object_size, 0, "[\n"))
              return false;

            for (size_t index = 0; index < element_count; ++index)
            {
              if (index > 0)
              {
                if (!internal::json::_allocate_string(mem, whole_object_size, 0, ",\n"))
                  return false;
              }
              if (!internal::json::_allocate_string(mem, whole_object_size, indent_level + 1, ""))
                return false;

              size_t element_size = 0;
              if (!Caller::to_memory_single(mem, element_size, iterator, ptr, indent_level + 1))
                return false;
              whole_object_size += element_size;
              if (!Caller::to_memory_increment_iterator(iterator))
                return false;
            }
            if (!Caller::to_memory_end_iterator(iterator))
              return false;

            if (!internal::json::_allocate_string(mem, whole_object_size, 0, "\n"))
              return false;
            if (!internal::json::_allocate_string(mem, whole_object_size, indent_level, "]"))
              return false;

            size = whole_object_size;

            return true;
          }

        private:
          /// \todo find something better than this slow thing
          static inline size_t get_slot_count(const char *memory, size_t size)
          {
            size_t count = 0;
            size_t index = 0;
            while (true)
            {
              size_t end_index = index;

              // chain unserialize
              internal::json::types elem_type = internal::json::get_type(memory[index]);
              end_index = index;
              if (!internal::json::get_element_end_index(memory, size, end_index, elem_type))
                return 0;
              ++count;
              index = end_index;

              if (!internal::json::advance_next(memory, size, index, ','))
                break; // can't do much more
            }
            return count;
          }
      };

      template<> struct should_be_serialized_as_collection<persistence_backend::json, const char *> : public std::true_type {};
      template<> struct should_be_serialized_as_collection<persistence_backend::json, char *> : public std::true_type {};
      template<> struct should_be_serialized_as_collection<persistence_backend::json, char *const> : public std::true_type {};
      template<> struct should_be_serialized_as_collection<persistence_backend::json, const char *const> : public std::true_type {};
      template<typename CharT, typename Traits, typename Alloc> struct should_be_serialized_as_collection<persistence_backend::json, std::basic_string<CharT, Traits, Alloc>> : public std::true_type {};
      template<typename CharT, typename Traits, typename Alloc> struct should_be_serialized_as_collection<persistence_backend::json, const std::basic_string<CharT, Traits, Alloc>> : public std::true_type {};

      /// \brief Helper to [de]serialize collection-like objects
      /// In this backend, this is exactly as the list serializer. (code re-use ftw).
      template<typename Type, typename Caller, serializable_mode Mode>
      class collection_serializable<persistence_backend::json, Type, Caller, Mode>
      {
        public:
          static inline bool from_memory(allocation_transaction &transaction, const char *memory, size_t size, Type *ptr)
          {
            internal::json::types type = internal::json::get_type(memory[0]);
            if (type == internal::json::types::list)
              return list_serializable<persistence_backend::json, Type, Caller, Mode>::from_memory(transaction, memory, size, ptr);
            if (type != internal::json::types::collection)
              return false;

            --size;   // skip the closing brace
            ++memory; // skip the opening brace

            if (!Caller::from_memory_allocate(transaction, 0, ptr))
              return false;

            // I know that this is unforgivable, but I want to have an object without dynamic allocation and without calling its constructor
            using temp_memory_t = typename Caller::kv_instance_t;
            int8_t temp_memory[sizeof(temp_memory_t)];
            temp_memory_t *temp_memory_ptr = reinterpret_cast<temp_memory_t *>(temp_memory);

            size_t index = 0;
            if (memory[0] == '"' || internal::json::advance_next(memory, size, index))
            {
              while (true)
              {
                // retrieve the string index
                if (internal::json::get_type(memory[index]) != internal::json::types::string)
                  return false;
                size_t end_index = index;
                if (!internal::json::get_element_end_index(memory, size, end_index, internal::json::types::string)) // the JSON is not well formatted
                  return false;

                allocation_transaction temp_transaction;
                allocation_transaction *transaction_ptr = (Caller::can_construct_inplace ? &transaction : &temp_transaction);

                // get the key (must be a string)
                if (!Caller::from_memory_single_key(*transaction_ptr, ptr, temp_memory_ptr, memory + index, end_index - index))
                  return false;

                // skip the : token
                index = end_index;
                if (!internal::json::advance_next(memory, size, index, ':'))
                  return false;

                // chain unserialize
                internal::json::types elem_type = internal::json::get_type(memory[index]);
                end_index = index;
                if (!internal::json::get_element_end_index(memory, size, end_index, elem_type))
                  return false;

                // get the value
                if (!Caller::from_memory_single_value(*transaction_ptr, ptr, temp_memory_ptr, memory + index, end_index - index))
                  return false;
                index = end_index;

                if (!Caller::from_memory_single_push_kv(transaction, ptr, temp_memory_ptr))
                  return false;

                // call the destructor of the temporary memory
                temp_transaction.rollback();

                if (!internal::json::advance_next(memory, size, index, ','))
                  break; // can't do much more
              }
            }
            return Caller::from_memory_end(transaction, ptr);
          }

          static inline bool to_memory(NCR_ENABLE_IF(!(Mode & to_memory_compiletime), memory_allocator) &mem, size_t &size, const Type *ptr, size_t indent_level = 0)
          {
            if (!Caller::should_be_serialized_as_collection)
              return list_serializable<persistence_backend::json, Type, Caller, Mode>::to_memory(mem, size, ptr, indent_level);

            size_t whole_object_size = 0;

            if (!internal::json::_allocate_string(mem, whole_object_size, 0, "{\n"))
              return false;

            const size_t element_count = Caller::to_memory_get_element_count(ptr);
            auto it = Caller::to_memory_get_iterator(ptr);

            bool first = true;
            for (size_t i = 0; i < element_count; ++i)
            {
              if (!first)
              {
                if (!internal::json::_allocate_string(mem, whole_object_size, 0, ",\n"))
                  return false;
              }
              if (!internal::json::_allocate_string(mem, whole_object_size, indent_level + 1, ""))
                return false;
              size_t sz = 0;
              if (!Caller::to_memory_single_key(mem, sz, it, ptr, indent_level + 1))
                return false;

              whole_object_size += sz;

              if (!internal::json::_allocate_string(mem, whole_object_size, 0, ":"))
                return false;

              sz = 0;

              if (!Caller::to_memory_single_value(mem, sz, it, ptr, indent_level + 1))
                return false;

              whole_object_size += sz;
              first = false;

              if (!Caller::to_memory_increment_iterator(it))
                return false;
            }
            if (!Caller::to_memory_end_iterator(it))
              return false;
            if (!internal::json::_allocate_string(mem, whole_object_size, 0, "\n"))
              return false;
            if (!internal::json::_allocate_string(mem, whole_object_size, indent_level, "}"))
              return false;
            size = whole_object_size;
            return true;
          }

          // compile time to_memory
          static inline bool to_memory(NCR_ENABLE_IF((Mode & to_memory_compiletime) != 0, memory_allocator) &mem, size_t &size, const Type *ptr, size_t indent_level = 0)
          {
            if (!Caller::should_be_serialized_as_collection)
              return list_serializable<persistence_backend::json, Type, Caller, Mode>::to_memory(mem, size, ptr, indent_level);

            size_t whole_object_size = 0;

            if (!internal::json::_allocate_string(mem, whole_object_size, 0, "{\n"))
              return false;

            constexpr size_t element_count = Caller::compile_time_t::size;

            if (!ct_to_memory_loop(gen_seq<element_count>(), mem, whole_object_size, ptr, indent_level))
              return false;

            if (!internal::json::_allocate_string(mem, whole_object_size, 0, "\n"))
              return false;
            if (!internal::json::_allocate_string(mem, whole_object_size, indent_level, "}"))
              return false;
            size = whole_object_size;
            return true;
          }

        private: // compile time thingies:
          template<size_t... Indexes>
          static inline bool ct_to_memory_loop(seq<Indexes...>, NCR_ENABLE_IF((Mode & to_memory_compiletime) != 0, memory_allocator) &mem, size_t &size, const Type *ptr, size_t indent_level)
          {
            bool res = true;
            NEAM_EXECUTE_PACK(
              (res &= ct_to_memory_single<Indexes>(mem, size, ptr, indent_level))
            );
            return res;
          }
          template<size_t Index>
          static inline bool ct_to_memory_single(NCR_ENABLE_IF((Mode & to_memory_compiletime) != 0, memory_allocator) &mem, size_t &size, const Type *ptr, size_t indent_level)
          {
            if (Index != 0) // Yay ! compile time switch !
            {
              if (!internal::json::_allocate_string(mem, size, 0, ",\n"))
                return false;
            }
            if (!internal::json::_allocate_string(mem, size, indent_level + 1, ""))
              return false;
            size_t sz = 0;
            if (!Caller::compile_time_t::template get_type<Index>::to_memory_single_key(mem, sz, ptr, indent_level + 1))
              return false;

            size += sz;

            if (!internal::json::_allocate_string(mem, size, 0, ":"))
              return false;

            sz = 0;

            if (!Caller::compile_time_t::template get_type<Index>::to_memory_single_value(mem, sz, ptr, indent_level + 1))
              return false;

            size += sz;
            return true;
          }
      };
    } // namespace persistence_helper
  } // namespace cr
} // namespace neam

#endif /*__N_331927306113683385_1782336113__SERIALIZABLE_SPECS_JSON_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


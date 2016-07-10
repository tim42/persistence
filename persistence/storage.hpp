//
// file : storage.hpp
// in : file:///home/tim/projects/reflective/reflective/persistence/storage.hpp
//
// created by : Timothée Feuillet on linux.site
// date: 20/09/2014 18:23:28
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

#ifndef __N_14097779492124764538_1369986451__STORAGE_HPP__
# define __N_14097779492124764538_1369986451__STORAGE_HPP__

#include <string>
#include <map>
#include <fstream>
#include "object.hpp"

namespace neam
{
  namespace cr
  {
    /// \brief this is a \e reflective storage file (or whatever)
    /// it provide a way to serialize / deserialize information from objects directly to a file
    /// \note a file can contains multiple objects (they are named)
    class storage
    {
      public:
        storage(const std::string &filename);
        ~storage();

        /// \brief return the filename
        std::string get_filename() const;

        /// \brief return \b true if the file exists
        bool exists() const;

        /// \brief truncate the file (but keep it valid: storage::is_valid() will still return \b true)
        void truncate();

        /// \brief return \b true if the file is a valid \e storage file
        bool is_valid();

        /// \brief test whether or not if the storage object contains the object with the name \e name
        bool contains(const std::string &name) const;

        /// \brief remove a section from the file
        void remove(const std::string &name);

        /// \brief write the object to the file
        template<typename Object>
        bool write_to_file(const std::string &name, const Object &obj)
        {
          size_t size = 0;

          memory_allocator mem;
          if (!neam::cr::persistence::serializable<persistence_backend::neam, checksum<Object>>::to_memory(mem, size, const_cast<Object *>(&obj)))
            return false;

          return _write_to_file(name, reinterpret_cast<char *>(mem.get_contiguous_data()), size);
        }

        template<typename Object>
        Object *load_from_file(const std::string &name)
        {
          char *memory = nullptr;
          size_t size = 0;

          if (!_read_from_file(name, memory, size))
            return nullptr;

          cr::allocation_transaction transaction;
          Object *ret = reinterpret_cast<Object *>(transaction.allocate_raw(sizeof(Object)));

          if (!ret)
            return nullptr;

          if (!neam::cr::persistence::serializable<persistence_backend::neam, checksum<Object>>::from_memory(transaction, memory, size, ret))
          {
            transaction.rollback();
            return nullptr;
          }
          transaction.complete();
          return ret;
        }

        /// \brief write serialized data to the file
        bool _write_to_file(const std::string &name, char *memory, size_t size);

        /// \brief return the data from a named section of the file
        bool _read_from_file(const std::string &name, char *&memory, size_t &size);

        /// \brief sync the changes in memory with the file
        void _sync();

        /// \brief (re) load the file
        bool _load();

      private:
        std::map<std::string, raw_data> *mapped_file;
        std::fstream file;
        std::string filename;

    };
  } // namespace r
} // namespace neam

#endif /*__N_14097779492124764538_1369986451__STORAGE_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


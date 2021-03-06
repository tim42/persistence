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

#include "storage.hpp"
#include "stl/map.hpp"
#include "stl/string.hpp"

neam::cr::storage::storage(const std::string &_filename) : mapped_file(nullptr), filename(_filename)
{
  file.open(filename, std::ios_base::binary | std::ios_base::in | std::ios_base::out);

  _load();
}

neam::cr::storage::~storage()
{
  _sync();
  if (mapped_file)
    delete mapped_file;
}

std::string neam::cr::storage::get_filename() const
{
  return filename;
}

bool neam::cr::storage::exists() const
{
  return file.is_open();
}

bool neam::cr::storage::is_valid()
{
  return exists() && mapped_file;
}

void neam::cr::storage::truncate()
{
  if (mapped_file)
    mapped_file->clear();
  else
    mapped_file = new std::map<std::string, raw_data>;
  _sync();
}

bool neam::cr::storage::contains(const std::string &name) const
{
  if (mapped_file)
    return mapped_file->count(name);
  return false;
}

void neam::cr::storage::remove(const std::string &name)
{
  if (mapped_file)
    mapped_file->erase(name);
}

bool neam::cr::storage::_read_from_file(const std::string &name, char *&memory, size_t &size)
{
  memory = nullptr;
  size = 0;

  if (!mapped_file)
    return false;

  auto it = mapped_file->find(name);
  if (it == mapped_file->end())
    return false;

  memory = reinterpret_cast<char *>(it->second.data);
  size = it->second.size;
  return true;
}

bool neam::cr::storage::_write_to_file(const std::string &name, char *memory, size_t size)
{
  if (!mapped_file)
    mapped_file = new std::map<std::string, raw_data>;
  mapped_file->erase(name);
  mapped_file->emplace(name, (raw_data(size, reinterpret_cast<int8_t *>(memory), neam::force_duplicate)));

  _sync();
  return true;
}

void neam::cr::storage::_sync()
{
  if (!mapped_file)
    mapped_file = new std::map<std::string, raw_data>;

  size_t size = 0;

  memory_allocator mem;
  if (!neam::cr::persistence::serializable<persistence_backend::neam, xor_data<std::map<std::string, raw_data>>>::to_memory(mem, size, mapped_file))
    return;

  file.close();
  file.open(filename, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
  if (file.is_open())
  {
    if (mem.size() != size)
      abort();
    file.write(reinterpret_cast<const char *>(mem.get_contiguous_data()), size);
    file.flush();
    file.sync(); // yay.
  }
}

bool neam::cr::storage::_load()
{
  if (!file)
    return false;

  if (mapped_file)
    delete mapped_file;
  mapped_file = nullptr;

  file.sync();
  file.seekg(0, std::ios_base::end);
  size_t size = file.tellg();
  file.seekg(0, std::ios_base::beg);

  if (!size)
    return false;

  char *memory = new char[size];

  file.read(memory, size);
  cr::allocation_transaction transaction;
  if (!neam::cr::persistence::serializable<persistence_backend::neam, xor_data<std::map<std::string, raw_data> *>>::from_memory(transaction, memory, size, &mapped_file))
  {
    mapped_file = nullptr;
    transaction.rollback();
    delete [] memory;
    return false;
  }

  transaction.complete();

  delete [] memory;

  return !!mapped_file;
}

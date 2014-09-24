//
// file : memory_allocator.hpp
// in : file:///home/tim/projects/persistence/persistence/tools/memory_allocator.hpp
//
// created by : Timothée Feuillet on linux.site
// date: 22/09/2014 11:00:31
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

#ifndef __N_2080522430493415542_1720253011__MEMORY_ALLOCATOR_HPP__
# define __N_2080522430493415542_1720253011__MEMORY_ALLOCATOR_HPP__

#include <cstdlib>
#include <new>
#include <cstddef>
#include <cstring>

namespace neam
{
  namespace cr
  {
    /// \brief a chunked memory pool that can produce contiguous output.
    class memory_allocator
    {
      public:
        memory_allocator()
        {
        }

        ~memory_allocator()
        {
          clear();
        }

        /// \brief allocate \e count bytes at the end of the pool
        /// \return \b nullptr if allocation failed.
        void *allocate(size_t count)
        {
          if (!first || last->end_offset + count > last->size) // allocate a chunk (no chunk or no place in the last chunk)
          {
            memory_chunk *nchk = new memory_chunk;
            nchk->size = count > chunk_size ? count : chunk_size;
            nchk->data = reinterpret_cast<uint8_t *>(operator new(nchk->size, std::nothrow));
            if (!nchk->data)
            {
              delete nchk;
              return nullptr;
            }

            nchk->end_offset = count;
            if (first)
            {
              last->next = nchk;
              nchk->prev = last;
              last = nchk;
            }
            else
            {
              first = nchk;
              last = nchk;
            }
            pool_size += count;
            return nchk->data;
          }

          // there is enough room in the last chunk
          void *data = last->data + last->end_offset;
          last->end_offset += count;
          pool_size += count;
          return data;
        }

        /// \brief allocate \e count bytes at the front of the pool
        /// \return \b nullptr if allocation failed.
        void *allocate_front(size_t count)
        {
          if (!first || first->start_offset < count) // allocate a chunk (no chunk or no place left in the first chunk)
          {
            memory_chunk *nchk = new memory_chunk;
            nchk->size = count > chunk_size ? count : chunk_size;
            nchk->data = reinterpret_cast<uint8_t *>(operator new(nchk->size, std::nothrow));
            if (!nchk->data)
            {
              delete nchk;
              return nullptr;
            }

            nchk->end_offset = nchk->size;
            nchk->start_offset = count;
            if (first)
            {
              first->prev = nchk;
              nchk->next = first;
              first = nchk;
            }
            else
            {
              first = nchk;
              last = nchk;
            }
            pool_size += count;
            return nchk->data;
          }

          // there is enough room in the last chunk
          first->start_offset -= count;
          pool_size += count;
          return first->data + first->start_offset;
        }

        /// \brief remove \e count bytes at the end of the pool
        void pop(size_t count)
        {
          if (!last)
            return;
          if (count >= pool_size)
            return clear();

          while (count)
          {
            if (last != first && (last->end_offset - last->start_offset) < count)
            {
              pool_size -= last->end_offset - last->start_offset;
              count -= last->end_offset - last->start_offset;
              last->prev->next = nullptr;
              memory_chunk *lst = last;
              last = last->prev;
              delete lst;
            }
            else if (last == first && (last->end_offset - last->start_offset) < count)
            {
              delete first;
              first = nullptr;
              last = nullptr;
              pool_size = 0;
              return;
            }
            else
            {
              last->end_offset -= count;
              pool_size -= count;
              return;
            }
          }
        }

        /// \brief remove \e count bytes at the start of the pool
        void pop_front(size_t count)
        {
          if (!first || !pool_size)
            return;
          if (count >= pool_size)
            return clear();

          while (count)
          {
            if (last != first && (first->end_offset - first->start_offset) < count)
            {
              pool_size -= first->end_offset - first->start_offset;
              count -= first->end_offset - first->start_offset;
              first->prev->next = nullptr;
              memory_chunk *lst = first;
              first = first->prev;
              delete lst;
            }
            else if (last == first && (last->end_offset - last->start_offset) < count)
            {
              delete first;
              first = nullptr;
              last = nullptr;
              pool_size = 0;
              return;
            }
            else
            {
              first->start_offset += count;
              pool_size -= count;
              return;
            }
          }
        }

        /// \brief make the data contiguous.
        /// \note don't free the memory !!!
        void *get_contiguous_data()
        {
          // easy cases:
          if (!first)
            return nullptr;
          if (!first->next && first->start_offset == 0) // already contiguous
            return first->data;

          memory_chunk *new_chk = new memory_chunk;
          new_chk->data = reinterpret_cast<uint8_t *>(operator new(pool_size, std::nothrow));
          if (!new_chk->data)
          {
            delete new_chk;
            return nullptr;
          }

          new_chk->size = pool_size;
          new_chk->start_offset = 0;
          new_chk->end_offset = pool_size;

          size_t idx = 0;
          memory_chunk *next = nullptr;
          for (memory_chunk *chr = first; chr; chr = next)
          {
            memcpy(new_chk->data + idx, chr->data + chr->start_offset, chr->end_offset - chr->start_offset);
            idx += chr->end_offset - chr->start_offset;
            next = chr->next;
            delete chr;
          }

          first = new_chk;
          last = new_chk;
          return new_chk->data;
        }

        /// \brief give the the data ownership, return the pointer to the data and clear the pool
        /// \note it's up to the user to delete this pointer.
        /// \see get_contiguous_data()
        /// \see clear()
        void *give_up_data()
        {
          void *ret = get_contiguous_data();
          if (first)
            first->data = nullptr;
          clear();
          return ret;
        }

        /// \brief empty the memory pool, delete every allocated memory.
        void clear()
        {
          memory_chunk *next = nullptr;
          for (memory_chunk *chr = first; chr; chr = next)
          {
            next = chr->next;
            delete chr;
          }
          first = nullptr;
          last = nullptr;
          pool_size = 0;
        }

        /// \brief return the size of the pool
        size_t size() const
        {
          return pool_size;
        }

      private:
        struct memory_chunk
        {
          uint8_t *data = nullptr;

          size_t start_offset = 0;
          size_t end_offset = 0;
          size_t size = 0;

          memory_chunk *next = nullptr;
          memory_chunk *prev = nullptr;

          ~memory_chunk()
          {
            delete data;
          }
        };

      private:
        constexpr static size_t chunk_size = 4096;

      private:
        memory_chunk *first = nullptr;
        memory_chunk *last = nullptr;
        size_t pool_size = 0;
    };
  } // namespace cr
} // namespace neam

#endif /*__N_2080522430493415542_1720253011__MEMORY_ALLOCATOR_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


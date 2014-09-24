//
// file : memory_pool.hpp
// in : file:///home/tim/projects/yaggler/yaggler/tools/memory_pool.hpp
//
// created by : Timothée Feuillet on linux-coincoin.tim
// date: 14/02/2014 11:31:05
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

#ifndef __N_2080156092666096177_2010861593__MEMORY_POOL_HPP__
# define __N_2080156092666096177_2010861593__MEMORY_POOL_HPP__

#include <cstdint>
#include <memory>

namespace neam
{
  namespace cr
  {
    /// \brief this class provide a fast and easy way to deal with 
    /// \param ObjectType is the type of object stored in the pool
    /// \param ObjectCount is the number of object per chunk.
    ///
    /// \attention This pool is oversimplified. Some allocation schemes won't be optimal at all...
    /// an optimal use would be that after a short initialization phase, the number
    /// of elements allocated remain almost always the same
    ///
    /// \note the memory is held until someone call \e clear or the pool is destructed.
    template<typename ObjectType, size_t ObjectCount = 420>
    class memory_pool
    {
      private: // helpers
      private: // types
        struct chunk;

        // free memory :)
        struct allocation_slot
        {
          union// just to get the size.
          {
            allocation_slot *next;
            ObjectType obj;
          };
          uint8_t free;

          // get allocation_slot for pointer
          static allocation_slot *get_slot(void *ptr)
          {
            return reinterpret_cast<allocation_slot *>(reinterpret_cast<size_t>(ptr));
          }
        };

        struct chunk
        {
          uint32_t object_count = 0;
          uint32_t max_object_count = 0;
          chunk *next = nullptr;
          chunk *prev = nullptr;

          allocation_slot data[0]; // clang doesn't likes the '[]' array... :/
        };

      public:
        memory_pool()
          : object_count(0), chunk_count(0), first_chunk(nullptr), first_free(nullptr)
        {
        }
        memory_pool(memory_pool &&mp)
          : object_count(mp.object_count), chunk_count(mp.chunk_count), first_chunk(mp.first_chunk), first_free(mp.first_free)
        {
          mp.object_count = 0;
          mp.chunk_count = 0;
          mp.first_chunk = nullptr;
          mp.first_free = nullptr;
        }

        memory_pool &operator = (memory_pool &&mp)
        {
          if (&mp == this)
            return *this;

          clear();
          object_count = mp.object_count;
          chunk_count = mp.chunk_count;
          first_chunk = mp.first_chunk;
          first_free = mp.first_free;
          mp.object_count = 0;
          mp.chunk_count = 0;
          mp.first_chunk = nullptr;
          mp.first_free = nullptr;
          return *this;
        }

        ~memory_pool()
        {
          clear();
        }

        // no copy.
        memory_pool(const memory_pool &) = delete;
        memory_pool &operator = (const memory_pool &) = delete;

        // clear the pool, remove every chunks, every element slots.
        // NOTE: could be slow.
        void clear()
        {
          for (chunk *chk = first_chunk; chk != nullptr;)
          {
            // for each slots, call the destructor if needed
            for (size_t i = 0; i < chk->max_object_count; ++i)
            {
              if (!chk->data[i].free)
                chk->data[i].obj.~ObjectType();
            }
            chunk *next = chk->next;
            std::return_temporary_buffer(reinterpret_cast<uint8_t *>(chk));
            chk = next;
          }

          object_count = 0;
          chunk_count = 0;
          first_chunk = nullptr;
          first_free = nullptr;
        }

        // construct an allocated object
        template<typename... Args>
        ObjectType *construct(ObjectType *p, Args &&... args)
        {
          new (p) ObjectType (std::forward<Args>(args)...);
          return p;
        }

        void destroy(ObjectType *p)
        {
          p->~ObjectType();
        }

        // allocate an object (do not call the constructor)
        ObjectType *allocate()
        {
          if (!first_free) // allocate a new chunk
          {
            auto alloc_info = std::get_temporary_buffer<uint8_t>(sizeof(chunk) + ObjectCount * sizeof(allocation_slot));

            chunk *chk = reinterpret_cast<chunk *>(alloc_info.first);

            if (!chk || alloc_info.second == 0)
              return nullptr;

            chk->max_object_count = (alloc_info.second - sizeof(chunk)) / sizeof(allocation_slot);
            chk->next = first_chunk;
            chk->prev = nullptr;

            if (!chk->max_object_count) // no more memory... :/
            {
              std::return_temporary_buffer(reinterpret_cast<uint8_t *>(chk));
              return nullptr;
            }

            // init the chunk data (SLOW)
            for (size_t i = 0; i < chk->max_object_count; ++i)
            {
              chk->data[i].free = 1;
              if (i + 1 < chk->max_object_count)
                chk->data[i].next = chk->data + i + 1;
              else
                chk->data[i].next = first_free;
            }

            // push free slots
            first_free = chk->data;
            first_chunk = chk;
            ++chunk_count;
          }

          allocation_slot *slot = first_free;
          first_free = slot->next;

          slot->free = 0;
          ++object_count;
          return &slot->obj;
        }

        // deallocate an object
        // the object **MUST** be allocated by the pool (or nullptr)
        // (else you'll seg. ATTENTION: NO CHECK ARE PERFORMED.)
        void deallocate(ObjectType *p)
        {
          if (p)
          {
            // get slot and chunk
            allocation_slot *slot = allocation_slot::get_slot(p);


//          // ATTENTION: commented out because:
//          //            - this would lead to an extra memory consumption (every slot needs to store its offset from the start of the chunk)
//          //            - this would be extremely slow as when the chunk will be removed, you need to go through all the list of free
//          //              allocation_slot to remove the ones that belongs to the removed chunk
//          //              NOTE: (this could be avoided by storing a per chunk list of free slots and having a list of chunks with non-empty free slots list...)
//          //                    (and this wouldn't have a so big impact on the perfs (there would be no loops, if you store a prev and a next pointer per chunk for the free list ;) ))
//          //                    (maybe one day when I will have the time...)
//
//
//             if (!(--chk->object_count)) // free the chunk, do not push slot into free slots
//             {
//               if (chk->prev)
//                 chk->prev->next = chk->next;
//               if (chk->next)
//                 chk->next->prev = chk->prev;
//               if (chk == first_chunk)
//                 first_chunk = chk->next;
// 
//               if (!first_chunk)
//                 first_free = nullptr;
//               else
//               {
//                 // remove every free slots from the list... :(
//               }
// 
//               std::return_temporary_buffer(reinterpret_cast<uint8_t *>(chk));
//               --chunk_count;
//               return;
//             }

            // chunk has some slots lefts, push the slot into the free slots.
            slot->free = 1;
            slot->next = first_free;
            first_free = slot;
            --object_count;
          }
        }

        // gp getters
        size_t get_number_of_chunks() const
        {
          return chunk_count;
        }

        size_t get_number_of_object() const
        {
          return object_count;
        }

      private:
        size_t object_count;
        size_t chunk_count;

        chunk *first_chunk;
        allocation_slot *first_free;
    };

  } // namespace cr
} // namespace neam

template<typename ObjectType, size_t ObjectCount>
void *operator new(size_t, neam::cr::memory_pool<ObjectType, ObjectCount> &pool)
{
  return pool.allocate();
}
template<typename ObjectType, size_t ObjectCount>
void operator delete(void *ptr, neam::cr::memory_pool<ObjectType, ObjectCount> &pool)
{
  pool.deallocate(reinterpret_cast<ObjectType *>(ptr));
}

#endif /*__N_2080156092666096177_2010861593__MEMORY_POOL_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


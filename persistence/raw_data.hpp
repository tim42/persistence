//
// file : raw_data.hpp
// in : file:///home/tim/projects/persistence/persistence/raw_data.hpp
//
// created by : Timothée Feuillet on linux.site
// date: 22/09/2014 09:13:10
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

#ifndef __N_1853963877117555565_285875035__RAW_DATA_HPP__
# define __N_1853963877117555565_285875035__RAW_DATA_HPP__

#include <cstring>
#include <new>

#include <tools/ownership.hpp>

namespace neam
{
  namespace cr
  {
    /// \brief a struct that hold raw data
    /// \attention this class use the ownership memory management model, and you should be very cautionary when using this class.
    struct raw_data
    {
      uint32_t size;
      int8_t *data;

      bool ownership;

      raw_data() : size(0), data(nullptr), ownership(false) {}
      raw_data(uint32_t _size, int8_t *_data, neam::assume_ownership_t) : size(_size), data(_data), ownership(true) {}
      raw_data(uint32_t _size, int8_t *_data, neam::force_duplicate_t) : size(_size), data(_data), ownership(false)
      {
        get_ownership();
      }
      raw_data(uint32_t _size, int8_t *_data) : size(_size), data(_data), ownership(false) {}
      raw_data(const raw_data &o) : size(o.size), data(o.data), ownership(false) {}
      raw_data(const raw_data &o, neam::force_duplicate_t) : size(o.size), data(o.data), ownership(false)
      {
        get_ownership();
      }

      raw_data(raw_data && o) : size(o.size), data(o.data), ownership(o.ownership)
      {
        o.ownership = false;
      }
      raw_data(raw_data &o, neam::stole_ownership_t) : size(o.size), data(o.data), ownership(o.ownership)
      {
        o.ownership = false;
      }

      ~raw_data()
      {
        _clean();
      }

      raw_data &operator=(const raw_data &o)
      {
        if (&o == this)
          return *this;
        _clean();

        size = (o.size);
        data = o.data;
        ownership = false;
        return *this;
      }

      raw_data &operator =(raw_data && o)
      {
        if (&o == this)
          return *this;
        _clean();
        size = (o.size);
        data = (o.data);
        o.size = 0;
        o.data = nullptr;
        return *this;
      }

      void give_up_ownership()
      {
        ownership = false;
      }

      void assume_ownership()
      {
        ownership = true;
      }

      void stole_ownership(raw_data &o)
      {
        if (&o == this)
          return;
        _clean();
        ownership = o.ownership;
        o.ownership = false;
        data = o.data;
        size = o.size;
      }

      // duplicate if needed
      void get_ownership()
      {
        if (ownership)
          return;

        int8_t *tmp = data;
        data = reinterpret_cast<int8_t *>(operator new(size));
        memcpy(data, tmp, size);
        ownership = true;
      }

      raw_data &set(size_t _size, int8_t *_data)
      {
        if (_data != data)
        {
          _clean();
          ownership = false;
          data = _data;
        }

        size = _size;
        return *this;
      }

      raw_data &set(size_t _size, int8_t *_data, neam::assume_ownership_t)
      {
        if (_data != data)
        {
          _clean();
          data = _data;
        }

        ownership = true;
        size = _size;
        return *this;
      }

      void _clean()
      {
        if (ownership)
          delete data;
        ownership = false;
        data = nullptr;
        size = 0;
      }
    };
  } // namespace cr
} // namespace neam

#endif /*__N_1853963877117555565_285875035__RAW_DATA_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


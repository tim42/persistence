//
// file : raw_data.hpp
// in : file:///home/tim/projects/persistence/persistence/raw_data.hpp
//
// created by : Timothée Feuillet on linux.site
// date: 22/09/2014 09:13:10
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

#ifndef __N_1853963877117555565_285875035__RAW_DATA_HPP__
# define __N_1853963877117555565_285875035__RAW_DATA_HPP__

#include <cstring>
#include <new>

#include "tools/ownership.hpp"

namespace neam
{
  namespace cr
  {
    /// \brief a struct that hold raw data
    /// \attention this class use the ownership memory management model, and you should be very cautionary when using this class.
    struct raw_data
    {
      size_t size;
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
        ownership = o.ownership;
        o.size = 0;
        o.data = nullptr;
        o.ownership = false;
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


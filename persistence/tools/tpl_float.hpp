//
// file : tpl_float.hpp
// in : file:///home/tim/projects/yaggler/yaggler/tools/tpl_float.hpp
//
// created by : Timothée Feuillet on linux.site
// date: 24/07/2014 17:40:44
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

#ifndef __N_231085794123560898_1536835867__TPL_FLOAT_HPP__
# define __N_231085794123560898_1536835867__TPL_FLOAT_HPP__

#include <cstddef>
#include <cstdint>

//
// tpl_float: allow embedding floating point numbers in templates (see neam::embed)
// TODO: check for completeness of this implementation... (INFs, NaNs, ...)
// NOTE: doesn't support long doubles.
//

namespace neam
{
  namespace embed
  {
    template<typename Type, int64_t Mantissa, int64_t Exp>
    struct _floating_point {};

    // for floats (Exp is a power of 2)
    template<int64_t Mantissa, int64_t Exp>
    struct _floating_point<float, Mantissa, Exp>
    {
      constexpr _floating_point() {}

      constexpr static float get_val()
      {
        return Exp < 0 ? static_cast<float>(Mantissa) / static_cast<float>(2ll << -Exp) : static_cast<float>(Mantissa) * static_cast<float>(2ll << Exp);
      }

      constexpr operator float()
      {
        return get_val();
      }

      // the preferred way to use this class :)
      constexpr static float get()
      {
        return get_val();
      }

      using type = float;
      static constexpr float value __attribute__((deprecated)) = get_val();
    };

    // for double (Exp is a power of 2)
    template<int64_t Mantissa, int64_t Exp>
    struct _floating_point<double, Mantissa, Exp>
    {
      constexpr _floating_point() {}

      constexpr static double get_val()
      {
        return Exp < 0 ? static_cast<double>(Mantissa) / static_cast<double>(2ll << -Exp) : static_cast<double>(Mantissa) * static_cast<double>(2ll << Exp);
      }

      constexpr operator double()
      {
        return get_val();
      }

      // the preferred way to use this class :)
      constexpr static double get()
      {
        return get_val();
      }

      using type = double;
      static constexpr double value __attribute__((deprecated)) = get_val();
    };

    // embedding for floats/doubles
    template<int64_t Mantissa, int64_t Exp>
    using floating_point = _floating_point< float, Mantissa, Exp >;
    template<int64_t Mantissa, int64_t Exp>
    using double_floating_point = _floating_point< double, Mantissa, Exp >;

#define N_EMBED_FLOAT(Mantissa, Exp)        neam::embed::floating_point<Mantissa, Exp>
#define N_EMBED_DOUBLE(Mantissa, Exp)        neam::embed::double_floating_point<Mantissa, Exp>

  } // namespace embed
} // namespace neam

#endif /*__N_231085794123560898_1536835867__TPL_FLOAT_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


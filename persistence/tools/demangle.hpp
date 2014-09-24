//
// file : demangle.hpp
// in : file:///home/tim/projects/yaggler/yaggler/tools/demangle.hpp
//
// created by : Timothée Feuillet on linux-coincoin.tim
// date: 19/03/2014 14:56:08
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

#ifndef __N_11013861961332692490_320117215__DEMANGLE_HPP__
# define __N_11013861961332692490_320117215__DEMANGLE_HPP__

#ifndef __has_feature
#define __has_feature(x) false
#endif

#if defined __GXX_RTTI || __has_feature(cxx_rtti)
#include <typeinfo>
#endif

// GCC only...
#ifdef __GNUC__
#include <cxxabi.h>
#include <stdlib.h>

namespace neam
{
  static inline std::string demangle(const std::string &symbol)
  {
    int status = 0;
    char *realname = nullptr;

    realname = abi::__cxa_demangle(symbol.data(), 0, 0, &status);
    std::string ret;
    if (status)
      ret = symbol;
    else
    {
      ret = realname;
      free(realname);
    }
    return ret;
  }
} // neam
#else // !__GNUC__
namespace neam
{
  static inline std::string demangle(const std::string &symbol)
  {
    return symbol;
  }
} // neam
#endif

namespace neam
{
  template<typename Type>
  static inline std::string demangle()
  {
#if defined __GXX_RTTI || __has_feature(cxx_rtti)
    return demangle(typeid(Type).name());
#else
    return "[unknow symbol: rtti disabled]";
#endif
  }
} // neam

#endif /*__N_11013861961332692490_320117215__DEMANGLE_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


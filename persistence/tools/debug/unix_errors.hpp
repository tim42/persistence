//
// file : unix_errors.hpp
// in : file:///home/tim/projects/nsched/nsched/tools/debug/unix_errors.hpp
//
// created by : Timothée Feuillet on linux.site
// date: 04/08/2014 14:24:51
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

#ifndef __N_14838165221451961813_480336325__UNIX_ERRORS_HPP__
# define __N_14838165221451961813_480336325__UNIX_ERRORS_HPP__

#include <cerrno>
#include <cstring>

#include <string>
#include <sstream>

namespace neam
{
  namespace debug
  {
    namespace errors
    {
      // errno / return codes (direct linux syscall return: -errno)
      template<typename T>
      struct unix_errors
      {
        static bool is_error(long code)
        {
          return code < 0;
        }

        static bool exists(long code)
        {
          return code <= 0;
        }

        static std::string get_code_name(long code)
        {
          std::ostringstream os;
          if (code == -1)
            os << "errno: " << errno;
          else if (code < -1)
            os << "code: " << -code;
          else if (!code)
            os << "success";
          return os.str();
        }

        static std::string get_description(long code)
        {
          if (code == -1)
            return strerror(errno);
          else if (code < -1)
            return strerror(-code);
          else
            return "success";
        }
      };
    } // namespace errors
  } // namespace debug
} // namespace neam

#endif /*__N_14838165221451961813_480336325__UNIX_ERRORS_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


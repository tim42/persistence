//
// file : exception.hpp (2)
// in : file:///home/tim/projects/nsched/nsched/tools/exception.hpp
//
// created by : Timothée Feuillet on linux.site
// date: 03/08/2014 16:20:59
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

#ifndef __N_1232091940699248471_1200439019__EXCEPTION_HPP__2___
# define __N_1232091940699248471_1200439019__EXCEPTION_HPP__2___

#include <exception>
#include <string>

#include <tools/macro.hpp>
#include <tools/demangle.hpp>

namespace neam
{
  // the base neam exception class
  class exception : public std::exception
  {
    private:
      std::string msg;

    public:
      explicit exception(const std::string &what_arg) noexcept : msg(what_arg) {}
      explicit exception(std::string &&what_arg) noexcept : msg(std::move(what_arg)) {}
      virtual ~exception() {}

      virtual const char *what() const noexcept
      {
        return msg.data();
      }
  };

  // an CRTP exception class that add the type name at the start of the string
  template<typename ExceptionType>
  class typed_exception : public exception
  {
    public:
      typed_exception(const std::string &s) noexcept : exception(neam::demangle<ExceptionType>() + ": " + s) {}
      typed_exception(std::string && s) noexcept : exception(neam::demangle<ExceptionType>() + ": " + std::move(s)) {}

      virtual ~typed_exception() {}
  };

// log the line and the file of an exception
// string should be a simple quoted string
// (usage exemple: N_THROW(neam::exception, "this is the exception string !");
#define N_THROW(exception_class, string)        throw exception_class(__FILE__ ": " N_EXP_STRINGIFY(__LINE__) ": " string)
// for std::strings
#define N_THROW_STRING(exception_class, str) throw exception_class(std::string(__FILE__ ": " N_EXP_STRINGIFY(__LINE__) ": ") + str)

//
// some default catch macros
//
#define N_PRINT_EXCEPTION(e)            std::cerr << " ** " __FILE__ ": " N_EXP_STRINGIFY(__LINE__) ": catched exception  '" << e.what() << "'" << std::endl
#define N_PRINT_UNKNOW_EXCEPTION        std::cerr << " ** " __FILE__ ": " N_EXP_STRINGIFY(__LINE__) ": catched unknow exception..." << std::endl
#define N_CATCH_ACTION(x)               catch(std::exception &e) { N_PRINT_EXCEPTION(e); x; } catch(...) { N_PRINT_UNKNOW_EXCEPTION; x; }
#define N_CATCH                         N_CATCH_ACTION( )

// a verbose abort
#define N_ABORT                         do {std::cerr << " ** " __FILE__ ": " N_EXP_STRINGIFY(__LINE__) ": [aborting program]\n" << std::endl; std::cerr.flush(); abort();} while(0)

// fatal exception (call abort)
#define N_FATAL_CATCH                   N_CATCH_ACTION(N_ABORT)
#define N_FATAL_CATCH_ACTION(x)         N_CATCH_ACTION({x;N_ABORT;})

} // namespace neam

#endif /*__N_1232091940699248471_1200439019__EXCEPTION_HPP__2___*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


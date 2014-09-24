//
// file : assert.hpp
// in : file:///home/tim/projects/nsched/nsched/tools/debug/assert.hpp
//
// created by : Timothée Feuillet on linux.site
// date: 03/08/2014 16:18:37
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

#ifndef __N_1744658631925022054_1134258981__ASSERT_HPP__
# define __N_1744658631925022054_1134258981__ASSERT_HPP__

#include <tools/exception.hpp>

#include <string>

#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>

#include <stdexcept>

//
// (yet another) nice assert/function check tool
// NOTE: you could define N_DISABLE_DEBUG to completly disable debugging.
//       you could define N_DISABLE_ASSERTS to disable asserts.
//       you could define N_DISABLE_FNC_CHECKS to disable function checks.
//

namespace neam
{
  namespace debug
  {
    // set this to true to active debug
    namespace setup
    {
      bool debug = false;
      size_t column_width = 200;
    } // namespace setup

    template<template<typename ErrorType> class ErrorClass>
    class on_error
    {
      private:
        class assert{};
        class function_check {};

      public:
        template<typename ReturnType>
        static ReturnType &&_throw_exception(const std::string &func_call, ReturnType &&ret)
        {
          const bool is_error = ErrorClass<ReturnType>::is_error(ret);
          if (setup::debug || is_error)
          {
            std::string message = get_message(func_call, ret);
            std::cerr << ((is_error) ? " ** " : " -- ") << message << std::endl;

            if (is_error)
              throw typed_exception<ErrorClass<ReturnType>>(message);
          }
          return std::move(ret);
        }

        // return true in case of error
        template<typename ReturnType>
        static bool _log_and_check(const std::string &func_call, ReturnType &&ret)
        {
          const bool is_error = ErrorClass<ReturnType>::is_error(ret);
          if (setup::debug || is_error)
          {
            std::string message = get_message(func_call, ret);
            std::cerr << ((is_error) ? " ** " : " -- ") << message << std::endl;

            if (is_error)
              return false;
            return true;
          }
          return false;
        }

        // simply log the error and return the value 'as-is'
        template<typename ReturnType>
        static ReturnType &&_log_and_return(const std::string &func_call, ReturnType &&ret)
        {
          const bool is_error = ErrorClass<ReturnType>::is_error(ret);
          if (setup::debug || is_error)
          {
            std::string message = get_message(func_call, ret);
            std::cerr << ((is_error) ? " ** " : " -- ") << message << std::endl;
          }
          return std::move(ret);
        }

        static void _assert(const std::string &test, bool status, const std::string message)
        {
          if (setup::debug || !status)
          {
            std::cerr << ((!status) ? " ** " : " -- ") << std::left << std::setw(setup::column_width) << std::setfill('.') << (neam::demangle<assert>() + ": " + test + " ") << (!status ? " [FAILED]: " + message : " [SUCCESS]") << std::endl;
            if (!status)
              throw typed_exception<on_error>(test + ": assert failed: " + message);
          }
        }

        static bool _assert_and_return(const std::string &test, bool status, const std::string message)
        {
          if (setup::debug || !status)
            std::cerr << ((!status) ? " ** " : " -- ") << std::left << std::setw(setup::column_width) << std::setfill('.') << (neam::demangle<assert>() + ": " + test + " ") << (!status ? " [FAILED]: " + message : " [SUCCESS]") << std::endl;
          return !status;
        }

        template<typename ReturnType>
        static ReturnType &&_dummy(ReturnType &&ret)
        {
          return std::move(ret);
        }

#if not defined N_DISABLE_ASSERTS and not defined N_DISABLE_DEBUG
#define assert(test, message)                   _assert(__FILE__ ": " N_EXP_STRINGIFY(__LINE__) ": " #test, test, message)
#define assert_exp(test, message)               assert(test, message)
#define assert_and_return(test, message)        _assert_and_return(__FILE__ ": " N_EXP_STRINGIFY(__LINE__) ": " #test, test, message)
#define assert_and_return_exp(test, message)    assert_and_return(test, message)
#else
#define assert(test, message)                   _dummy(((test), true))
#define assert_exp(test, message)               assert(test, message)
#define assert_and_return(test, message)        _dummy(((test), true))
#define assert_and_return_exp(test, message)    assert_and_return(test, message)
#endif

#if not defined N_DISABLE_FNC_CHECKS and not defined N_DISABLE_DEBUG
# define throw_exception(fnc)                   _throw_exception(__FILE__ ": " N_EXP_STRINGIFY(__LINE__) ": " #fnc, fnc)
# define throw_exception_exp(fnc)               throw_exception(fnc)
# define log_and_check(fnc)                     _log_and_check(__FILE__ ": " N_EXP_STRINGIFY(__LINE__) ": " #fnc, fnc)
# define log_and_check_exp(fnc)                 log_and_check(fnc)
# define log_and_return(fnc)                    _log_and_return(__FILE__ ": " N_EXP_STRINGIFY(__LINE__) ": " #fnc, fnc)
# define log_and_return_exp(fnc)                log_and_return(fnc)
#else
# define throw_exception(fnc)                   _dummy(fnc)
# define throw_exception_exp(fnc)               _dummy(fnc)
# define log_and_return(fnc)                    _dummy(fnc)
# define log_and_return_exp(fnc)                _dummy(fnc)
# define log_and_check(fnc)                     _dummy(fnc)
# define log_and_check_exp(fnc)                 _dummy(fnc)
#endif

      private:
        template<typename ReturnType>
        static std::string get_message(const std::string &func_call, const ReturnType &code)
        {
          std::ostringstream is;

          if (ErrorClass<ReturnType>::exists(code))
          {
            const std::string code_name = ErrorClass<ReturnType>::get_code_name(code);
            const std::string description = ErrorClass<ReturnType>::get_description(code);

            is << std::left << std::setw(setup::column_width) << std::setfill('.') << (neam::demangle<function_check>() + ": " +  func_call + " ") << " [" << code_name << "]: " << description;
          }
          else
            is << std::left << std::setw(setup::column_width) << std::setfill('.') << (neam::demangle<function_check>() + ": " + func_call + " ") << " [code: " << code << "]: [UNKNOW ERROR CODE]";
          return is.str();
        }
    };
  } // namespace debug
} // namespace neam

#endif /*__N_1744658631925022054_1134258981__ASSERT_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


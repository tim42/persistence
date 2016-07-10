//
// file : stl.hpp
// in : file:///home/tim/projects/persistence/persistence/stl.hpp
//
// created by : Timothée Feuillet
// date: Sun Jul 10 2016 13:44:08 GMT+0200 (CEST)
//
//
// Copyright (c) 2016 Timothée Feuillet
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

#ifndef __N_2417526974258096055_1532218477_STL_HPP__
#define __N_2417526974258096055_1532218477_STL_HPP__


// // NOTE: All the STL container implementations are backend independent
// // NOTE: You shouldn't include this header directly, as it will include a
// //       lot of STL header. Only include that file if you use MOST of the STL
// //       containers !!

// Sequence containers:
#include "stl/vector.hpp"
#include "stl/deque.hpp"
#include "stl/array.hpp"
#include "stl/list.hpp"
#include "stl/forward_list.hpp"

// string
#include "stl/string.hpp"

// assoc containers:
#include "stl/map.hpp"
#include "stl/unordered_map.hpp"
#include "stl/set.hpp"
#include "stl/unordered_set.hpp"
// multiset (?)
// multimap (?)

namespace neam
{
  namespace cr
  {
    
  } // namespace cr
} // namespace neam

#endif // __N_2417526974258096055_1532218477_STL_HPP__


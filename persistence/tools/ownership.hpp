//
// file : ownership.hpp
// in : file:///home/tim/projects/yaggler/yaggler/tools/ownership.hpp
//
// created by : Timothée Feuillet on linux-coincoin.tim
// date: 08/01/2014 21:08:48
//
//
// Copyright (C) 2013-2014 Timothée Feuillet
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


#ifndef __N_2119167279615582259_1717942984__OWNERSHIP_HPP__
# define __N_2119167279615582259_1717942984__OWNERSHIP_HPP__

// ownership is a dumb resource management pattern: it's like a reference counting, but without the reference.
// There is an owner of the resource that is responsible of its destruction (or whatever)
// and there are 'links' objects: instances that share the pointer or the id of the resource,
// but won't destroy it at the end of their life time.
// Please note that not being the owner doesn't prevent from modifying the resource, but, mainly, from destructing it.
// This management pattern involve a bit more reflection than the reference counting one, but is lightweight, thread safe (nothing is shared)
// and gives absolutely no warranty: its the duty of the user to deal with a simple scoped memory management.
// But when used well, this is fast !!! (duplication is explicitly asked by the user)
//
// some common member functions are:
//  - give_up_ownership()       <- set the ownership flag to false
//  - assume_ownership()        <- set the ownership flag to true
//  - stole_ownership(obj &)    <- behave exactly the same as when constructed with the flag 'stole_ownership'
//
// and a number of constructor with those flags:

namespace neam
{
  // The object will assume that he is the only owner of the resource (set the 'owner' flag to true)
  static class assume_ownership_t {} assume_ownership __attribute__((unused));

  // Stole the ownership flag from the other object (the other will become a 'link').
  // If the object to which we steal the owner flag wasn't the owner, the ownership thief will **NOT** assume ownership.
  static class stole_ownership_t {} stole_ownership __attribute__((unused));

  // force the creation of a copy of the resource
  static class force_duplicate_t {} force_duplicate __attribute__((unused));
} // namespace neam

#endif /*__N_2119167279615582259_1717942984__OWNERSHIP_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


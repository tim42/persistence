//
// file : spinlock.hpp
// in : file:///home/tim/projects/yaggler/yaggler/threads/spinlock.hpp
//
// created by : Timothée Feuillet on linux-coincoin.tim
// date: 19/10/2013 05:02:34
//

#ifndef __N_343181348955182040_1435630634__SPINLOCK_HPP__
# define __N_343181348955182040_1435630634__SPINLOCK_HPP__

#include <atomic>

namespace neam
{
  // I do like classes without constructors :D
  //
  //
  // I also do like comment unrelated with the class functionnality :)
  //
  // Also, compatible with std::lock_guard (--> '<neam::spinlock>').
  class spinlock
  {
    public:
      inline void lock()
      {
        while (lock_flag.test_and_set(std::memory_order_acquire));
      }

      bool try_lock()
      {
        return !lock_flag.test_and_set(std::memory_order_acquire);
      }

      void unlock()
      {
        lock_flag.clear(std::memory_order_release);
      }

    private:
      std::atomic_flag lock_flag = ATOMIC_FLAG_INIT;
  };
} // namespace neam

#endif /*__N_343181348955182040_1435630634__SPINLOCK_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 


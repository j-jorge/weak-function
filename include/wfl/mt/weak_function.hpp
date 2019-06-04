#pragma once

#include "wfl/detail/thread_safe_function_allocator.hpp"
#include "wfl/detail/weak_function.hpp"

namespace wfl
{
  namespace mt
  {
    template< typename F >
    using weak_function =
      wfl::detail::weak_function
      <
        F,
        wfl::detail::thread_safe_function_allocator
      >;
  }
}

extern template class wfl::detail::weak_function
<
  void(),
  wfl::detail::thread_safe_function_allocator
>;

#pragma once

#include "wfl/detail/thread_safe_function_allocator.hpp"
#include "wfl/detail/shared_function.hpp"

namespace wfl
{
  namespace mt
  {
    template< typename F >
    using shared_function =
      wfl::detail::shared_function
      <
        F,
        wfl::detail::thread_safe_function_allocator
      >;
  }
}

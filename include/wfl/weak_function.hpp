#pragma once

#include "wfl/detail/function_allocator.hpp"
#include "wfl/detail/weak_function.hpp"

namespace wfl
{
  template< typename F >
  using weak_function =
    detail::weak_function< F, detail::thread_local_function_allocator >;
}

extern template class wfl::detail::weak_function
<
  void(),
  wfl::detail::thread_local_function_allocator
>;

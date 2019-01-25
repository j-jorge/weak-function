#pragma once

#include "wfl/detail/function_allocator.hpp"
#include "wfl/detail/shared_function.hpp"

namespace wfl
{
  template< typename F >
  using shared_function =
    detail::shared_function< F, detail::thread_local_function_allocator >;
}

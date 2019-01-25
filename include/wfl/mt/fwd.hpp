#pragma once

namespace wfl
{
  namespace detail
  {
    template< typename F, typename FunctionAllocator >
    class weak_function;
    
    template< typename F, typename FunctionAllocator >
    class shared_function;

    class thread_safe_function_allocator;
  }

  namespace mt
  {
    template< typename F >
    using weak_function =
      wfl::detail::weak_function
      <
        F,
        wfl::detail::thread_safe_function_allocator
      >;

    template< typename F >
    using shared_function =
      wfl::detail::shared_function
      <
        F,
        wfl::detail::thread_safe_function_allocator
      >;
  }
}

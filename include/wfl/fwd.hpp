#pragma once

namespace wfl
{
  namespace detail
  {
    template< typename F, typename FunctionAllocator >
    class weak_function;
    
    template< typename F, typename FunctionAllocator >
    class shared_function;

    class thread_local_function_allocator;
  }
  
  template< typename F >
  using weak_function =
    detail::weak_function< F, detail::thread_local_function_allocator >;

  template< typename F >
  using shared_function =
    detail::shared_function< F, detail::thread_local_function_allocator >;
}

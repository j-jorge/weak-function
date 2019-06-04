#include "wfl/weak_function.hpp"
#include "wfl/mt/weak_function.hpp"

template class wfl::detail::weak_function
<
  void(),
  wfl::detail::thread_local_function_allocator
>;

template class wfl::detail::weak_function
<
  void(),
  wfl::detail::thread_safe_function_allocator
>;

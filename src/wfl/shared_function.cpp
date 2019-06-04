#include "wfl/shared_function.hpp"
#include "wfl/mt/shared_function.hpp"

template class wfl::detail::shared_function
<
  void(),
  wfl::detail::thread_local_function_allocator
>;

template class wfl::detail::shared_function
<
  void(),
  wfl::detail::thread_safe_function_allocator
>;

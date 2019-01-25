#include "wfl/detail/function_allocator.hpp"

wfl::detail::function_allocator&
wfl::detail::thread_local_function_allocator::instance()
{
  thread_local function_allocator result;
  return result;
}

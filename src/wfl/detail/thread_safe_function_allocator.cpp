#include "wfl/detail/thread_safe_function_allocator.hpp"

wfl::detail::mt_function_allocator
wfl::detail::thread_safe_function_allocator::s_instance;

wfl::detail::mt_function_allocator&
wfl::detail::thread_safe_function_allocator::instance()
{
  return s_instance;
}

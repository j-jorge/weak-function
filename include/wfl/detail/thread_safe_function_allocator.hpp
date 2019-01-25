#pragma once

#include "wfl/detail/function_allocator.hpp"

#include <mutex>
#include <thread>

namespace wfl
{
  namespace detail
  {
    class mt_function_allocator
    {
    public:
      typedef detail::function_allocator::allocation_handle allocation_handle;
  
    public:
      template< typename... Args >
      allocation_handle allocate( std::function< void( Args... ) > f )
      {
        const std::lock_guard< std::recursive_mutex > lock( m_mutex );
        return m_allocator.allocate( std::move( f ) );
      }

      template< typename... Args >
      void call( const allocation_handle& handle, Args&&... args )
      {
        const std::lock_guard< std::recursive_mutex > lock( m_mutex );
        return m_allocator.call( handle, std::forward< Args >( args )... );
      }
  
      template< typename... Args >
      void safe_call( const allocation_handle& handle, Args&&... args )
      {
        const std::lock_guard< std::recursive_mutex > lock( m_mutex );
        return m_allocator.safe_call( handle, std::forward< Args >( args )... );
      }

      template< typename F >
      void release_one( const allocation_handle& handle )
      {
        const std::lock_guard< std::recursive_mutex > lock( m_mutex );
        m_allocator.release_one< F >( handle );
      }

      void add_one( const allocation_handle& handle )
      {
        const std::lock_guard< std::recursive_mutex > lock( m_mutex );
        m_allocator.add_one( handle );
      }
  
    private:
      detail::function_allocator m_allocator;
      std::recursive_mutex m_mutex;
    };

    struct thread_safe_function_allocator
    {
      typedef mt_function_allocator::allocation_handle allocation_handle;
      static mt_function_allocator& instance();

    private:
      static mt_function_allocator s_instance;
    };
  }
}

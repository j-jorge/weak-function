#pragma once

#include "wfl/detail/function_allocator_storage.hpp"
#include "wfl/detail/debug.hpp"

namespace wfl
{
  namespace detail
  {
    class function_allocator
    {
    public:
      typedef
      detail::function_allocator_storage::allocation_handle allocation_handle;
  
    public:
      template< typename... Args >
      allocation_handle allocate( std::function< void( Args... ) > f )
      {
        const typename function_allocator_storage::allocation_result result
          ( m_storage.allocate() );
        
        new ( result.storage )
          std::function< void( Args... ) >( std::move( f ) );
    
        return result.handle;
      }

      template< typename... Args >
      void call( const allocation_handle& handle, Args&&... args ) const
      {
        const typename function_allocator_storage::function_storage* const
          function
          ( m_storage.grab( handle ) );

        wfl_debug_assert( function != nullptr );
        
        typedef std::function< void( Args... ) > function_type;
    
        const function_type& typed_function
          ( *reinterpret_cast< const function_type* >( function ) );

        return typed_function( std::forward< Args >( args )... );
      }
  
      template< typename... Args >
      void safe_call( const allocation_handle& handle, Args&&... args ) const
      {
        const typename function_allocator_storage::function_storage* const
          function
          ( m_storage.grab( handle ) );

        if ( function == nullptr )
          return;
        
        typedef std::function< void( Args... ) > function_type;
    
        const function_type& typed_function
          ( *reinterpret_cast< const function_type* >( function ) );

        return typed_function( std::forward< Args >( args )... );
      }

      template< typename F >
      void release_one( const allocation_handle& handle )
      {
        const typename function_allocator_storage::function_storage* const
          function
          ( m_storage.release_one( handle ) );

        if ( function == nullptr )
          return;

        reinterpret_cast< const F* >( function )->~F();
      }

      void add_one( const allocation_handle& handle )
      {
        m_storage.add_one( handle );
      }
  
    private:
      detail::function_allocator_storage m_storage;
    };

    struct thread_local_function_allocator
    {
      typedef function_allocator::allocation_handle allocation_handle;
      static function_allocator& instance();
    };
  }
}

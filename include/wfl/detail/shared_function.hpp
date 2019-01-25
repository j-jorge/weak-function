#pragma once

#include <functional>

namespace wfl
{
  namespace detail
  {
    template< typename F, typename FunctionAllocator >
    class weak_function;

    template< typename F, typename FunctionAllocator >
    class shared_function;

    template< typename FunctionAllocator, typename... Args >
    class shared_function< void( Args... ), FunctionAllocator >
    {
      friend class weak_function< void( Args... ), FunctionAllocator >;

    private:
      typedef FunctionAllocator function_allocator;
      typedef std::function< void( Args... ) > function_type;
      typedef shared_function< void( Args... ), FunctionAllocator > self_type;
  
    public:
      shared_function() = default;

      shared_function( const self_type& that )
        : m_handle( that.m_handle )
      {
        function_allocator::instance().add_one( m_handle );
      }
  
      explicit shared_function( std::function< void( Args... ) > f )
        : m_handle
          ( function_allocator::instance().allocate( std::move( f ) ) )
      {

      }

      ~shared_function()
      {
        function_allocator::instance().template release_one< function_type >
          ( m_handle );
      }

      void operator()( Args&&... args ) const
      {
        function_allocator::instance().call
          ( m_handle, std::forward< Args >( args )... );
      }

      self_type& operator=( const self_type& that )
      {
        if ( this == &that )
          return *this;

        function_allocator::instance().template release_one< function_type >
          ( m_handle );
      
        m_handle = that.m_handle;
      
        function_allocator::instance().add_one( m_handle );
    
        return *this;
      }
  
    private:
      typename function_allocator::allocation_handle m_handle;
    };
  }
}

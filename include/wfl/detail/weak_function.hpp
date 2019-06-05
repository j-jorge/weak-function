#pragma once

#include "wfl/detail/shared_function.hpp"

namespace wfl
{
  namespace detail
  {
    template< typename F, typename FunctionAllocator >
    class weak_function;

    template< typename FunctionAllocator, typename... Args >
    class weak_function< void( Args... ), FunctionAllocator >
    {
    private:
      typedef FunctionAllocator function_allocator;
      typedef std::function< void( Args... ) > function_type;
      typedef
      shared_function< void( Args... ), FunctionAllocator > matching_shared;
      typedef weak_function< void( Args... ), FunctionAllocator > self_type;
      
    public:
      weak_function() = default;
      weak_function( const self_type& ) = default;
      weak_function( self_type&& ) = default;
  
      weak_function( const matching_shared& f )
        : m_handle( f.m_handle )
      {

      }

      self_type& operator=( const matching_shared& f )
      {
        m_handle = f.m_handle;
        return *this;
      }
  
      self_type& operator=( matching_shared&& f )
      {
        m_handle = f.m_handle;
        return *this;
      }
  
      self_type& operator=( const self_type& ) = default;
      self_type& operator=( self_type&& f ) = default;

      void operator()( Args... args ) const
      {
        function_allocator::instance().safe_call
          ( m_handle, std::forward< Args >( args )... );
      }
  
    private:
      typename function_allocator::allocation_handle m_handle;
    };
  }
}


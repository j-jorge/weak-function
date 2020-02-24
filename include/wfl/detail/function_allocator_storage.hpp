#pragma once

#include <deque>
#include <functional>
#include <type_traits>
#include <vector>

namespace wfl
{
  namespace detail
  {
    class function_allocator_storage
    {
    public:
      static constexpr int not_a_version = 0;
  
      struct allocation_handle
      {
        int version = not_a_version;
        std::size_t id;
      };
  
      typedef
      std::aligned_storage
      <
        sizeof( std::function< void() > ),
        alignof( std::function< void() > )
      >::type
      function_storage;

      struct block
      {
        function_storage storage;
        int version = not_a_version;
        char ref_count = 0;
      };

      struct allocation_result
      {
        allocation_handle handle;
        function_storage* storage;
      };
        
    public:
      allocation_result allocate();
      const function_storage* grab( const allocation_handle& handle ) const;
      function_storage* release_one( const allocation_handle& handle );
      void add_one( const allocation_handle& handle );
  
    private:
      std::deque< block > m_blocks;
      std::vector< std::size_t > m_available;
    };
  }
}

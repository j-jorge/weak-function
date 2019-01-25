#include <wfl/detail/function_allocator_storage.hpp>

wfl::detail::function_allocator_storage::allocation_result
wfl::detail::function_allocator_storage::allocate()
{
  std::size_t id;
    
  if ( m_available.empty() )
    {
      id = m_blocks.size();
      m_blocks.emplace_back();
    }
  else
    {
      id = m_available.back();
      m_available.pop_back();
    }

  block& block( m_blocks[ id ] );
  
  ++block.version;
  block.ref_count = 1;

  allocation_result result;
  result.handle.version = block.version;
  result.handle.id = id;
  result.storage = &block.storage;

  return result;
}

const wfl::detail::function_allocator_storage::function_storage*
wfl::detail::function_allocator_storage::grab
( const allocation_handle& handle ) const
{
  const block& block( m_blocks[ handle.id ] );

  if ( ( handle.version != block.version )
       || ( handle.version == not_a_version )
       || ( block.ref_count == 0 ) )
    return nullptr;

  return &block.storage;
}
      
wfl::detail::function_allocator_storage::function_storage*
wfl::detail::function_allocator_storage::release_one
( const allocation_handle& handle )
{
  if ( handle.version == not_a_version )
    return nullptr;
    
  std::size_t id( handle.id );
  block& block( m_blocks[ id ] );

  --block.ref_count;

  if ( block.ref_count == 0 )
    {
      m_available.emplace_back( id );
      return &block.storage;
    }

  return nullptr;
}

void wfl::detail::function_allocator_storage::add_one
( const allocation_handle& handle )
{
  if ( handle.version == not_a_version )
    return;
    
  ++m_blocks[ handle.id ].ref_count;
}

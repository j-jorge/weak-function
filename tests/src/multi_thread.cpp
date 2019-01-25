// Forward declaration is included as a test, to ensure that it declares the
// correct types
#include "wfl/mt/fwd.hpp"

#include "wfl/mt/shared_function.hpp"
#include "wfl/mt/weak_function.hpp"

#include <atomic>
#include <memory>
#include <mutex>
#include <thread>

#include <gtest/gtest.h>

TEST( wfl_weak_function, weak_functions_from_multiple_threads )
{
  std::vector< std::unique_ptr< wfl::mt::shared_function< void() > > > shared;
  std::vector< wfl::mt::weak_function< void() > > weak;
  std::mutex mutex_on_shared;
  std::mutex mutex_on_weak;
  std::atomic< int > call_count( 0 );

  const auto increment
    ( [ & ]() -> void
      {
        ++call_count;
      } );

  constexpr std::size_t shared_limit( 1000 );
  constexpr std::size_t weak_copy_count( 2 );
  constexpr std::size_t weak_limit( weak_copy_count * shared_limit );
  
  std::thread creator
    ( [ & ]() -> void
      {
        std::size_t new_size( 0 );
        
        while ( new_size < shared_limit )
          {
            {
              const std::lock_guard< std::mutex > lock_shared
                ( mutex_on_shared );
              const std::lock_guard< std::mutex > lock_weak
                ( mutex_on_weak );
              const int count( 200 );

              new_size = shared.size() + count;
              shared.reserve( new_size );
              weak.reserve( weak.size() + weak_copy_count * count );

              for ( int i( 0 ); i != count; ++i )
                {
                  shared.emplace_back
                    ( new wfl::mt::shared_function< void() >( increment ) );
                  weak.emplace_back( *shared.back() );
                  weak.emplace_back( *shared.back() );
                }
            }
            
            std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
          }
      } );
  
  std::thread caller
    ( [ & ]() -> void
      {
        const std::chrono::steady_clock::time_point start
          ( std::chrono::steady_clock::now() );
        std::size_t weak_count( 0 );
        
        while ( weak_count < weak_limit )
          {
            std::vector< wfl::mt::weak_function< void() > > functions;
            
            {
              const std::lock_guard< std::mutex > lock( mutex_on_weak );
              functions = weak;
            }

            weak_count = functions.size();
            
            for ( const auto& f : functions )
              f();

            std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
          }
      } );
  
  
  std::thread destroyer
    ( [ & ]() -> void
      {
        bool stop( false );

        while ( !stop )
          {
            std::size_t count;

            {
              const std::lock_guard< std::mutex > lock( mutex_on_shared );
              count = shared.size();
            }

            stop = ( count == shared_limit );

            for ( int j( 0 ); j != 2; ++j )
              {
                for ( int i( count - 1 - j ); i >= 0; i -= 2 )
                  {
                    const std::lock_guard< std::mutex > lock
                      ( mutex_on_shared );

                    if ( !shared[ i ] )
                      break;
                
                    stop = false;
                    shared[ i ].reset();
                  }
            
                std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
              }
          }
      } );
  
  creator.join();
  caller.join();
  destroyer.join();

  EXPECT_NE( 0, call_count.load() );
}


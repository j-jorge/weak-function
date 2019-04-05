// Forward declaration is included as a test, to ensure that it declares the
// correct types
#include "wfl/fwd.hpp"

#include "wfl/weak_function.hpp"
#include "wfl/shared_function.hpp"

#include <memory>
#include <thread>

#include <gtest/gtest.h>

namespace wfl
{
  namespace test
  {
    static int weak_function_call_count( 0 );

    static void count_weak_function_call()
    {
      ++weak_function_call_count;
    }
  }
}

TEST( wfl_weak_function, call_empty_weak_function )
{
  const wfl::weak_function< void() > weak{};
  weak();
}

TEST( wfl_weak_function, shared_goes_out_of_scope )
{
  wfl::test::weak_function_call_count = 0;
  
  wfl::weak_function< void() > weak;

  {
    const wfl::shared_function< void() > shared
      ( &wfl::test::count_weak_function_call );
    weak = shared;

    EXPECT_EQ( 0, wfl::test::weak_function_call_count );
    
    weak();
    EXPECT_EQ( 1, wfl::test::weak_function_call_count );
  }
    
  weak();
  EXPECT_EQ( 1, wfl::test::weak_function_call_count );

  {
    const wfl::shared_function< void() > shared
      ( &wfl::test::count_weak_function_call );
    weak = shared;

    EXPECT_EQ( 1, wfl::test::weak_function_call_count );
    
    weak();
    EXPECT_EQ( 2, wfl::test::weak_function_call_count );
  }
  
  weak();
  EXPECT_EQ( 2, wfl::test::weak_function_call_count );
}

TEST( wfl_weak_function, std_function_from_weak_function )
{
  int call_count( 0 );
  
  const wfl::shared_function< void() > shared
    ( [ & ]() -> void
      {
        ++call_count;
      } );

  const std::function< void() > function
    { wfl::weak_function< void() >( shared ) };
  
  EXPECT_EQ( 0, call_count );
    
  function();
  EXPECT_EQ( 1, call_count );
}

TEST( wfl_weak_function, reset )
{
  int call_count( 0 );
  
  wfl::shared_function< void() > shared
    ( [ & ]() -> void
      {
        ++call_count;
      } );

  const wfl::weak_function< void() > weak( shared );
  
  EXPECT_EQ( 0, call_count );
    
  weak();
  EXPECT_EQ( 1, call_count );

  shared.reset();
  weak();
  EXPECT_EQ( 1, call_count );
}

TEST( wfl_weak_function, reset_replace )
{
  int call_count_1( 0 );
  
  wfl::shared_function< void() > shared
    ( [ & ]() -> void
      {
        ++call_count_1;
      } );

  const wfl::weak_function< void() > weak( shared );
  
  EXPECT_EQ( 0, call_count_1 );
    
  weak();
  EXPECT_EQ( 1, call_count_1 );

  int call_count_2( 0 );
  shared.reset
    ( [ & ]() -> void
      {
        ++call_count_2;
      } );
  
  weak();
  EXPECT_EQ( 1, call_count_1 );
  EXPECT_EQ( 0, call_count_2 );

  shared();
  EXPECT_EQ( 1, call_count_1 );
  EXPECT_EQ( 1, call_count_2 );
}

TEST( wfl_weak_function, weak_function_stays_valid_until_last_shared_is_dead )
{
  int call_count( 0 );
  
  std::unique_ptr< wfl::shared_function< void() > > shared_1
    ( new wfl::shared_function< void() >
      ( [ & ]() -> void
        {
          ++call_count;
        } ) );
  std::unique_ptr< wfl::shared_function< void() > > shared_2
    ( new wfl::shared_function< void() >( *shared_1 ) );
  std::unique_ptr< wfl::shared_function< void() > > shared_3
    ( new wfl::shared_function< void() >( *shared_2 ) );

  const wfl::weak_function< void() > weak( *shared_1 );
  
  EXPECT_EQ( 0, call_count );
    
  weak();
  EXPECT_EQ( 1, call_count );

  shared_1.reset();
  weak();
  EXPECT_EQ( 2, call_count );

  shared_3.reset();
  weak();
  EXPECT_EQ( 3, call_count );

  shared_2.reset();
  weak();
  EXPECT_EQ( 3, call_count );
}

TEST( wfl_weak_function, weak_functions_from_independent_threads )
{
  auto run_test
    ( []() -> void
      {
        std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
        
        int call_count( 0 );
        std::vector< std::unique_ptr< wfl::shared_function< void() > > > shared
          ( 100 );

        for ( auto& p : shared )
          p.reset
            ( new wfl::shared_function< void() >
              ( [ & ]() -> void
                {
                  ++call_count;
                } ) );

        std::vector< wfl::weak_function< void() > > weak( 2 * shared.size() );

        for ( std::size_t i( 0 ); i != shared.size(); ++i )
          {
            weak[ i ] = *shared[ i ];
            weak[ i + shared.size() ] = *shared[ i ];
          }

        for ( auto& w : weak )
          w();

        EXPECT_EQ( 200, call_count );

        for ( std::size_t i( 0 ); i < shared.size(); i += 2 )
          shared[ i ].reset();

        for ( auto& w : weak )
          w();

        EXPECT_EQ( 300, call_count );
      } );

  std::vector< std::thread > threads;

  for ( int i( 0 ); i != 100; ++i )
    threads.emplace_back( run_test );

  for ( std::thread& t : threads )
    t.join();
}

TEST( wfl_weak_function, assign_during_call )
{
  int shared_1_call_count( 0 );
  wfl::shared_function< void() > shared_1
    ( [&]() -> void
      {
        ++shared_1_call_count;
      } );
  
  wfl::weak_function< void() > weak;
  
  int shared_2_call_count( 0 );
  wfl::shared_function< void() > shared_2
    ( [&]() -> void
      {
        weak = shared_1;
        ++shared_2_call_count;
      } );

  weak = shared_2;
  weak();

  EXPECT_EQ( 0, shared_1_call_count );
  EXPECT_EQ( 1, shared_2_call_count );

  weak();

  EXPECT_EQ( 1, shared_1_call_count );
  EXPECT_EQ( 1, shared_2_call_count );
}

TEST( wfl_weak_function, assign_same_during_call )
{
  int shared_call_count( 0 );
  wfl::weak_function< void() > weak;

  wfl::shared_function< void() > shared
    ( [&]() -> void
      {
        ++shared_call_count;
        weak = shared;
      } );

  weak = shared;
  weak();

  EXPECT_EQ( 1, shared_call_count );

  weak();

  EXPECT_EQ( 2, shared_call_count );
}

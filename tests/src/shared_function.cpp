#include "wfl/shared_function.hpp"

#include <gtest/gtest.h>

namespace wfl
{
  namespace test
  {
    static int shared_function_call_count( 0 );

    static void count_shared_function_call()
    {
      ++shared_function_call_count;
    }
  }
}

TEST( wfl_shared_function, shared_from_free_function )
{
  wfl::test::shared_function_call_count = 0;
  
  const wfl::shared_function< void() > shared
    ( &wfl::test::count_shared_function_call );

  EXPECT_EQ( 0, wfl::test::shared_function_call_count );
    
  shared();
  EXPECT_EQ( 1, wfl::test::shared_function_call_count );
}

TEST( wfl_shared_function, shared_from_lambda )
{
  int call_count( 0 );
  
  const wfl::shared_function< void() > shared
    ( [ & ]() -> void
      {
        ++call_count;
      } );

  EXPECT_EQ( 0, call_count );
    
  shared();
  EXPECT_EQ( 1, call_count );
}

TEST( wfl_shared_function, shared_from_function_object )
{
  struct functor
  {
    void operator()()
    {
      ++call_count;
    }
    
    int call_count = 0;
  };

  functor f;
  const wfl::shared_function< void() > shared( std::ref( f ) );

  EXPECT_EQ( 0, f.call_count );
    
  shared();
  EXPECT_EQ( 1, f.call_count );
}

TEST( wfl_shared_function, shared_from_std_function )
{
  int call_count( 0 );

  const std::function< void() > function
    ( [ & ]() -> void
      {
        ++call_count;
      } );
  
  const wfl::shared_function< void() > shared( function );

  EXPECT_EQ( 0, call_count );
    
  shared();
  EXPECT_EQ( 1, call_count );
}

TEST( wfl_shared_function, shared_with_arguments )
{
  std::string result;
  
  const wfl::shared_function< void( std::string, int ) > shared
    ( [ & ]( std::string s, int i ) -> void
      {
        result.clear();
        
        for ( ; i > 0; --i )
          result += s;
      } );

  shared( "ab", 0 );
  EXPECT_EQ( "", result );

  shared( "ab", 1 );
  EXPECT_EQ( "ab", result );
  
  shared( "ab", 2 );
  EXPECT_EQ( "abab", result );

  shared( "ab", 3 );
  EXPECT_EQ( "ababab", result );
}

TEST( wfl_shared_function, std_function_from_shared_function )
{
  int call_count( 0 );
  
  const std::function< void() > function
    ( wfl::shared_function< void() >
      ( [ & ]() -> void
        {
          ++call_count;
        } ) );
  
  EXPECT_EQ( 0, call_count );
    
  function();
  EXPECT_EQ( 1, call_count );
}

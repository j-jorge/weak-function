#include <wfl/mt/weak_function.hpp>
#include <wfl/mt/shared_function.hpp>

#include <iostream>
#include <thread>

// Here's an hypothetical class which sends messages to a target and notifies
// the caller when the message has been sent.
class message_handler
{
public:
  ~message_handler()
  {
    for ( std::thread& t : m_threads )
      if ( t.joinable() )
        t.join();
  }
  
  void send_message
  ( std::string message, wfl::mt::weak_function< void() > on_sent )
  {
    m_threads.emplace_back
      ( [=]() -> void
        {
          std::cout << "sending '" << message << "'\n";
          std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );
          on_sent();
        } );
  }
  
private:
  std::vector< std::thread > m_threads;
};

int main()
{
  message_handler handler;
  
  {
    wfl::mt::shared_function< void() > on_sent
      ( []() -> void
        {
          std::cout << "First message sent\n";
        } );
    handler.send_message( "Hello", on_sent );
  }
  
  wfl::mt::shared_function< void() > on_sent
    ( []() -> void
      {
        std::cout << "Second message sent.\n";
      } );

  handler.send_message( "Hello, world", on_sent );
  std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
  
  return 0;
}

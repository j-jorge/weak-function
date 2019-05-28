Weak Function Library is a C++ 11 library providing a solution to
passing around references to functions that may be destroyed before
their call.

- [Quick Start](#quick-start)
- [Usage Example](#usage-example)
- [Building](#building)
- [Library's Content](#librarys-content)
- [Why not use a signal/slot library?](#why-not-use-a-signalslot-library)

# Quick Start

1. Declare a `wfl::shared_function< Signature >` to hold a reference to
the function that may be destroyed.

2. Declare a `wfl::weak_function< Signature >` on the caller side.

3. Assign the `shared_function` to the `weak_function`.

4. Call the `weak_function` when you need it.

Bonus: Use `wfl::mt::shared_function< Signature >` and
`wfl::mt::weak_function< Signature >` if the call occurs in another
thread.

# Usage Example

```c++
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
```

The output of this program is
```
sending 'Hello'
sending 'Hello, world'
Second message sent.
```

As expected, the first shared function which should have printed
`First message sent` is not called since it has been destroyed before
the actual call. On the caller side no special handling is needed
before the call.

# Building

The CMake scripts are located in the `cmake` directory.

```sh
mkdir build
cd build
cmake ../cmake/
make
```

The following arguments can be passed to CMake:

- `WFL_CMAKE_PACKAGE_ENABLED=ON/OFF` controls whether the CMake
  script allowing to import the library in another project must be
  installed or not. Default is `ON`.
- `WFL_DEBUG=ON/OFF` controls the activation of debugging code
  in the library. It is a developer feature, you probably won't want
  it. Default is `OFF`.
- `WFL_EXAMPLES_ENABLED=ON/OFF` controls the build of the example
  programs. Default is `OFF`.
- `WFL_TESTING_ENABLED=ON/OFF` controls the build of the unit
  tests. Default value is `ON`. You will need
  [Google Test](https://github.com/google/googletest) for this.

# Library's Content

There are two classes in this library: `wfl::shared_function` and
`wfl::weak_function`. The former is like an `std::function`, it stores
a callable. The latter is to `wfl::shared_function` what
`std::weak_ptr` is to `std::shared_ptr`: a handle through which the
validity of the `wfl::shared_function` can be tested. The whole point
of the library is that calling a function via `wfl::weak_function` has
no effect if the corresponding `wfl::shared_function` has been
destroyed.

If your instances of `wfl::weak_function` never leave the thread in
which they are created, then use `wfl::weak_function` and
`wfl::shared_function`. Otherwise, for a thread-safe version, use
`wfl::mt::weak_function` and `wfl::mt::shared_function`.

In the observed instance, store a `wfl::weak_function`:

```c++
#include <wfl/weak_function.hpp>

struct observed
{
  void call_me( wfl::weak_function< void() > callback )
  {
    do_stuff().then( [=]() -> void { callback(); } );
  }
}
```

In the observer, store a `wfl::shared_function`:

```c++
#include <wfl/shared_function.hpp>
#include <iostream>

struct observer
{
  observer()
    : m_callback( []() -> void { std::cout << "called\n"; } )
  { }
    
  void observe( observed& o )
  {
    o.observe( m_callback );
  }
  
private:
  wfl::shared_function< void() > m_callback;
};
```

Then keep going without wondering if the observer dies before the observed.

# Why not use a signal/slot library?

Signals are great when multiple callbacks must be called in batch or
when a callback must be trigered on several occasions. They fail
nonetheless when the callback is attached to a context.

For example, in the `message_handler` class defined above, sending a
message must trigger a callback, and only one. Using signals for this
would require a new signal for each call to `send_message`. Example
with Boost.Signals2:

```c++
boost::signals2::connection message_handler::send_message
( std::string message, std::function< void() > on_sent )
{
  std::shared_ptr< boost::signals2::signal< void() > > signal
    ( std::make_shared< boost::signals2::signal< void() > >() );
    
  const boost::signals2::connection result
    ( signal.connect( std::move( on_sent ) ) );
    
  m_threads.emplace_back
    ( [=]() -> void
      {
        std::cout << "sending '" << message << "'\n";
        std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );
        signal();
      } );
        
  return result;
}
```

This kind of code feels overcomplicated and raises several questions:

1. Why using a signal designed to store several callbacks when only
   one will ever be assigned?
   
2. Do you really need to pay a dynamic allocation for every callback?

3. Boost.Signals2 is thread safe by default. If your app is
single-threaded, why do you have to pay for the synchronization?

Also, handling signal connections is quite error-prone. Here it is the
caller's responsibility to store the resulting connection and to
manually call `disconnect()` on it when the function passed as
`on_sent` is destroyed.

`wfl` aims to solve these issues. It is designed for a one-to-one
relationship, does minimal allocations and provides optional thread
safety.

Also, with `wfl` the storing of the `wfl::shared_function` cannot be
missed out and its disconnection is automatic. There is no way for a
function to be called after being destroyed with this design.


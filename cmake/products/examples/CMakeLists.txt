find_package( Threads REQUIRED )

add_executable(
  message-handler
  ${source_root}/examples/message_handler.cpp
  )

target_link_libraries(
  message-handler
  ${core_library_name}
  Threads::Threads
  )

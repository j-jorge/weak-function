set( core_library_name wfl )
set( core_library_name ${core_library_name} PARENT_SCOPE )

add_unity_build_library(
  TARGET ${core_library_name}
  ROOT ${source_root}/src/wfl/
  FILES
  "shared_function.cpp"
  "weak_function.cpp"
  "detail/function_allocator.cpp"
  "detail/function_allocator_storage.cpp"
  "detail/thread_safe_function_allocator.cpp"
  )
  
target_include_directories(
  ${core_library_name}
  PUBLIC
  $<BUILD_INTERFACE:${source_root}/include>
  )

install(
  DIRECTORY ${source_root}/include/wfl
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

install(
  TARGETS ${core_library_name}
  EXPORT ${core_library_name}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

if( WFL_DEBUG )
  target_compile_definitions( ${core_library_name} PUBLIC WFL_DEBUG )
endif()

set( unit_tests_executable_name ${core_library_name}-tests )

add_unity_build_executable(
  TARGET ${unit_tests_executable_name}
  ROOT "${source_root}/tests/src/"
  FILES
  "multi_thread.cpp"
  "shared_function.cpp"
  "weak_function.cpp"
  )

target_link_libraries(
  ${unit_tests_executable_name}
  ${core_library_name}
  GTest::GTest
  GTest::Main
  )

gtest_discover_tests( ${unit_tests_executable_name} )

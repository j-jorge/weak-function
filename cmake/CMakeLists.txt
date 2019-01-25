cmake_minimum_required( VERSION 3.10 )

project( wfl )

set( source_root "${CMAKE_CURRENT_SOURCE_DIR}/.." )
set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules" )
set( CMAKE_CXX_STANDARD 11 )

include( GNUInstallDirs )
include( unity-build )

option( WFL_TESTING_ENABLED "Build the unit tests." ON )
option( WFL_EXAMPLES_ENABLED "Build the examples." OFF )
option( WFL_CMAKE_PACKAGE_ENABLED "Build the CMake package." ON )
option( WFL_DEBUG "Enable internal debug." OFF )

add_subdirectory( "products/core/" )

if( WFL_TESTING_ENABLED )
  enable_testing()
  find_package( GTest REQUIRED )
  add_subdirectory( "products/tests/" )
endif()

if( WFL_CMAKE_PACKAGE_ENABLED )
  add_subdirectory( "products/package/" )
endif()

if( WFL_EXAMPLES_ENABLED )
  add_subdirectory( "products/examples/" )
endif()

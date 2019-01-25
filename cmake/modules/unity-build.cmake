#[[.rst:
Compilation Units
-----------------

This modules declares a function to help use unity builds for a project.

Unity builds consists into compiling a single C++ source file (a unit)
that includes several source files instead of compiling each of these
files individually, thus reducing the number of compiler invocations
as well as the overall build time.

Functions declared in this module behave with respect to the
ENABLE_COMPILATION_UNITS variable.

.. command:: add_unity_build_executable

  Add a new executable target, similar to one declared with
  ``add_executable``::

    add_unity_build_executable( TARGET target
                                ROOT dir
                                FILES src1…
    )

  The options are:

  ``TARGET target``
    Specifies the name of the target to create.

  ``ROOT dir``
    The path to the root directory where the ``FILES`` are located.

  ``FILES src1…``
    The paths to the files to use to build ``target``.
  
  If ``ENABLE_COMPILATION_UNITS`` is ``OFF`` then this function
  behaves as ``add_executable( target ${dir}${src1}… )``.
#]]

option(
  ENABLE_COMPILATION_UNITS
  "Merge all source files together to reduce the number of times the compiler\
 is invoked."
  ON
  )

set(
  COMPILATION_UNIT_COUNT "" CACHE STRING
  "The number of compilation units to create if they are enabled."
  )

#[[
Generates a single C++ source file which includes the given source files.

The options are:

``UNIT_VARIABLE var``
  The name of the variable receiving the path of the generated file.
  
``OUTPUT_BASENAME basename``
  The base name to give to the unit, allowing to distinguish several
  units in the same directory.
  
``FILES src…``
  The files to include in the generated unit.

The unit file is generated if and only if the provided ``FILES`` have changed.
#]]
function( make_compilation_units )

  cmake_parse_arguments( ARG
    ""
    "OUTPUT_BASENAME;UNIT_VARIABLE"
    "FILES"
    ${ARGN}
    )

  if( COMPILATION_UNIT_COUNT )
    set( unit_count ${COMPILATION_UNIT_COUNT} )
  else()
    set( unit_count 1 )
  endif()
  
  set( temporary_units )
  
  foreach( unit_index RANGE 1 ${unit_count} )
    set(
      temp
      "${PROJECT_BINARY_DIR}/${ARG_OUTPUT_BASENAME}-unit.tmp.${unit_index}"
      )
    
    file( REMOVE "${temp}" )
    set( temporary_units ${temporary_units} ${temp} )
  endforeach()

  set( unit_index 0 )
  
  foreach( f ${ARG_FILES} )
    list( GET temporary_units ${unit_index} temp )
    file( APPEND "${temp}" "#include \"${f}\"\n" )

    math( EXPR unit_index "${unit_index} + 1" )

    if( ${unit_index} EQUAL ${unit_count} )
      set( unit_index 0 )
    endif()
  endforeach()

  set( result )

  foreach( unit_index RANGE 1 ${unit_count} )
    set(
      unit
      "${PROJECT_BINARY_DIR}/${ARG_OUTPUT_BASENAME}-unit-${unit_index}.cpp"
      )
    set( result ${result} ${unit} )

    math( EXPR list_index "${unit_index} - 1" )
    list( GET temporary_units ${list_index} temp )

    execute_process( COMMAND ${CMAKE_COMMAND}
      -E copy_if_different
      "${temp}"
      "${unit}"
      OUTPUT_QUIET
      ERROR_QUIET
      )

    file( REMOVE "${temp}" )
  endforeach()

  set( ${ARG_UNIT_VARIABLE} ${result} PARENT_SCOPE )
endfunction()

#[[
Build a list of strings by prefixing the given strings with the given
prefix.

The options are:

``OUTPUT_VARIABLE var``
  The variable that receives the resulting list.

``PREFIX string``
  The prefix to insert before each entry of ``STRINGS``

``STRINGS string…``
  The string to concatenate with ``PREFIX``.

For example, ``prepend_string( OUTPUT_VARIABLE out PREFIX foo STRINGS
  "bar" "BAR" )`` will fill the variable ``out`` with the values
  ``foobar`` and ``fooBAR``.
#]]
function( prepend_string )

  cmake_parse_arguments( ARG
    ""
    "OUTPUT_VARIABLE;PREFIX"
    "STRINGS"
    ${ARGN}
    )

  set( result "" )
  
  foreach( suffix ${ARG_STRINGS} )
    set(
      result
      "${result}"
      "${ARG_PREFIX}${suffix}"
      )
  endforeach()

  set( ${ARG_OUTPUT_VARIABLE} ${result} PARENT_SCOPE )
endfunction()

# ------------------------------------------------------------------------------
# Public interface. See top level comments for details.
# ------------------------------------------------------------------------------

function( add_unity_build_executable )
  
  cmake_parse_arguments( ARG
    ""
    "TARGET;ROOT"
    "FILES"
    ${ARGN}
    )

  prepend_string(
    OUTPUT_VARIABLE files
    PREFIX ${ARG_ROOT}
    STRINGS ${ARG_FILES}
    )
  
  if( ENABLE_COMPILATION_UNITS )
    make_compilation_units(
      UNIT_VARIABLE unit_file
      FILES ${files}
      OUTPUT_BASENAME ${ARG_TARGET}
      )
    add_executable( ${ARG_TARGET} ${unit_file} )
  else()
    add_executable( ${ARG_TARGET} ${files} )
  endif()
endfunction()

function( add_unity_build_library )
  
  cmake_parse_arguments( ARG
    ""
    "TARGET;ROOT;TYPE"
    "FILES"
    ${ARGN}
    )

  prepend_string(
    OUTPUT_VARIABLE files
    PREFIX ${ARG_ROOT}
    STRINGS ${ARG_FILES}
    )
  
  if( ENABLE_COMPILATION_UNITS )
    make_compilation_units(
      UNIT_VARIABLE unit_file
      FILES ${files}
      OUTPUT_BASENAME ${ARG_TARGET}
      )
    add_library( ${ARG_TARGET} ${ARG_TYPE} ${unit_file} )
  else()
    add_library( ${ARG_TARGET} ${ARG_TYPE} ${files} )
  endif()
endfunction()

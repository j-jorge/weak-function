#pragma once

#ifdef WDL_DEBUG
  #include <cassert>
  #define wfl_debug_assert( b ) assert( b )
#else
  #define wfl_debug_assert( b ) do {} while ( false )
#endif

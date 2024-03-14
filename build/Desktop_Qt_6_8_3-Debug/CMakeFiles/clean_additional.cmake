# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/StrawberryRemote_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/StrawberryRemote_autogen.dir/ParseCache.txt"
  "StrawberryRemote_autogen"
  "proto/CMakeFiles/Proto_autogen.dir/AutogenUsed.txt"
  "proto/CMakeFiles/Proto_autogen.dir/ParseCache.txt"
  "proto/Proto_autogen"
  )
endif()

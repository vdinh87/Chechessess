# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\ChessQt_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\ChessQt_autogen.dir\\ParseCache.txt"
  "ChessQt_autogen"
  )
endif()

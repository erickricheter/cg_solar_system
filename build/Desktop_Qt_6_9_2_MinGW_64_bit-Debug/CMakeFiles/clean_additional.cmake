# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CGUtfpr_autogen"
  "CMakeFiles\\CGUtfpr_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\CGUtfpr_autogen.dir\\ParseCache.txt"
  )
endif()

# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\EasyDRM_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\EasyDRM_autogen.dir\\ParseCache.txt"
  "EasyDRM_autogen"
  )
endif()

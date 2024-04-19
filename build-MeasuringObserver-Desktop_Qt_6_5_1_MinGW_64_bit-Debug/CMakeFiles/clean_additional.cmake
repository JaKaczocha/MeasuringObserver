# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\appMeasuringObserver_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\appMeasuringObserver_autogen.dir\\ParseCache.txt"
  "appMeasuringObserver_autogen"
  )
endif()

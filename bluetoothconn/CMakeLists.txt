cmake_minimum_required ( VERSION 2.6 )
project ( btconn )

# btconn Version 0.1
set ( btconn_VERSION_MAJOR 0 )
set ( btconn_VERSION_MINOR 1 )
configure_file (
        "${PROJECT_SOURCE_DIR}/btconn.h.in"
        "${PROJECT_BINARY_DIR}/btconn.h"
)

find_package ( Boost 1.60 REQUIRED )

include_directories ( "${Boost_INCLUDE_DIR}" )
include_directories ( "${PROJECT_BINARY_DIR}" )
include_directories ( "${PROJECT_SOURCE_DIR}" )
add_library ( btconn src/BtLogger.cpp src/SpheroHandler.cpp )

install ( TARGETS btconn DESTINATION "lib" )
install ( DIRECTORY "${PROJECT_SOURCE_DIR}/btconn" DESTINATION "include" )
install ( FILES "${PROJECT_BINARY_DIR}/btconn.h" DESTINATION "include" )


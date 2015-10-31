cmake_minimum_required(VERSION 3.2)

project(thread_pool)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DCURL_STATICLIB -pthread")

set(LIB_DIR "${PROJECT_SOURCE_DIR}/libs" )
set(cURL_DIR_INCLUDE "${LIB_DIR}/curl/include/")
set(cURL_DIR_LIB "${LIB_DIR}/curl/lib/")

include_directories("${cURL_DIR_INCLUDE}")
link_directories("${cURL_DIR_LIB}" )

set(SOURCE_FILES main.cpp thread_pool.cpp)
add_executable(thread_pool ${SOURCE_FILES} )
target_link_libraries(thread_pool curl)
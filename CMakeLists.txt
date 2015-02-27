# Notation [XX][dWW-YY-..] -> ElementId=XX / Depends from WW & YY

cmake_minimum_required(VERSION 2.8)

# ********[00] PROJECT VARIABLES ******** #
project(MP2P)

# ********[01] CMAKE OPTIONS ********* #
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


# ********[02] COMPILER OPTIONS ******** #
set(CMAKE_CXX_COMPILER "clang++")

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall -Wextra -Werror -pedantic")
else(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall -Wextra -Werror -pedantic -pthread")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

# ********[03] ADDING BOOST LIBRARY ******** #
set(BOOST_ROOT /opt/boost)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS system)
include_directories(${Boost_INCLUDE_DIR})


# ********[04][d01] ADDING LIBCONFIG LIBRARY ******** #
#set(LibConfig++_DIR /usr/include)
find_package(LibConfig++ REQUIRED)
include_directories(${LIBCONFIG++_INCLUDE_DIR})


# ********[05][d01-02-03-04] SOURCE FILES ******** #
add_subdirectory(${SOURCE_DIR}/libtools) # The tools library
add_subdirectory(${SOURCE_DIR}/client)   # The client
add_subdirectory(${SOURCE_DIR}/server)   # The server
add_subdirectory(${SOURCE_DIR}/tests)    # The "tests" target





#add_executable(${PROJECT_NAME} ${SOURCES})
#set_target_properties(td42 PROPERTIES COMPILE_FLAGS "${CXX_FLAGS} -O3")
#target_link_libraries(${PROJECT_NAME} ${SFML_LIBRARIES})

#target_link_libraries(42sh ${CMAKE_THREAD_LIBS_INIT})

# The debug target
#add_executable(debug EXCLUDE_FROM_ALL ${SOURCES})
#set_target_properties(debug PROPERTIES COMPILE_FLAGS "${CXX_FLAGS} -ggdb3")
#target_link_libraries(debug ${SFML_LIBRARIES})

#target_link_libraries(debug ${CMAKE_THREAD_LIBS_INIT})

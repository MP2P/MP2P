# Notation [XX][dWW-YY-..] -> ElementId=XX / Depends from WW & YY

cmake_minimum_required(VERSION 3.1)

# ********[00] PROJECT VARIABLES ******** #
project(MP2P)

# ********[01] CMAKE OPTIONS ********* #
# Finding libconfig++
find_path(LIBCONFIG++_INCLUDE_DIR libconfig.h++)
find_library(LIBCONFIG++_LIBRARY config++)
if (LIBCONFIG++_INCLUDE_DIR AND LIBCONFIG++_LIBRARY)
   set(LIBCONFIG++_FOUND TRUE)
else (LIBCONFIG++_INCLUDE_DIR AND LIBCONFIG++_LIBRARY)
   set(LIBCONFIG++_FOUND FALSE)
endif (LIBCONFIG++_INCLUDE_DIR AND LIBCONFIG++_LIBRARY)

#MESSAGE( STATUS "LIBCONFIG++_FOUND:      " ${LIBCONFIG++_FOUND} )

find_library(LIBCRYPTO_LIBRARY crypto)
if (LIBCRYPTO_LIBRARY)
   set(LIBCRYPTO_FOUND TRUE)
else (LIBCRYPTO_LIBRARY)
   set(LIBCRYPTO_FOUND FALSE)
endif (LIBCRYPTO_LIBRARY)

#MESSAGE( STATUS "LIBCRYPTO_FOUND:        " ${LIBCRYPTO_FOUND} )

find_path(OPENSSL_INCLUDE_DIR openssl/sha.h)
find_library(OPENSSL_LIBRARY ssl)
if (OPENSSL_INCLUDE_DIR AND OPENSSL_LIBRARY)
   set(OPENSSL_FOUND TRUE)
else (OPENSSL_INCLUDE_DIR AND OPENSSL_LIBRARY)
   set(OPENSSL_FOUND FALSE)
endif (OPENSSL_INCLUDE_DIR AND OPENSSL_LIBRARY)

#MESSAGE( STATUS "OPENSSL_FOUND:          " ${OPENSSL_FOUND} )

set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


# ********[02] COMPILER OPTIONS ******** #
#set(CMAKE_CXX_COMPILER "clang++")
#set(CMAKE_CXX_COMPILER if hash clang++ 2>/dev/null; then "clang++" else "g++")
#execute_process(COMMAND (if hash clang++ 2>/dev/null; then echo clang++; else echo g++; fi) OUTPUT_VARIABLE CMAKE_CXX_COMPILER)

execute_process(COMMAND which clang++ RESULT_VARIABLE clangnotavailable OUTPUT_QUIET ERROR_QUIET)
#message("cavailable: ${cavailable}")
if(${clangnotavailable} EQUAL 0)
  set(CMAKE_CXX_COMPILER "clang++")
#else(NOT ${cavailable})
#  set(CMAKE_CXX_COMPILER "g++")
endif(${clangnotavailable} EQUAL 0)

#message(STATUS "CMAKE_CXX_COMPILER:     " ${CMAKE_CXX_COMPILER})

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -pedantic -Wall -Wextra -Werror -O3 -Wno-error=deprecated-declarations")
set(SHARED_FLAGS "-std=c++1y -pedantic -Wall -Wextra -Werror -Wno-error=deprecated-declarations")

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

# ********[03] ADDING BOOST LIBRARY ******** #
set(BOOST_ROOT /opt/boost)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS system iostreams REQUIRED)
include_directories(${Boost_INCLUDE_DIR})


# ********[04][d01] ADDING LIBCONFIG LIBRARY ******** #
#set(LibConfig++_DIR /usr/include)
#find_package(LibConfig++ REQUIRED)
include_directories(${LIBCONFIG++_INCLUDE_DIR})

# ********[04][d02] ADDING OPENSSL LIBRARY ********** #
include_directories(${OPENSSL_INCLUDE_DIR})


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

cmake_minimum_required(VERSION 2.8)
project(MP2P)

set(CMAKE_CXX_COMPILER "clang++")
set(CXX_FLAGS "-Wall -Wextra -Werror -pedantic -std=c++1y")
set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

#The tools library
add_subdirectory(${SOURCE_DIR}/libtools)

# The "tests" target
add_subdirectory(${SOURCE_DIR}/tests)

#add_executable(${PROJECT_NAME} ${SOURCES})
#set_target_properties(td42 PROPERTIES COMPILE_FLAGS "${CXX_FLAGS} -O3")
#target_link_libraries(${PROJECT_NAME} ${SFML_LIBRARIES})

#target_link_libraries(42sh ${CMAKE_THREAD_LIBS_INIT})

# The debug target
#add_executable(debug EXCLUDE_FROM_ALL ${SOURCES})
#set_target_properties(debug PROPERTIES COMPILE_FLAGS "${CXX_FLAGS} -ggdb3")
#target_link_libraries(debug ${SFML_LIBRARIES})

#target_link_libraries(debug ${CMAKE_THREAD_LIBS_INIT})

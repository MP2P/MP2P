file(GLOB_RECURSE TESTSSOURCES *.cc)
add_executable(tests ${TESTSSOURCES})
target_link_libraries(tests LINK_PUBLIC libtools)
target_include_directories(tests PUBLIC include)

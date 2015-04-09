file(GLOB_RECURSE TESTSSOURCES *.cc)

#The debug target
add_executable(tests-debug ${TESTSSOURCES})
target_link_libraries(tests-debug LINK_PUBLIC libtools-debug)
#target_include_directories(tests-debug PUBLIC include)
set_target_properties(tests-debug PROPERTIES COMPILE_FLAGS "-ggdb3 ${SHARED_FLAGS}")

#The release target
add_executable(tests-release ${TESTSSOURCES})
target_link_libraries(tests-release LINK_PUBLIC libtools-release)
#target_include_directories(tests-release PUBLIC include)
set_target_properties(tests-release PROPERTIES EXCLUDE_FROM_ALL true COMPILE_FLAGS "-O3 ${SHARED_FLAGS}")

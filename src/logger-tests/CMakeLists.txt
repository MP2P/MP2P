file(GLOB_RECURSE TESTSSOURCES *.cc)

#The debug target
add_executable(logger-tests-debug ${TESTSSOURCES})
add_dependencies(debug logger-tests-debug)
target_link_libraries(logger-tests-debug LINK_PUBLIC libtools-debug)
#target_include_directories(logger-tests-debug PUBLIC include)
set_target_properties(logger-tests-debug PROPERTIES COMPILE_FLAGS "-ggdb3 ${SHARED_FLAGS}")

#The release target
add_executable(logger-tests-release ${TESTSSOURCES})
add_dependencies(release logger-tests-release)
target_link_libraries(logger-tests-release LINK_PUBLIC libtools-release)
#target_include_directories(logger-tests-release PUBLIC include)
set_target_properties(logger-tests-release PROPERTIES EXCLUDE_FROM_ALL true COMPILE_FLAGS "-O3 ${SHARED_FLAGS}")

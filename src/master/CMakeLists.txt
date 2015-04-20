file(GLOB_RECURSE SERVERSOURCES *.cc)

#The degug target
add_executable(master-debug ${SERVERSOURCES})
add_dependencies(debug master-debug)
target_link_libraries(master-debug LINK_PUBLIC libtools-debug couchbase)
target_include_directories(master-debug PUBLIC .)
set_target_properties(master-debug PROPERTIES COMPILE_FLAGS "-ggdb3 ${SHARED_FLAGS}")

#The release target
add_executable(master-release ${SERVERSOURCES})
add_dependencies(release master-release)
target_link_libraries(master-release LINK_PUBLIC libtools-release couchbase)
target_include_directories(master-release PUBLIC .)
set_target_properties(master-release PROPERTIES EXCLUDE_FROM_ALL true COMPILE_FLAGS "-O3 ${SHARED_FLAGS}")

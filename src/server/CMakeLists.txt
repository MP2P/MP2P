file(GLOB_RECURSE SERVERSOURCES *.cc)

#The degug target
add_executable(server-debug ${SERVERSOURCES})
target_link_libraries(server-debug LINK_PUBLIC libtools-debug)
target_include_directories(server-debug PUBLIC include)
set_target_properties(server-debug PROPERTIES COMPILE_FLAGS "-ggdb3 ${SHARED_FLAGS}")

#The release target
add_executable(server-release ${SERVERSOURCES})
target_link_libraries(server-release LINK_PUBLIC libtools-release)
target_include_directories(server-release PUBLIC include)
set_target_properties(server-release PROPERTIES EXCLUDE_FROM_ALL true COMPILE_FLAGS "-O3 ${SHARED_FLAGS}")

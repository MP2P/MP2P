file(GLOB_RECURSE SERVERSOURCES *.cc)

#The degug target
add_executable(storage-debug ${SERVERSOURCES})
target_link_libraries(storage-debug LINK_PUBLIC libtools-debug)
target_include_directories(storage-debug PUBLIC include)
set_target_properties(storage-debug PROPERTIES COMPILE_FLAGS "-ggdb3 ${SHARED_FLAGS}")

#The release target
add_executable(storage-release ${SERVERSOURCES})
target_link_libraries(storage-release LINK_PUBLIC libtools-release)
target_include_directories(storage-release PUBLIC include)
set_target_properties(storage-release PROPERTIES EXCLUDE_FROM_ALL true COMPILE_FLAGS "-O3 ${SHARED_FLAGS}")

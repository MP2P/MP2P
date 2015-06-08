#The degug target
add_executable(storage-debug ${STORAGESOURCES})
add_dependencies(debug storage-debug)
target_link_libraries(storage-debug LINK_PUBLIC libtools-debug)
target_include_directories(storage-debug PUBLIC include)
set_target_properties(storage-debug PROPERTIES COMPILE_FLAGS ${DEBUG_FLAGS})

#The release target
add_executable(storage-release ${STORAGESOURCES})
add_dependencies(release storage-release)
target_link_libraries(storage-release LINK_PUBLIC libtools-release)
target_include_directories(storage-release PUBLIC include)
set_target_properties(storage-release PROPERTIES EXCLUDE_FROM_ALL true COMPILE_FLAGS ${OPTIMIZED_FLAGS})

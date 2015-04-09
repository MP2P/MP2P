file(GLOB_RECURSE CLIENTSOURCES *.cc)

#The debug version
add_executable(client-debug ${CLIENTSOURCES})
target_link_libraries(client-debug LINK_PUBLIC libtools-debug)
target_include_directories(client-debug PUBLIC include)
set_target_properties(client-debug PROPERTIES COMPILE_FLAGS "-ggdb3 ${SHARED_FLAGS}")

#The release version
add_executable(client-release ${CLIENTSOURCES})
target_link_libraries(client-release LINK_PUBLIC libtools-release)
target_include_directories(client-release PUBLIC include)
set_target_properties(client-release PROPERTIES EXCLUDE_FROM_ALL true COMPILE_FLAGS "-O3 ${SHARED_FLAGS}")

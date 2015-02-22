file(GLOB_RECURSE SERVERSOURCES *.cc)
add_executable(server ${SERVERSOURCES})
target_link_libraries(server LINK_PUBLIC libtools)
#target_include_directories(server PUBLIC include)

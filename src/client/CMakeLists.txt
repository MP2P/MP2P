file(GLOB_RECURSE CLIENTSOURCES *.cc)
add_executable(client ${CLIENTSOURCES})
target_link_libraries(client LINK_PUBLIC libtools)
#target_include_directories(client PUBLIC include)

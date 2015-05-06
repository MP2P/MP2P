add_executable(tests-client ${CLIENTSOURCES})
add_dependencies(test-suite tests-clients)
target_link_libraries(tests-client LINK_PUBLIC libtools-release)
set_target_properties(tests-client PROPERTIES COMPILE_FLAGS "-O3 ${SHARED_FLAGS}")

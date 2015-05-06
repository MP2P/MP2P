add_executable(tests-master ${MASTERSOURCES})
add_dependencies(test-suite tests-master)
target_link_libraries(tests-master LINK_PUBLIC libtools-debug)
set_target_properties(tests-master PROPERTIES COMPILE_FLAGS "-O3 ${SHARED_FLAGS}")

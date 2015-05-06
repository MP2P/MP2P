add_executable(tests-storage ${STORAGESOURCES})
add_dependencies(test-suite tests-storage)
target_link_libraries(tests-storage LINK_PUBLIC libtools-debug)
set_target_properties(tests-storage PROPERTIES COMPILE_FLAGS "-O3 ${SHARED_FLAGS}")

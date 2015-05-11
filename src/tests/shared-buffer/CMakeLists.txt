set(CLIENT_SRC_WITHOUT_MAIN ${CLIENTSOURCES})

# Adding the main.cc of the tests/shared-buffer folder
file(GLOB_RECURSE TESTS_SRC *.cc)

# Merging two lists
set(TESTS_CLIENT_SRC ${CLIENT_SRC_WITHOUT_MAIN} ${TESTS_SRC})

add_executable(tests-shared-buffer ${TESTS_CLIENT_SRC})
add_dependencies(test-suite tests-shared-buffer)
add_dependencies(release tests-shared-buffer)
target_link_libraries(tests-shared-buffer LINK_PUBLIC libtools-debug couchbase)
set_target_properties(tests-shared-buffer PROPERTIES EXCLUDE_FROM_ALL true COMPILE_FLAGS "-O3 ${SHARED_FLAGS}")

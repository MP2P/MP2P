set(CLIENT_SRC_WITHOUT_MAIN ${CLIENTSOURCES})

# Removing the main.cc of src/client folder
list(REMOVE_ITEM CLIENT_SRC_WITHOUT_MAIN ${SOURCE_DIR}/client/main.cc)

# Adding the main.cc of the tests/client folder
file(GLOB_RECURSE TESTS_SRC *.cc)

# Merging two lists
set(TESTS_CLIENT_SRC ${CLIENT_SRC_WITHOUT_MAIN} ${TESTS_SRC})

add_executable(tests-client ${TESTS_CLIENT_SRC})
add_dependencies(test-suite tests-client)
add_dependencies(release tests-client)
target_link_libraries(tests-client LINK_PUBLIC libtools-debug couchbase)
set_target_properties(tests-client PROPERTIES EXCLUDE_FROM_ALL true COMPILE_FLAGS "-O3 ${SHARED_FLAGS}")

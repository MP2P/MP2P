set(MASTER_SRC_WITHOUT_MAIN ${MASTERSOURCES})

# Removing the main.cc of src/master folder
list(REMOVE_ITEM MASTER_SRC_WITHOUT_MAIN ${SOURCE_DIR}/master/main.cc)

# Adding the main.cc of the tests/master folder
file(GLOB_RECURSE TESTS_SRC *.cc)

# Merging two lists
set(TESTS_MASTER_SRC ${MASTER_SRC_WITHOUT_MAIN} ${TESTS_SRC})

add_executable(tests-master ${TESTS_MASTER_SRC})
add_dependencies(test-suite tests-master)
add_dependencies(release tests-master)
target_link_libraries(tests-master LINK_PUBLIC libtools-debug couchbase)
set_target_properties(tests-master PROPERTIES EXCLUDE_FROM_ALL true COMPILE_FLAGS "-O3 ${SHARED_FLAGS}")

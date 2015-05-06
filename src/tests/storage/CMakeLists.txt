set(STORAGE_SRC_WITHOUT_MAIN ${STORAGESOURCES})

# Removing the main.cc of src/storage folder
list(REMOVE_ITEM STORAGE_SRC_WITHOUT_MAIN ${SOURCE_DIR}/storage/main.cc)

# Adding the main.cc of the tests/storage folder
file(GLOB_RECURSE TESTS_SRC *.cc)

# Merging two lists
set(TESTS_STORAGE_SRC ${STORAGE_SRC_WITHOUT_MAIN} ${TESTS_SRC})

add_executable(tests-storage ${TESTS_STORAGE_SRC})
add_dependencies(test-suite tests-storage)
add_dependencies(release tests-storage)
target_link_libraries(tests-storage LINK_PUBLIC libtools-debug couchbase)
set_target_properties(tests-storage PROPERTIES EXCLUDE_FROM_ALL true COMPILE_FLAGS "-O3 ${SHARED_FLAGS}")

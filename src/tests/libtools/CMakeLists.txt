set(LIBTOOLS_SRC_WITHOUT_MAIN ${LIBTOOLSSOURCES})

# Adding the main.cc of the tests/libtools folder
file(GLOB_RECURSE TESTS_SRC *.cc)

# Merging two lists
set(TESTS_LIBTOOLS_SRC ${LIBTOOLS_SRC_WITHOUT_MAIN} ${TESTS_SRC})

add_executable(tests-libtools ${TESTS_LIBTOOLS_SRC})
add_dependencies(test-suite tests-libtools)
add_dependencies(release tests-libtools)
target_link_libraries(tests-libtools LINK_PUBLIC libtools-debug)
string(REPLACE "-Werror" "" SHARED_FLAGS_NO_ERROR ${SHARED_FLAGS})
set_target_properties(tests-libtools PROPERTIES EXCLUDE_FROM_ALL true COMPILE_FLAGS "-O3 ${SHARED_FLAGS_NO_ERROR}")

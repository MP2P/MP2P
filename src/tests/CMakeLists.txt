add_custom_target(check
  COMMAND ${CMAKE_BINARY_DIR}/tests-client
  COMMAND ${CMAKE_BINARY_DIR}/tests-master
  COMMAND ${CMAKE_BINARY_DIR}/tests-storage)
add_dependencies(check test-suite)
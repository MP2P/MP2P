file(GLOB_RECURSE LIBSOURCES *.cc)

#Add the subdirectories to the include paths
MACRO(SUBDIRLIST result)
  FILE(GLOB children RELATIVE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()
SUBDIRLIST(SUBDIRS)

#The degug target
add_library(libtools-debug ${LIBSOURCES})

foreach(subdir ${SUBDIRS})
  target_include_directories(libtools-debug PUBLIC ${subdir})
endforeach(subdir)

set_target_properties(libtools-debug PROPERTIES OUTPUT_NAME tools-debug)
set_target_properties(libtools-debug PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(libtools-debug PROPERTIES COMPILE_FLAGS "-ggdb3 ${SHARED_FLAGS}")
target_link_libraries(libtools-debug ${Boost_LIBRARIES} ${LIBCONFIG++_LIBRARY})

#The release target
add_library(libtools-release ${LIBSOURCES})

foreach(subdir ${SUBDIRS})
  target_include_directories(libtools-release PUBLIC ${subdir})
endforeach(subdir)

set_target_properties(libtools-release PROPERTIES OUTPUT_NAME tools-release)
set_target_properties(libtools-release PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(libtools-release PROPERTIES EXCLUDE_FROM_ALL true COMPILE_FLAGS "-O3 ${SHARED_FLAGS}")
target_link_libraries(libtools-release ${Boost_LIBRARIES} ${LIBCONFIG++_LIBRARY})

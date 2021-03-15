set(BINARY ${CMAKE_PROJECT_NAME}_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
set(SOURCES ${TEST_SOURCES})
add_executable(${BINARY} ${TEST_SOURCES})
add_test(NAME ${BINARY} COMMAND ${BINARY})
target_link_libraries(${BINARY} PUBLIC ${CMAKE_PROJECT_NAME}_lib gtest)

link_directories(
    ${GTKMM_LIBRARY_DIRS})

include_directories(
    ${GTKMM_INCLUDE_DIRS}
	../lib/yaml-cpp/include
	)

target_link_libraries(${BINARY}
	PUBLIC ${CMAKE_PROJECT_NAME}_lib gtest
    ${GTKMM_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
	yaml-cpp
	)


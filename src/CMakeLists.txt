set(BINARY ${CMAKE_PROJECT_NAME})
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp)
set(SOURCES ${SOURCES})
add_executable(${BINARY} ${SOURCES})
add_library(${BINARY}_lib STATIC ${SOURCES})

link_directories(
    ${GTKMM_LIBRARY_DIRS})

include_directories(
    ${GTKMM_INCLUDE_DIRS}
	../lib/third-party/yaml-cpp/include
	)

target_link_libraries(${BINARY}
    ${GTKMM_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
	yaml-cpp
	)


set(EXECUTABLE_OUTPUT_PATH "../bin")
set(CMAKE_CXX_FLAGS "-Ofast" )

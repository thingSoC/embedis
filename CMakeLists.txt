cmake_minimum_required (VERSION 2.8)

project (embedis)
enable_testing()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # set(CMAKE_C_FLAGS "-std=c99 ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    if (CMAKE_BUILD_TYPE MATCHES "Release")
        set(CMAKE_C_FLAGS "-flto ${CMAKE_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "-flto ${CMAKE_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "-flto ${CMAKE_EXE_LINKER_FLAGS}")
        set(CMAKE_SHARED_LINKER_FLAGS "-flto ${CMAKE_SHARED_LINKER_FLAGS}")
        set(CMAKE_STATIC_LINKER_FLAGS "-flto ${CMAKE_STATIC_LINKER_FLAGS}")
        set(CMAKE_MODULE_LINKER_FLAGS "-flto ${CMAKE_MODULE_LINKER_FLAGS}")
    endif()
endif()

### set up header groups for xcode and visual studio

file(GLOB_RECURSE INCLUDE_BENCHTEST "benchtest/include/*.hpp")
source_group("Header Files\\benchtest" FILES ${INCLUDE_BENCHTEST})

file(GLOB_RECURSE ARDUINO_MOCK "arduino/*")
source_group("Header Files\\arduino" FILES ${ARDUINO_MOCK})

file(GLOB_RECURSE EMBEDIS_SRC "src/*")
source_group("Source Files\\embedis" FILES ${EMBEDIS_SRC})

include_directories(src tests arduino benchtest/include)

### testing

file(GLOB_RECURSE TEST_EMBEDIS_SRC "tests/*.c*"  "tests/*.h*")
add_executable(test_embedis ${TEST_EMBEDIS_SRC} ${INCLUDE_BENCHTEST} ${ARDUINO_MOCK} ${EMBEDIS_SRC})
add_test(NAME run_test_embedis COMMAND test_embedis)

### documentation

find_package(Doxygen)
if(DOXYGEN_FOUND)
add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
    COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)

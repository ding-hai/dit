cmake_minimum_required(VERSION 3.21)
project(dit)

set(CMAKE_CXX_STANDARD 11)
set(common_repository /Users/dh/project/CLionProjects/common_repository)

set(Boost_INCLUDE_DIR ${common_repository}/boost_1.78/include)
set(Boost_LIBRARY_DIR ${common_repository}/boost_1.78/lib)
find_package(Boost COMPONENTS system filesystem program_options REQUIRED)

set(Zlib_INCLUDE_DIR ${common_repository}/zlib-1.2.11/include)
set(Zlib_LIBRARY_DIR ${common_repository}/zlib-1.2.11/lib)


set(SRC_FILES src/objects.cpp src/objects.h src/utils.cpp src/utils.h  src/exceptions.cpp src/exceptions.h src/file_system.cpp src/file_system.h)


# main executable
add_executable(dit src/main.cpp ${SRC_FILES})
message(STATUS ${Boost_INCLUDE_DIR} ${Zlib_INCLUDE_DIR} )
# boost , zlib
target_include_directories(dit PRIVATE ${Boost_INCLUDE_DIR} ${Zlib_INCLUDE_DIR} )
target_link_directories(dit PRIVATE ${Boost_LIBRARY_DIR} ${Zlib_LIBRARY_DIR})
target_link_libraries(dit PRIVATE ${Boost_LIBRARIES} z)


# google test
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)
enable_testing()
add_executable(test_objects test/test_objects.cpp ${SRC_FILES})
target_include_directories(test_objects PRIVATE src ${Boost_INCLUDE_DIR} ${Zlib_INCLUDE_DIR})
target_link_directories(test_objects PRIVATE ${Boost_LIBRARY_DIR} ${Zlib_LIBRARY_DIR})
target_link_libraries(test_objects PRIVATE gtest_main ${Boost_LIBRARIES} z)
include(GoogleTest)
gtest_discover_tests(test_objects)
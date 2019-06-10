cmake_minimum_required (VERSION 3.10)
project (ThousandEyes)

set(CMAKE_BUILD_TYPE Debug)
set(BOOST_ROOT /mnt/c/devel/3pty/boost_1_70_0)
set(BOOST_LIBRARYDIR /mnt/c/devel/3pty/boost_1_70_0/stage/lib)
set(GTEST_ROOT "/mnt/c/devel/3pty/googletest/build_wsl")
set(GTEST_INCLUDE_DIR "/mnt/c/devel/3pty/googletest/googletest/include")
set(GMOCK_INCLUDE_DIR "/mnt/c/devel/3pty/googletest/googlemock/include")
set(PROPERTIES
                CXX_STANDARD 17
                CXX_EXTENSIONS OFF )

enable_testing()

add_subdirectory(src)
add_subdirectory(unit_test)

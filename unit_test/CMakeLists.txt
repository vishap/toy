set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

find_package(GTest REQUIRED)

add_executable(unit_test 
                test_main.cpp
                scheduler.t.cpp)
target_include_directories(unit_test PUBLIC 
        ${PROJECT_SOURCE_DIR}/src
        ${GTEST_INCLUDE_DIR})

target_compile_options(scheduler PUBLIC "-pthread")
target_link_libraries(unit_test LINK_PUBLIC
    scheduler_lib
    ${Boost_LIBRARIES}
    ${GTEST_LIBRARIES}
    ${GMOCK_LIBRARIES})

gtest_discover_tests(unit_test)
#add_test(NAME unit_test COMMAND unit_test)

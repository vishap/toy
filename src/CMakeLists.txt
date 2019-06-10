set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

#find_package(Boost COMPONENTS asio REQUIRED)
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
include_directories(.)


add_library(scheduler_lib STATIC
    scheduler.cpp
    scheduler.h)
target_link_libraries(scheduler_lib LINK_PUBLIC
    ${Boost_LIBRARIES}
    Threads::Threads)

add_executable(scheduler main.cpp)
set_target_properties(scheduler PROPERTIES
                            CXX_STANDARD 17
                            CXX_EXTENSIONS OFF )

target_compile_options(scheduler PUBLIC "-pthread")
target_link_libraries(scheduler LINK_PUBLIC scheduler_lib)

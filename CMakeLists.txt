cmake_minimum_required(VERSION 3.16.3)
project(sik_zadanie2B)
#set(BOOST_ROOT "/usr/include/boost/")
set(Boost_USE_STATIC_LIBS OFF)

find_package(Boost COMPONENTS system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

if (NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find boost!")
endif ()

set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_STANDARD 20)

add_executable(robots-server robots-server.cpp
commandline_arguments.cpp sending_messages.cpp
serialization_deserialization.cpp event.cpp random_generator.cpp)

#target_link_libraries(robots-server ${Boost_LIBRARIES} Threads::Threads)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(Boost 1.40 COMPONENTS program_options REQUIRED)
target_link_libraries(robots-server Boost::program_options Threads::Threads)

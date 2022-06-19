cmake_minimum_required(VERSION 3.22)
project(SmallDB)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall -Wpedantic -Weffc++" )
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g" )
set(CMAKE_CXX_FLAGS_RELEASE "-O3" )

add_executable(SmallDB main.cpp SmallDB.cpp SmallDB.hpp Datatype.hpp TypeDB.hpp Exceptions.hpp Datetime.hpp Query.cpp Query.hpp)
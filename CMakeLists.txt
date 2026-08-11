cmake_minimum_required(VERSION 4.2)
project(Anansi)

set(CMAKE_CXX_STANDARD 20)
include(FetchContent)
add_library(Anansi_lib STATIC
        src/storage/RBTree.h
        src/storage/Memtable.h
        src/types/AnansiTypes.h
        src/storage/Nodes.h
        src/dependency_injection/application.h
        src/storage/SSHandler.h
        src/models/tables.h
        src/storage/SSHandler.cpp
)

add_executable(Anansi
        src/main.cpp
)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(Anansi_Tests
       test/memory/RBTreeTester.cpp
        test/TestHelper.h
        test/memory/SSHandlerTester.cpp
)

target_link_libraries(Anansi_Tests
        Anansi_lib
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(Anansi_Tests DISCOVERY_TIMEOUT 30)

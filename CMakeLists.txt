cmake_minimum_required(VERSION 3.21)
project(PlantManager VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#Lib
add_library(PMLib
    src/Sections.cpp
    src/Plant.cpp
    src/Console.cpp
    src/UserActionHandler.cpp
    src/Draw.cpp
)

target_include_directories(PMLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

#Exec
add_executable(PlantManager main.cpp)
target_link_libraries(PlantManager PRIVATE PMLib)
target_compile_options(PlantManager PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)

#Tests
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

#Text Exec
add_executable(PMTests
    tests/testSections.cpp
)

target_link_libraries(PMTests
    PRIVATE
    PMLib
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(PMTests)

add_custom_target(testAll
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS PlantManager PMTests
)
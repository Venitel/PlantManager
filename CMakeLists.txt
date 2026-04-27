cmake_minimum_required(VERSION 3.21)
project(PlantManager VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#SQLite
add_library(sqlite3 STATIC third_party/sqlite/sqlite3.c)
target_include_directories(sqlite3 PUBLIC third_party/sqlite)
target_compile_options(sqlite3 PRIVATE -w) #suppress all warnings for sqlite
target_compile_definitions(sqlite3 PRIVATE SQLITE_ENABLE_MATH_FUNCTIONS)
#Lib
add_library(PMLib
    src/Sections.cpp
    src/Record.cpp
    src/Plant.cpp
    src/Console.cpp
    src/UserActionHandler.cpp
    src/Draw.cpp
    src/Database.cpp
    src/Logger.cpp
    src/Species.cpp
    src/Schedule.cpp
    src/Utils.cpp
    src/Setting.cpp
)
target_include_directories(PMLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

#Exec
add_executable(PlantManager main.cpp)
target_link_libraries(PlantManager PRIVATE PMLib sqlite3)
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
    sqlite3
)

include(GoogleTest)
gtest_discover_tests(PMTests)

add_custom_target(testAll
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS PlantManager PMTests
)
# Setup
cmake_minimum_required(VERSION 3.10)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")
project(CooleyTukeyFilter)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(BUILD_TESTS "Build tests" OFF)
option(ADD_COVERAGE "Add code coverage report" OFF)
option(OPTIMIZE "Set optimization flags on" ON)

if(OPTIMIZE)
    set(CMAKE_CXX_FLAGS "-O2")
endif(OPTIMIZE)

# Build main program
add_executable(ctf src/main.cpp)
target_include_directories(ctf PUBLIC "${PROJECT_SOURCE_DIR}/src/include")

include(FetchContent)

# Build AudioFile library
FetchContent_Declare(
    AudioFile
    GIT_REPOSITORY https://github.com/adamstark/AudioFile.git
    GIT_TAG 1.1.1
)

FetchContent_MakeAvailable(AudioFile)
target_link_libraries(ctf AudioFile)

if(BUILD_TESTS)
    # Build Catch2 library
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.7.1
    )

    FetchContent_MakeAvailable(Catch2)

    # Build tests
    add_executable(tests tests/test.cpp)
    target_link_libraries(tests PRIVATE Catch2::Catch2WithMain AudioFile)
    target_include_directories(tests PUBLIC ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/tests AudioFile ${PROJECT_SOURCE_DIR}/build/_deps/audiofile-src)
    
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
endif(BUILD_TESTS)

if(ADD_COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags_to_target(tests)
    set(COVERAGE_EXCLUDES
        "${PROJECT_SOURCE_DIR}/build"
        "${PROJECT_SOURCE_DIR}/tests")
    setup_target_for_coverage_gcovr_html(
        NAME ctf_coverage
        EXECUTABLE tests
        DEPENDENCIES tests
        )
endif(ADD_COVERAGE)
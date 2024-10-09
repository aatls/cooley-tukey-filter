cmake_minimum_required(VERSION 3.10)
project(CooleyTukeyFilter)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(ctf src/main.cpp)

target_include_directories(ctf PUBLIC ${PROJECT_SOURCE_DIR}/src/include)

include(FetchContent)

FetchContent_Declare(
    audiofile
    GIT_REPOSITORY https://github.com/adamstark/AudioFile.git
    GIT_TAG 1.1.1)

FetchContent_MakeAvailable(audiofile)

target_link_libraries(ctf AudioFile)
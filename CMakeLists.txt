cmake_minimum_required(VERSION 3.10)
project(CooleyTukeyFilter)

add_executable(ctf src/main.cpp)

target_include_directories(ctf PUBLIC ${PROJECT_SOURCE_DIR}/src/include)
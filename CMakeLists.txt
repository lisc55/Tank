cmake_minimum_required(VERSION 3.10)
project(Tank)

add_compile_options(-O2)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../)

set(CMAKE_BUILD_TYPE "Debug")

add_executable(Tank main.cpp 
        src/rule.h src/rule.cpp src/template.h src/template.cpp
        json/json.h json/jsoncpp.cpp src/bot.h src/bot.cpp)

add_executable(Play Play.cpp
        src/rule.h src/rule.cpp src/template.h src/template.cpp
        json/json.h json/jsoncpp.cpp src/bot.h src/bot.cpp)
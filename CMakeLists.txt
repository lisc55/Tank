cmake_minimum_required(VERSION 3.10)
project(Tank)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../)

add_executable(Tank main.cpp src/template.h src/template.cpp src/rule.h src/rule.cpp
        json/json.h json/jsoncpp.cpp src/bot.h src/bot.cpp)
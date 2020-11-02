cmake_minimum_required(VERSION 3.10)
project(game_api)

SET(LIBRARY_OUTPUT_PATH ${GAME_BUILD}/lib)
add_compile_options(-std=gnu++11 -g)
include_directories(${PROJECT_SOURCE_DIR})

aux_source_directory(. API_SRC)
add_library(game_api SHARED ${API_SRC})
target_link_libraries(game_api protobuf curl)

add_executable(api_test api_test.cpp)
target_link_libraries(api_test game_api)
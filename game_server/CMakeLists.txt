cmake_minimum_required(VERSION 3.10)
project(tcp_daemon)

SET(EXECUTABLE_OUTPUT_PATH ${GAME_BUILD}/bin)
add_compile_options(-std=gnu++11 -g)

include_directories(${PROJECT_SOURCE_DIR})

aux_source_directory(. MAIN_SRC)

add_executable(tcp_daemon ${MAIN_SRC})

find_library(GAME_API_LIB libgame_api.so ${GAME_BUILD}/lib)
find_library(TCP_FRAMEWORK libtcp_framework.a ${GAME_BUILD}/lib)

target_link_libraries(tcp_daemon ${GAME_API_LIB} ${TCP_FRAMEWORK} protobuf uuid pthread curl sqlite3)
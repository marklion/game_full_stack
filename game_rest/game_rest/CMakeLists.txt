cmake_minimum_required(VERSION 2.6)

project (game_rest CXX)

set(GAME_REST_HEADERS game_rest.h)

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(GAME_REST_CODEGEN_DIR "${PROJECT_BINARY_DIR}/codegen")

PREPEND(GAME_REST_HEADERS_PATHS ${PROJECT_SOURCE_DIR} ${GAME_REST_HEADERS})

CODEGEN_FILES(GAME_REST_CODEGEN_SOURCES ${GAME_REST_CODEGEN_DIR} ${GAME_REST_HEADERS})

add_custom_command(OUTPUT ${GAME_REST_CODEGEN_SOURCES}
    COMMAND ${NGREST_BIN_PATH}ngrestcg -i "${PROJECT_SOURCE_DIR}" -o ${GAME_REST_CODEGEN_DIR} -t service ${GAME_REST_HEADERS}
    DEPENDS ${GAME_REST_HEADERS_PATHS}
)

file(GLOB GAME_REST_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)

list(APPEND GAME_REST_SOURCES ${GAME_REST_CODEGEN_SOURCES})

include_directories(${PROJECT_SOURCE_DIR} $ENV{NGREST_EXT_INCLUDES})

add_library(game_rest MODULE ${GAME_REST_SOURCES})

set_target_properties(game_rest PROPERTIES PREFIX "")
set_target_properties(game_rest PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SERVICES_DIR}"
)

find_library(GAME_API_LIB libgame_api.so ${GAME_BUILD}/lib)
target_link_libraries(game_rest ngrestutils ngrestcommon ngrestjson ngrestengine $ENV{NGREST_EXT_LIBS} ${GAME_API_LIB} protobuf)

cmake_minimum_required(VERSION 3.14)

project(jsonvsxml_app LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MU_ROOT ${CMAKE_CURRENT_LIST_DIR}/../..)
set(PROJECT_ROOT_DIR ${MU_ROOT})

set(CMAKE_MODULE_PATH
    ${MU_ROOT}/build
    ${MU_ROOT}/build/cmake
    ${CMAKE_MODULE_PATH}
    )

include(SetupBuildEnvironment)
include(GetPlatformInfo)

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
)

add_definitions(
    -DNO_QT_SUPPORT
    -DMUSESCORE_REVISION="abc123"
    -DMUSESCORE_VERSION="4.0.0"
    -DMUSESCORE_VERSION_LABEL="dev"
)

set(GLOBAL_NO_INTERNAL ON)
add_subdirectory(${MU_ROOT}/src/framework/global global)

#include(${MU_ROOT}/src/framework/global/modularity/modularity.cmake)

add_executable(jsonvsxml_app
    main.cpp
    score.cpp
    score.h
    rwjson.cpp
    rwjson.h
    rwxml.cpp
    rwxml.h
)

set_target_properties(global PROPERTIES COMPILE_FLAGS "-fPIC")
set_target_properties(jsonvsxml_app PROPERTIES COMPILE_FLAGS "-fPIC")

target_include_directories(jsonvsxml_app PUBLIC
    ${MU_ROOT}/src/framework
    ${MU_ROOT}/src/framework/global
    ${MU_ROOT}/src
)

target_link_libraries(jsonvsxml_app
    global
)

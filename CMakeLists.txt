cmake_minimum_required(VERSION 3.16...4.5)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(KNEe
    VERSION
        0.1
    DESCRIPTION
        "Teminal file-system Editor"
    LANGUAGES
        CXX
)

include(FetchContent)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v7.0.3
)
FetchContent_MakeAvailable(ftxui)

add_executable(knee)
target_sources(knee
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/renderer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/input_manager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/backend/file_handler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/backend/file_opener.cpp
)
target_include_directories(knee
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/backend/
)
target_compile_features(knee
    PRIVATE
        cxx_std_23
)
target_compile_options(knee
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall>
        $<$<CXX_COMPILER_ID:GNU,Clang>:-pedantic>
)
target_link_libraries(knee
    PRIVATE
        ftxui::ftxui
)



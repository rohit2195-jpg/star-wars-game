cmake_minimum_required(VERSION 3.20)
project(StarWarsGame VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# ─── Compiler warnings ───────────────────────────────────────────────────────
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ─── SDL2 (required) ─────────────────────────────────────────────────────────
# macOS/Homebrew: brew install sdl2
find_package(SDL2 REQUIRED CONFIG
    HINTS /usr/local/lib/cmake/SDL2 /opt/homebrew/lib/cmake/SDL2)

# ─── SDL2_image (Milestone 1+) ───────────────────────────────────────────────
# brew install sdl2_image
find_package(SDL2_image QUIET CONFIG
    HINTS /usr/local/lib/cmake/SDL2_image /opt/homebrew/lib/cmake/SDL2_image)

# ─── SDL2_mixer (Milestone 5+) ───────────────────────────────────────────────
# brew install sdl2_mixer
find_package(SDL2_mixer QUIET CONFIG
    HINTS /usr/local/lib/cmake/SDL2_mixer /opt/homebrew/lib/cmake/SDL2_mixer)

# ─── SDL2_ttf (Milestone 5+) ─────────────────────────────────────────────────
# brew install sdl2_ttf
find_package(SDL2_ttf QUIET CONFIG
    HINTS /usr/local/lib/cmake/SDL2_ttf /opt/homebrew/lib/cmake/SDL2_ttf)

# ─── Sources ─────────────────────────────────────────────────────────────────
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE src)

# ─── Link SDL2 ───────────────────────────────────────────────────────────────
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)

if(SDL2_image_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_image::SDL2_image)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAS_SDL2_IMAGE)
    message(STATUS "SDL2_image found — image loading enabled")
else()
    message(STATUS "SDL2_image not found (brew install sdl2_image) — BMP only for now")
endif()

if(SDL2_mixer_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_mixer::SDL2_mixer)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAS_SDL2_MIXER)
    message(STATUS "SDL2_mixer found — audio enabled")
else()
    message(STATUS "SDL2_mixer not found (brew install sdl2_mixer) — audio disabled")
endif()

if(SDL2_ttf_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_ttf::SDL2_ttf)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAS_SDL2_TTF)
    message(STATUS "SDL2_ttf found — font rendering enabled")
else()
    message(STATUS "SDL2_ttf not found (brew install sdl2_ttf) — font rendering disabled")
endif()

# ─── Assets: copy to build directory ─────────────────────────────────────────
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMENT "Copying assets to build directory"
)

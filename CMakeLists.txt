cmake_minimum_required(VERSION 3.29)
project(SDL3_Vk_ImGui)

set(PROJECT_NAME SDL3_Vk_ImGui)

set(CMAKE_CXX_STANDARD 23)

if (NOT MSVC)
    error("This project requires MSVC compiler.")
else ()
    # use multi-threaded static runtime, this could be necessary for some libraries
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

file(GLOB_RECURSE MODULE_FILES "src/*.ixx" "src/*.cppm")
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.cxx")
file(GLOB_RECURSE HEADER_FILES "src/*.h" "src/*.hpp")

add_executable(${PROJECT_NAME} ${MODULE_FILES} ${SOURCE_FILES} ${HEADER_FILES}
        src/CoreHeader.hpp)

target_sources(
        ${PROJECT_NAME}
        PRIVATE
        FILE_SET cxx_modules
        TYPE CXX_MODULES
        FILES
        ${MODULE_FILES}
        ${SOURCE_FILES})

set(SDL_VULKAN ON)
set(SDL_STATIC ON)
set(SDL_SHARED OFF)
add_subdirectory(vendor/SDL3)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3)

set(IMGUI_DEFINE_MATH_OPERATORS ON)
set(IMGUI_BACKEND_RENDERER vulkan CACHE STRING "ImGui backend renderer" FORCE)
set(IMGUI_BACKEND_PLATFORM sdl3 CACHE STRING "ImGui backend platform" FORCE)
add_subdirectory(vendor/imgui)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui::imgui)


find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Vulkan)
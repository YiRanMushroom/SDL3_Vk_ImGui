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
        src/Framework_glfw/FrameworkCoreHeader.hpp
        src/Framework_glfw/Program.ixx
        src/Framework_glfw/Program.cpp)

target_sources(
        ${PROJECT_NAME}
        PRIVATE
        FILE_SET cxx_modules
        TYPE CXX_MODULES
        FILES
        ${MODULE_FILES}
        ${SOURCE_FILES})

#set(SDL_VULKAN ON)
#set(SDL_STATIC ON)
#set(SDL_SHARED OFF)
#add_subdirectory(vendor/SDL3)
#target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3)

add_subdirectory(vendor/glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)

set(IMGUI_DEFINE_MATH_OPERATORS ON)
set(IMGUI_BACKEND_RENDERER vulkan CACHE STRING "ImGui backend renderer" FORCE)
set(IMGUI_BACKEND_PLATFORM glfw CACHE STRING "ImGui backend platform" FORCE)
add_subdirectory(vendor/imgui)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui::imgui)

# utf-8 for MSVC
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif ()

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Vulkan)

set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE ON
)
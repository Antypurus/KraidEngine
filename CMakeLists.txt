cmake_minimum_required(VERSION 3.16)

project(KraidEngine LANGUAGES C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS 
        ./src/**.cpp
        ./src/**.hpp
        ./src/**.h
        )

file(GLOB_RECURSE THIRD_PARTY_SOURCES CONFIGURE_DEPENDS
        ./ThirdParty/DearImGui/**.cpp
        ./ThirdParty/DearImGui/**.h
        )

add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${THIRD_PARTY_SOURCES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_link_libraries(${PROJECT_NAME} PRIVATE
    "d3d12.lib"
    "dxgi.lib"
    "D3DCompiler.lib"
    "dxcompiler.lib"
)

target_include_directories(${PROJECT_NAME} PRIVATE ./ThirdParty/)
target_include_directories(${PROJECT_NAME} PRIVATE ./src/)
target_precompile_headers(${PROJECT_NAME} PRIVATE 
        <windows.h>
        <d3d12.h>
        <wrl.h>
        <d3d12video.h>
        <dxgi1_6.h>
        <dxcapi.h>
        <d3d12shader.h>
        <d3dcompiler.h>
        <DirectXMath.h>

        <unordered_map>
        <string>
        <vector>
        <functional>
        <thread>
        <chrono>

        <DearImGui/imgui.h>
        <tinyobjloader/tiny_obj_loader.h>

        <Core/Windows.h>
        <Core/Rendering/D3D12/D3D12.h>
        <Core/Rendering/D3D12/D3D12SDK.h>
        <Core/stdcpp.h>
        )


if(MSVC)
    message("msvc build")
    add_compile_options(${PROJECT_NAME} /wd26495)
    add_compile_options(${PROJECT_NAME} /W4 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_SOURCE_DIR}/ThirdParty/DXC/"
    "${CMAKE_BINARY_DIR}")

add_custom_command(
        TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/Resources"
        "${CMAKE_BINARY_DIR}/Resources")

add_custom_command(
            TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PROJECT_SOURCE_DIR}/Shaders"
            "${CMAKE_BINARY_DIR}/Shaders")

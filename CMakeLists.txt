cmake_minimum_required(VERSION 3.16)
project(sdl_test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找所需的包
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_mixer REQUIRED)
find_package(SDL3_ttf REQUIRED)

# 添加可执行文件
add_subdirectory(libs/imgui)
add_executable(sdl_test 
                src/main.cpp
                src/Game/Game.cpp
                src/ECS/ECS.cpp
                src/AssetStore/AssetStore.cpp
                )

# 链接所有库
target_link_libraries(sdl_test PRIVATE 
    ${SDL3_LIBRARIES}
    SDL3_image::SDL3_image
    SDL3_mixer::SDL3_mixer
    SDL3_ttf::SDL3_ttf
    imgui
    ${CMAKE_SOURCE_DIR}/libs/lua/lua54.dll
)

# 复制dll动态链接库
add_custom_command(TARGET sdl_test POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  "${CMAKE_SOURCE_DIR}/libs/lua/lua54.dll"
  "$<TARGET_FILE_DIR:sdl_test>"
)

include_directories(
    ${CMAKE_SOURCE_DIR}/libs/
    ${CMAKE_SOURCE_DIR}/libs/lua
)
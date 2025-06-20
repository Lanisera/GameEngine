cmake_minimum_required(VERSION 3.16)
project(sdl_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找所需的包
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_mixer REQUIRED)
find_package(SDL3_ttf REQUIRED)

# 添加可执行文件
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
)

include_directories(
    ${CMAKE_SOURCE_DIR}/libs/glm
)
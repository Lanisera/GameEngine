cmake_minimum_required(VERSION 3.16)
project(sdl_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置SDL3相关库的路径
set(SDL3_DIR "C:/Library/SDL3/lib/cmake/SDL3")
set(SDL3_IMAGE_DIR "C:/Library/SDL3_image/lib/cmake/SDL3_image")
set(SDL3_MIXER_DIR "C:/Library/SDL3_mixer/lib/cmake/SDL3_mixer")
set(SDL3_TTF_DIR "C:/Library/SDL3_ttf/lib/cmake/SDL3_ttf")

# 查找所需的包
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_mixer REQUIRED)
find_package(SDL3_ttf REQUIRED)

# 添加可执行文件
add_executable(sdl_test 
                src/main.cpp
                src/Game.cpp
                )

# 链接所有库
target_link_libraries(sdl_test PRIVATE 
    ${SDL3_LIBRARIES}
    SDL3_image::SDL3_image
    SDL3_mixer::SDL3_mixer
    SDL3_ttf::SDL3_ttf
)

# 添加包含目录
target_include_directories(sdl_test PRIVATE
    ${SDL3_INCLUDE_DIRS}
    ${SDL3_IMAGE_INCLUDE_DIRS}
    ${SDL3_MIXER_INCLUDE_DIRS}
    ${SDL3_TTF_INCLUDE_DIRS}
) 
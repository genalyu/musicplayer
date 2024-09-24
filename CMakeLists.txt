cmake_minimum_required(VERSION 3.16)
project(onlinemusicpilayersearch)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找 Qt6 模块
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Sql Multimedia)

# 指定目标文件
add_executable(onlinemusicpilayersearch
    main.cpp
    mianwidget.cpp
    images.qrc  # 如果有资源文件
)

# 链接 Qt6 模块
target_link_libraries(onlinemusicpilayersearch
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::Sql
    Qt6::Multimedia
)

# 如果你的项目中有 UI 文件，可以添加类似以下内容来处理这些文件
# set_target_properties(onlinemusicpilayersearch PROPERTIES
#     AUTOMOC TRUE
#     AUTOUIC TRUE
#     AUTORCC TRUE
# )

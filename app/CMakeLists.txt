
find_package(Qt6 COMPONENTS Widgets OpenGL OpenGLWidgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(${PROJECT_NAME}
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        E57TreeNode.cpp
        E57TreeNode.h
        E57Tree.cpp
        E57Tree.h
        E57PropertyTree.cpp
        E57PropertyTree.h
        utils.h
        siimageviewer.cpp
        siimageviewer.h
        pointcloudrenderer/sipointcloudrenderer.cpp
        pointcloudrenderer/sipointcloudrenderer.h
        pointcloudrenderer/openglarraybuffer.cpp
        pointcloudrenderer/openglarraybuffer.h
        pointcloudrenderer/octree.h
        pointcloudrenderer/octree.cpp
        pointcloudrenderer/silrucache.h
        pointcloudrenderer/scene.h
        pointcloudrenderer/scene.cpp
        pointcloudrenderer/camera.h
        pointcloudrenderer/camera.cpp
        pointcloudrenderer/pointcloud.h
        pointcloudrenderer/pointcloud.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE
        E57Format
        Qt6::Widgets
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        ${PROJECT_NAME}_lib)
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/pointcloudrenderer)

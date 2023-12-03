
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
        sipointcloudrenderer.cpp
        sipointcloudrenderer.h
        openglarraybuffer.cpp
        openglarraybuffer.h
        octree.h
        octree.cpp
        silrucache.h
        scene.h
        scene.cpp
        camera.h
        camera.cpp
        pointcloud.h
        pointcloud.cpp
        resources/resources.qrc
        shader.cpp
        shader.h)
target_link_libraries(${PROJECT_NAME} PRIVATE
        E57Format
        Qt6::Widgets
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        ${PROJECT_NAME}_lib)
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/pointcloudrenderer)

if(WIN32)
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
endif()

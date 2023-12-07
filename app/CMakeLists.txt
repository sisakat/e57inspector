
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
        SceneView.cpp
        SceneView.h
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
        shader.h
        SceneTree.cpp
        SceneTree.h
        SceneTreeNode.cpp
        SceneTreeNode.h
        propertyeditor/CBaseProperty.cpp
        propertyeditor/CBaseProperty.h
        propertyeditor/CBoolProperty.cpp
        propertyeditor/CBoolProperty.h
        propertyeditor/CButtonBasedEditor.cpp
        propertyeditor/CButtonBasedEditor.h
        propertyeditor/CColorProperty.cpp
        propertyeditor/CColorProperty.h
        propertyeditor/CDateProperty.cpp
        propertyeditor/CDateProperty.h
        propertyeditor/CDateTimeProperty.cpp
        propertyeditor/CDateTimeProperty.h
        propertyeditor/CDoubleProperty.cpp
        propertyeditor/CDoubleProperty.h
        propertyeditor/CFontProperty.cpp
        propertyeditor/CFontProperty.h
        propertyeditor/CIntegerProperty.cpp
        propertyeditor/CIntegerProperty.h
        propertyeditor/CListProperty.cpp
        propertyeditor/CListProperty.h
        propertyeditor/CPropertyEditor.cpp
        propertyeditor/CPropertyEditor.h
        propertyeditor/CPropertyHeader.cpp
        propertyeditor/CPropertyHeader.h
        propertyeditor/CStringProperty.cpp
        propertyeditor/CStringProperty.h
        propertyeditor/CTimeProperty.cpp
        propertyeditor/CTimeProperty.h
        propertyeditor/QColorComboBox.cpp
        propertyeditor/QColorComboBox.h
        SceneTreeNodeFactory.cpp
        SceneTreeNodeFactory.h
        ScenePropertyEditor.cpp
        ScenePropertyEditor.h)
target_link_libraries(${PROJECT_NAME} PRIVATE
        E57Format
        Qt6::Widgets
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        ${PROJECT_NAME}_lib)
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/propertyeditor)

if(WIN32)
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
endif()

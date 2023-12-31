find_package(Qt6 COMPONENTS Widgets OpenGL OpenGLWidgets REQUIRED)
find_package(OpenGL REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/version.h PROPERTY SKIP_AUTOGEN ON)
set_source_files_properties(version.h.in PROPERTIES HEADER_FILE_ONLY TRUE)

add_executable(${PROJECT_NAME}
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        welcome.cpp
        welcome.h
        welcome.ui
        about.cpp
        about.h
        about.ui
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
        ScenePropertyEditor.h
        ScenePropertyEditorUtils.h
        boundingbox.h
        geometry.h
        Image2d.cpp
        Image2d.h
        E57Utils.cpp
        E57Utils.h
        ShaderFactory.cpp
        ShaderFactory.h
        version.h)
target_link_libraries(${PROJECT_NAME} PRIVATE
        E57Format
        Qt6::Widgets
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        ${PROJECT_NAME}_lib
        ${OPENGL_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/propertyeditor
        ${CMAKE_CURRENT_BINARY_DIR}
        ../external/glm/)

add_custom_command(
        DEPENDS version.h.in
        OUTPUT version.h
        COMMAND ${CMAKE_COMMAND}
        -DREPO_DIR="${CMAKE_CURRENT_SOURCE_DIR}/.."
        -DVERSION_SOURCE="${CMAKE_CURRENT_SOURCE_DIR}/version.h.in"
        -DVERSION_DESTINATION="${CMAKE_CURRENT_BINARY_DIR}/version.h"
        -DCMAKE_PROJECT_VERSION_MAJOR=${CMAKE_PROJECT_VERSION_MAJOR}
        -DCMAKE_PROJECT_VERSION_MINOR=${CMAKE_PROJECT_VERSION_MINOR}
        -DCMAKE_PROJECT_VERSION_PATCH=${CMAKE_PROJECT_VERSION_PATCH}
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/gitversion.cmake"
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
endif ()

if (WIN32)
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
    target_compile_definitions(${PROJECT_NAME} PUBLIC _USE_MATH_DEFINES)
endif ()

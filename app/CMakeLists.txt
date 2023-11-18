
find_package(Qt6 COMPONENTS Widgets REQUIRED)

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
        utils.h)
target_link_libraries(${PROJECT_NAME} PRIVATE
        E57Format
        Qt6::Widgets
        ${PROJECT_NAME}_lib)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

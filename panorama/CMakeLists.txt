add_library(${PROJECT_NAME}_panorama_lib
        panorama.h
        panorama.cpp)
target_link_libraries(${PROJECT_NAME}_panorama_lib PRIVATE
        E57Format
        ${PROJECT_NAME}_lib)
target_include_directories(${PROJECT_NAME}_panorama_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${PROJECT_NAME}_panorama
        main.cpp)
target_link_libraries(${PROJECT_NAME}_panorama PRIVATE
        ${PROJECT_NAME}_panorama_lib)
target_include_directories(${PROJECT_NAME}_panorama PRIVATE
        ../external
)

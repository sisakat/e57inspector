
set(library_name ${PROJECT_NAME}_lib)

set(HEADERS
        include/e57inspector/E57Reader.h)

set(SOURCES
        src/E57Reader.cpp
        src/E57ReaderImpl.cpp
        src/E57ReaderImpl.h
        include/e57inspector/E57Node.h)

add_library(${library_name} ${HEADERS} ${SOURCES})
target_include_directories(${library_name} PUBLIC include)
target_link_libraries(${library_name} PRIVATE E57Format)

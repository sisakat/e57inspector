cmake_minimum_required(VERSION 3.23)

project(e57inspector VERSION 0.2.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        libE57Format
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/libE57Format
)
FetchContent_MakeAvailable(libE57Format)

add_subdirectory(lib)
add_subdirectory(app)


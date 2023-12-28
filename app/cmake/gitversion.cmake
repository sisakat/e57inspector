# gitversion.cmake
cmake_minimum_required(VERSION 3.0.0)

message(STATUS "Resolving GIT Version")

find_package(Git)
if(GIT_FOUND)
    execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
            WORKING_DIRECTORY ${REPO_DIR}
            OUTPUT_VARIABLE APP_GIT_HASH)
    string(STRIP "${APP_GIT_HASH}" APP_GIT_HASH)

    message( STATUS "GIT hash: ${APP_GIT_HASH}")
else()
    message(STATUS "GIT not found")
endif()

string(TIMESTAMP APP_BUILD_DATE UTC)

configure_file(${VERSION_SOURCE} ${VERSION_DESTINATION})
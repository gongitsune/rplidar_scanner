cmake_minimum_required(VERSION 3.30)

project(rplidar_scanner LANGUAGES CXX)

# Options
set(LIB_NAME "rplidar_scanner")

# Set C++ standard to C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(ExternalProject)

# RPLIDAR SDK
set(SDK_PREFIX ${CMAKE_BINARY_DIR}/external/rplidar_sdk)
set(SDK_INCLUDE_DIR ${SDK_PREFIX}/include)
set(SDK_LIB_DIR ${SDK_PREFIX}/lib)

file(MAKE_DIRECTORY ${SDK_INCLUDE_DIR})
ExternalProject_Add(project_rplidar_sdk
    GIT_REPOSITORY https://github.com/Slamtec/rplidar_sdk.git
    GIT_TAG master
    PREFIX ${SDK_PREFIX}

    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND make
    BUILD_IN_SOURCE 1

    INSTALL_COMMAND
        ${CMAKE_COMMAND} -E make_directory ${SDK_LIB_DIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SDK_INCLUDE_DIR}

        COMMAND ${CMAKE_COMMAND} -E copy
            <SOURCE_DIR>/output/${CMAKE_SYSTEM_NAME}/Release/libsl_lidar_sdk.a
            ${SDK_LIB_DIR}/

        COMMAND ${CMAKE_COMMAND} -E copy_directory
            <SOURCE_DIR>/sdk/include
            ${SDK_INCLUDE_DIR}/
)

add_library(rplidar_sdk STATIC IMPORTED GLOBAL)
set_target_properties(rplidar_sdk PROPERTIES
    IMPORTED_LOCATION ${SDK_LIB_DIR}/libsl_lidar_sdk.a
    INTERFACE_INCLUDE_DIRECTORIES ${SDK_INCLUDE_DIR}
)
add_dependencies(rplidar_sdk project_rplidar_sdk)

# Add subdirectories
add_subdirectory(src)

# Link libraries
target_link_libraries(${LIB_NAME} PRIVATE rplidar_sdk)


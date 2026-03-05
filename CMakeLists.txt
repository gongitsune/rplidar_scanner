cmake_minimum_required(VERSION 3.30)

project(rplidar_scanner LANGUAGES CXX)

# Options
set(APP_NAME "rplidar_scanner")
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/ThirdParty)

# ThirdParty library versions
set(RPLIDAR_SDK_VERSION "master")
set(LIBLO_VERSION "master")
set(TOMLPLUSPLUS_VERSION "master")
set(FTXUI_VERSION "main")
set(SPDLOG_VERSION "v1.17.0")

set(
  CMAKE_CXX_FLAGS_DEBUG
  "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer"
)
set(
  CMAKE_EXE_LINKER_FLAGS_DEBUG
  "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address"
)

# Set C++ standard to C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(ExternalProject)
include(FetchContent)

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

        COMMAND ${CMAKE_COMMAND} -E copy_directory
            <SOURCE_DIR>/sdk/src/hal
            ${SDK_INCLUDE_DIR}/hal

    BUILD_BYPRODUCTS ${SDK_LIB_DIR}/libsl_lidar_sdk.a
)

add_library(rplidar_sdk STATIC IMPORTED GLOBAL)
set_target_properties(rplidar_sdk PROPERTIES
    IMPORTED_LOCATION ${SDK_LIB_DIR}/libsl_lidar_sdk.a
    INTERFACE_INCLUDE_DIRECTORIES ${SDK_INCLUDE_DIR}
)
add_dependencies(rplidar_sdk project_rplidar_sdk)

# Liblo
FetchContent_Declare(
    liblo
    GIT_REPOSITORY https://github.com/radarsat1/liblo.git
    GIT_TAG ${LIBLO_VERSION}
    SOURCE_SUBDIR cmake
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "Build tests" FORCE)
FetchContent_MakeAvailable(liblo)

# Tomlplusplus
FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG ${TOMLPLUSPLUS_VERSION}
)
FetchContent_MakeAvailable(tomlplusplus)

# Ftxui
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG ${FTXUI_VERSION}
)
FetchContent_MakeAvailable(ftxui)

# Spdlog
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG ${SPDLOG_VERSION}
)
FetchContent_MakeAvailable(spdlog)

# Add subdirectories
add_subdirectory(src)

# Link libraries
target_link_libraries(${APP_NAME} PRIVATE
  rplidar_sdk
  liblo
  tomlplusplus::tomlplusplus
  ftxui::screen ftxui::dom ftxui::component
  spdlog::spdlog
)

# Copy compile_commands.json to source directory
# for better integration with IDEs and tools
if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json")
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json"
        "${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json")
endif()

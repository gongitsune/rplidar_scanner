include_directories(
    ${THIRD_PARTY_DIR}/tomlplusplus/include
    ${CMAKE_SOURCE_DIR}/include
)

add_executable(${APP_NAME}
    main.cpp
    config.cpp
    lidar.cpp
    osc.cpp
)

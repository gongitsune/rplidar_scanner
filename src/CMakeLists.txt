include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

add_executable(${APP_NAME}
    main.cpp
    config.cpp
    lidar.cpp
    osc.cpp
    ui.cpp
    log.cpp
)

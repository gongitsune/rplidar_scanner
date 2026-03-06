# RPLIDAR SCANNER

This is a rplidar tui visualizer that can also send data via OSC and is supposedly cross-platform.

![visualizer.png](docs/visualizer.png)

## Usage

### Build

The build requires an internet connection due to the use of FetchContent and ExternalProject.

```sh
cmake -S . -B build -G Ninja && cmake --build build 
```

## Dependencies

- ![rplidar_sdk](https://github.com/Slamtec/rplidar_sdk)
- ![liblo](https://github.com/radarsat1/liblo)
- ![FTXUI](https://github.com/ArthurSonzogni/FTXUI)
- ![spdlog](https://github.com/gabime/spdlog)
- ![tomlplusplus](https://github.com/marzer/tomlplusplus)

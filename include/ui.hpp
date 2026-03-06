#pragma once

#include "config.hpp"
#include "lidar.hpp"
#include "log.hpp"

namespace rplidar_scanner {
class UI {
public:
  explicit UI(rplidar_scanner::Config &config, rplidar_scanner::Log &log,
              rplidar_scanner::LidarSensor &sensor);
};
} // namespace rplidar_scanner

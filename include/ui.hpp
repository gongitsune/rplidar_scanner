#pragma once

#include "config.hpp"
#include "log.hpp"

namespace rplidar_scanner {
class UI {
public:
  explicit UI(rplidar_scanner::Config &config, rplidar_scanner::Log &log);
};
} // namespace rplidar_scanner

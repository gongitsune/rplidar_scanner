#pragma once

#include <memory>
#include <sl_lidar_driver.h>
#include <sl_types.h>

namespace rplidar_scanner {
class Driver {
public:
  Driver();
  ~Driver();

  bool connect(const std::string &port, int baudrate);
  bool health_check();

private:
  std::unique_ptr<sl::ILidarDriver> driver;
  sl_result connect_result;
};
} // namespace rplidar_scanner

#pragma once

#include <memory>
#include <sl_lidar_cmd.h>
#include <sl_lidar_driver.h>
#include <sl_types.h>

namespace rplidar_scanner {
typedef sl_lidar_response_measurement_node_hq_t scan_node_t;

class Driver {
public:
  Driver();
  ~Driver();

  bool connect(const std::string &port, int baudrate);
  bool health_check();
  void start_scan();
  bool fetch_scan_data(scan_node_t *nodes, size_t &count);

private:
  std::unique_ptr<sl::ILidarDriver> driver_;
  sl_result connect_result_;
};
} // namespace rplidar_scanner

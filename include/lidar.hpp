#pragma once

#include "uitls.hpp"
#include <memory>
#include <mutex>
#include <optional>
#include <sl_lidar_cmd.h>
#include <sl_lidar_driver.h>
#include <sl_types.h>

#define LIDAR_MAX_NODES 8192

namespace rplidar_scanner {
typedef sl_lidar_response_measurement_node_hq_t scan_node_t;

struct ScanData {
  NO_COPY(ScanData)
  ScanData() = default;

  scan_node_t nodes[LIDAR_MAX_NODES];
  std::size_t count = 0;
};

class LidarSensor {
public:
  LidarSensor();
  ~LidarSensor();

  bool connect(const std::string &port, int baudrate);
  bool health_check();
  void start_scan();
  std::optional<ScanData *> fetch_scan_data();
  std::optional<ScanData *> get_scan_data();

private:
  std::unique_ptr<sl::ILidarDriver> driver_;
  sl_result connect_result_;
  ScanData scan_data_;
  std::mutex scan_mutex_;
};
} // namespace rplidar_scanner

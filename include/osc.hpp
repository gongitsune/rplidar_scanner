#pragma once

#include "lidar.hpp"
#include <lo/lo_cpp.h>

namespace rplidar_scanner {
class OscSender {
public:
  OscSender(const lo::string_type &ip, const lo::num_string_type &port);

  void send_scan_data(rplidar_scanner::scan_node_t *nodes, size_t count);

private:
  lo::Address address_;
};
} // namespace rplidar_scanner

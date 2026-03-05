#include "lidar.hpp"
#include "lo/lo_cpp.h"
#include "lo/lo_types.h"
#include <lo/lo.h>
#include <osc.hpp>

namespace rplidar_scanner {

OscSender::OscSender(const lo::string_type &ip, const lo::num_string_type &port)
    : address_(ip, port) {}

void OscSender::send_scan_data(rplidar_scanner::scan_node_t *nodes,
                               size_t count) {
  lo::Blob blob(count * sizeof(rplidar_scanner::scan_node_t), nodes);
  this->address_.send("/scan", "b", (lo_blob)blob);
}

} // namespace rplidar_scanner

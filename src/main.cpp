#include "config.hpp"
#include "lidar.hpp"
#include "osc.hpp"
#include <cstddef>
#include <sl_lidar.h>
#include <sl_lidar_cmd.h>
#include <sl_lidar_driver.h>
#include <toml++/toml.hpp>

int main() {
  rplidar_scanner::Config config("config.toml");
  rplidar_scanner::Driver driver;
  rplidar_scanner::OscSender sender(config.getOscIp(), config.getOscPort());

  if (!driver.connect(config.getPort(), config.getBaudrate())) {
    return 1;
  }
  driver.health_check();

  rplidar_scanner::scan_node_t nodes[8192];
  size_t count = sizeof(nodes) / sizeof(nodes[0]);

  driver.start_scan();

  if (driver.fetch_scan_data(nodes, count)) {
    sender.send_scan_data(nodes, count);
  }

  return 0;
}

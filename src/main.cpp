#include "config.hpp"
#include "lidar.hpp"
#include "log.hpp"
#include "osc.hpp"
#include "ui.hpp"
#include <atomic>
#include <cstddef>
#include <sl_lidar.h>
#include <sl_lidar_cmd.h>
#include <sl_lidar_driver.h>
#include <toml++/toml.hpp>

std::atomic_bool running(true);

int lidar_job(rplidar_scanner::Config &config) {
  rplidar_scanner::Driver driver;
  rplidar_scanner::OscSender sender(config.osc_host, config.osc_port);

  if (!driver.connect(config.port, config.baudrate)) {
    return 1;
  }
  driver.health_check();
  driver.start_scan();

  rplidar_scanner::scan_node_t nodes[8192];

  while (running) {
    size_t count = sizeof(nodes) / sizeof(nodes[0]);
    if (driver.fetch_scan_data(nodes, count)) {
      sender.send_scan_data(nodes, count);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  return 0;
}

int main() {
  rplidar_scanner::Log log;
  rplidar_scanner::Config config("config.toml");

  std::thread lidar_thread(lidar_job, std::ref(config));

  // print time
  rplidar_scanner::UI ui(config, log);
  running = false;

  if (lidar_thread.joinable()) {
    lidar_thread.join();
  }
  return 0;
}

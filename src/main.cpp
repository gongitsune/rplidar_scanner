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

int lidar_job(rplidar_scanner::Config &config,
              rplidar_scanner::LidarSensor &sensor) {
  rplidar_scanner::OscSender sender(config.osc_host, config.osc_port);

  if (!sensor.connect(config.port, config.baudrate)) {
    return 1;
  }
  sensor.health_check();
  sensor.start_scan();

  while (running) {
    auto data = sensor.fetch_scan_data();
    if (data) {
      auto val = data.value();
      sender.send_scan_data(val->nodes, val->count);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  return 0;
}

int main() {
  rplidar_scanner::Log log;
  rplidar_scanner::Config config("config.toml");
  rplidar_scanner::LidarSensor sensor;

  std::thread lidar_thread(lidar_job, std::ref(config), std::ref(sensor));

  // print time
  rplidar_scanner::UI ui(config, log, sensor);
  running = false;

  if (lidar_thread.joinable()) {
    lidar_thread.join();
  }
  return 0;
}

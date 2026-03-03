#include "config.hpp"
#include "lidar.hpp"
#include <memory>
#include <sl_lidar.h>
#include <sl_lidar_cmd.h>
#include <sl_lidar_driver.h>
#include <toml++/toml.hpp>

int main() {
  auto config = rplidar_scanner::Config("config.toml");
  auto driver = std::make_unique<rplidar_scanner::Driver>();

  driver->connect(config.getPort(), config.getBaudrate());
  driver->health_check();

  return 0;
}

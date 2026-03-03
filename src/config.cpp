#include "toml++/toml.hpp"
#include <config.hpp>

namespace rplidar_scanner {
Config::Config(const std::string_view file_path) {
  auto file = toml::parse_file(file_path);

  this->port = *file["config"]["serial_port"].value<std::string>();
  this->baudrate = *file["config"]["baudrate"].value<int>();
}
} // namespace rplidar_scanner

#pragma once

#include <string>
#include <string_view>

namespace rplidar_scanner {

class Config {
public:
  Config(const std::string_view file_path);
  ~Config();

  void save() const;

  std::string port = "";
  int baudrate{115200};
  std::string osc_host = "localhost";
  int osc_port{9000};

private:
  std::string file_path_ = "config.toml";
};

} // namespace rplidar_scanner

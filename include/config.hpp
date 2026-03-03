#pragma once

#include <string>
#include <string_view>

namespace rplidar_scanner {

class Config {
public:
  Config(const std::string_view file_path);

  std::string getPort() const { return port; }
  int getBaudrate() const { return baudrate; }

private:
  std::string port = "";
  int baudrate{115200};
};

} // namespace rplidar_scanner

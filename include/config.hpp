#pragma once

#include <string>
#include <string_view>

namespace rplidar_scanner {

class Config {
public:
  Config(const std::string_view file_path);

  std::string getPort() const { return port; }
  int getBaudrate() const { return baudrate; }
  std::string getOscIp() const { return osc_host; }
  int getOscPort() const { return osc_port; }

private:
  std::string port = "";
  int baudrate{115200};
  std::string osc_host = "localhost";
  int osc_port{9000};
};

} // namespace rplidar_scanner

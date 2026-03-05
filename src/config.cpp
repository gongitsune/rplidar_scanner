#include "toml++/toml.hpp"
#include <config.hpp>
#include <iostream>

namespace rplidar_scanner {
Config::Config(const std::string_view file_path) : file_path_(file_path) {
  auto file = toml::parse_file(file_path);

  this->port = *file["config"]["serial_port"].value<std::string>();
  this->baudrate = *file["config"]["baudrate"].value<int>();
  this->osc_host = *file["config"]["osc_host"].value<std::string>();
  this->osc_port = *file["config"]["osc_port"].value<int>();
}

Config::~Config() {
  try {
    this->save();
  } catch (const std::exception &e) {
    std::cerr << "Error saving config: " << e.what() << std::endl;
  }
}

void Config::save() const {
  toml::table file = toml::parse_file(this->file_path_);
  auto data = toml::table{
      {"serial_port", this->port},
      {"baudrate", this->baudrate},
      {"osc_host", this->osc_host},
      {"osc_port", this->osc_port},
  };
  file.insert_or_assign("config", data);

  std::ofstream ofs(file_path_.data());
  ofs << file;
}
} // namespace rplidar_scanner

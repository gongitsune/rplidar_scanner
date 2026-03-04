#include <iostream>
#include <lidar.hpp>
#include <memory>
#include <sl_lidar_cmd.h>
#include <sl_lidar_driver.h>
#include <sl_types.h>

namespace rplidar_scanner {
Driver::Driver() {
  auto driver = sl::createLidarDriver();
  if (SL_IS_FAIL(driver.err)) {
    throw std::runtime_error("Failed to create LIDAR driver");
  }
  this->driver = std::unique_ptr<sl::ILidarDriver>(*driver);
}

Driver::~Driver() {
  if (SL_IS_OK(this->connect_result)) {
    this->driver->setMotorSpeed(0);
    this->driver->stop();
    this->driver->disconnect();
  }
}

bool Driver::connect(const std::string &port, int baudrate) {
  auto channel = sl::createSerialPortChannel(port, baudrate);
  if (SL_IS_FAIL(channel.err)) {
    std::cerr << "Failed to create serial channel: " << channel.err
              << std::endl;
    return false;
  }

  this->connect_result = this->driver->connect(*channel);
  if (SL_IS_FAIL(this->connect_result)) {
    std::cerr << "Failed to connect to LIDAR" << std::endl;
    return false;
  }

  sl_lidar_response_device_info_t dev_info;
  this->connect_result = this->driver->getDeviceInfo(dev_info);
  if (SL_IS_FAIL(this->connect_result)) {
    std::cerr << "Failed to get LIDAR device info" << std::endl;
    return false;
  }

  std::cout << "SLAMTEC LIDAR S/N: ";
  for (int i = 0; i < 16; ++i)
    std::cout << std::hex << (int)dev_info.serialnum[i];
  std::cout << std::dec << std::endl;

  std::cout << "Firmware Ver: " << (dev_info.firmware_version >> 8) << "."
            << (dev_info.firmware_version & 0xFF) << std::endl;
  std::cout << "Hardware Rev: " << (int)dev_info.hardware_version << std::endl;
  return true;
}

bool Driver::health_check() {
  sl_result op_result;
  sl_lidar_response_device_health_t health_info;

  op_result = this->driver->getHealth(health_info);
  if (SL_IS_OK(op_result)) {
    std::cout << "LIDAR health status: " << health_info.status << std::endl;
    if (health_info.status == SL_LIDAR_STATUS_ERROR) {
      std::cerr << "LIDAR is in error state!" << std::endl;
      return false;
    } else {
      std::cout << "LIDAR is healthy." << std::endl;
      return true;
    }
  } else {
    std::cerr << "Failed to get LIDAR health status: " << op_result
              << std::endl;
    return false;
  }
}

void Driver::start_scan() {
  this->driver->setMotorSpeed();
  this->driver->startScan(0, 1);
}

bool Driver::fetch_scan_data(scan_node_t *nodes, size_t &count) {
  auto op_result = this->driver->grabScanDataHq(nodes, count);
  if (SL_IS_FAIL(op_result)) {
    std::cerr << "Failed to grab scan data: " << op_result << std::endl;
    return false;
  }
  op_result = this->driver->ascendScanData(nodes, count);
  if (SL_IS_FAIL(op_result)) {
    std::cerr << "Failed to ascend scan data: " << op_result << std::endl;
    return false;
  }

  return true;
}
} // namespace rplidar_scanner

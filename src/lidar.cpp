#include "spdlog/spdlog.h"
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
  this->driver_ = std::unique_ptr<sl::ILidarDriver>(*driver);
}

Driver::~Driver() {
  if (SL_IS_OK(this->connect_result_)) {
    this->driver_->setMotorSpeed(0);
    this->driver_->stop();
    this->driver_->disconnect();
  }
}

bool Driver::connect(const std::string &port, int baudrate) {
  auto channel = sl::createSerialPortChannel(port, baudrate);
  if (SL_IS_FAIL(channel.err)) {
    spdlog::error("Failed to create serial channel: {}", channel.err);
    return false;
  }

  this->connect_result_ = this->driver_->connect(*channel);
  if (SL_IS_FAIL(this->connect_result_)) {
    spdlog::error("Failed to connect to LIDAR: {}", this->connect_result_);
    return false;
  }

  sl_lidar_response_device_info_t dev_info;
  this->connect_result_ = this->driver_->getDeviceInfo(dev_info);
  if (SL_IS_FAIL(this->connect_result_)) {
    spdlog::error("Failed to get LIDAR device info: {}", this->connect_result_);
    return false;
  }

  spdlog::info("Firmware Ver: {}.{}", (dev_info.firmware_version >> 8),
               (dev_info.firmware_version & 0xFF));
  spdlog::info("Hardware Rev: {}", (int)dev_info.hardware_version);
  return true;
}

bool Driver::health_check() {
  sl_result op_result;
  sl_lidar_response_device_health_t health_info;

  op_result = this->driver_->getHealth(health_info);
  if (SL_IS_OK(op_result)) {
    spdlog::info("LIDAR health status: {}", health_info.status);
    if (health_info.status == SL_LIDAR_STATUS_ERROR) {
      spdlog::error("LIDAR is in error state!");
      return false;
    } else {
      spdlog::info("LIDAR is healthy.");
      return true;
    }
  } else {
    spdlog::error("Failed to get LIDAR health status: {}", op_result);
    return false;
  }
}

void Driver::start_scan() {
  this->driver_->setMotorSpeed();
  this->driver_->startScan(0, 1);
}

bool Driver::fetch_scan_data(scan_node_t *nodes, size_t &count) {
  auto op_result = this->driver_->grabScanDataHq(nodes, count);
  if (SL_IS_FAIL(op_result)) {
    spdlog::error("Failed to grab scan data: {}", op_result);
    return false;
  }
  op_result = this->driver_->ascendScanData(nodes, count);
  if (SL_IS_FAIL(op_result)) {
    spdlog::error("Failed to ascend scan data: {}", op_result);
    return false;
  }

  return true;
}
} // namespace rplidar_scanner

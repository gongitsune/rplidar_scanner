#pragma once

#include "spdlog/common.h"
#include "spdlog/logger.h"
#include <memory>
#include <spdlog/sinks/ringbuffer_sink.h>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

namespace rplidar_scanner {
class Log {
public:
  Log();

  std::vector<std::string> get_recent_logs(int count) {
    return ring_buffer_sink_->last_formatted(count);
  }
  std::vector<std::string> get_recent_logs() {
    return ring_buffer_sink_->last_formatted();
  }

private:
  std::vector<spdlog::sink_ptr> sinks_;
  std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt> ring_buffer_sink_;
  std::shared_ptr<spdlog::logger> logger_;
};
} // namespace rplidar_scanner

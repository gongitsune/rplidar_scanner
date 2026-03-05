#include "spdlog/spdlog.h"
#include <log.hpp>

namespace rplidar_scanner {

Log::Log() {
  ring_buffer_sink_ = std::make_shared<spdlog::sinks::ringbuffer_sink_mt>(64);
  sinks_.push_back(ring_buffer_sink_);

  logger_ = std::make_shared<spdlog::logger>("rplidar_scanner", sinks_.begin(),
                                             sinks_.end());
  spdlog::set_default_logger(logger_);
}

} // namespace rplidar_scanner

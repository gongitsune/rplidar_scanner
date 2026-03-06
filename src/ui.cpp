#include "ftxui/dom/elements.hpp"
#include <cmath>
#include <format>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <string>
#include <ui.hpp>
#include <vector>

namespace rplidar_scanner {

UI::UI(rplidar_scanner::Config &config, rplidar_scanner::Log &log,
       rplidar_scanner::LidarSensor &sensor) {
  using namespace ftxui;

  auto screen = App::Fullscreen();

  // Tab labels
  std::vector<std::string> tab_labels = {"Dashboard", "Config"};
  int tab_selected = 0;
  auto tab_toggle = Toggle(&tab_labels, &tab_selected);

  // Dashboard content
  auto log_output = Renderer([&] {
    auto logs = log.get_recent_logs(10);
    auto elems = std::vector<Element>{};
    elems.push_back(text("Recent Logs:"));
    for (const auto &entry : logs) {
      elems.push_back(paragraph(entry));
    }

    return vbox(std::move(elems));
  });

  float max_distance = 0.f;
  float line_angle = 0.f;
  auto dashboard = Renderer([&] {
    auto lidar_canvas = Canvas(100, 100);
    auto low_quality_cnt = 0;

    auto data = sensor.get_scan_data();
    if (data) {
      auto scan_data = data.value();

      for (std::size_t i = 0; i < scan_data->count; ++i) {
        auto &node = scan_data->nodes[i];
        float distance = node.dist_mm_q2 / 4.0f;
        if (distance > max_distance) {
          max_distance = distance;
        }
      }

      lidar_canvas.DrawPointLine(
          50.0f, 50.0f, 50.0f + 50.0f * std::cos(line_angle),
          50.0f + 50.0f * std::sin(line_angle), Color::Red);
      line_angle = std::fmod(line_angle + 0.1f, M_PI * 2.f);

      for (std::size_t i = 0; i < scan_data->count; ++i) {
        auto &node = scan_data->nodes[i];
        float angle = node.angle_z_q14 * 90.0f / 16384.0f;
        float distance = node.dist_mm_q2 / 4.0f;
        int quality = node.quality >> SL_LIDAR_RESP_MEASUREMENT_QUALITY_SHIFT;
        if (quality == 0) {
          low_quality_cnt++;
          continue;
        }

        distance = distance / max_distance * 50.0f; // scale to fit canvas
        float x = distance * std::cos(angle * M_PI / 180.0f) + 50.0f;
        float y = distance * std::sin(angle * M_PI / 180.0f) + 50.0f;
        lidar_canvas.DrawPoint(x, y, Color::Green);
      }
    }
    return vbox({
        text(std::format("Max Distance: {:.2f} mm, Low Quality: {}",
                         max_distance, low_quality_cnt)),
        canvas(lidar_canvas) | border | center,
        separator(),
        log_output->Render(),
    });
  });

  // Config inputs
  std::string seial_port = config.port;
  std::string baudrate = std::to_string(config.baudrate);
  std::string osc_host = config.osc_host;
  std::string osc_port = std::to_string(config.osc_port);

  auto serial_port_input = Input(&seial_port, "Serial Port");
  auto baudrate_input = Input(&baudrate, "Baudrate");
  auto osc_host_input = Input(&osc_host, "OSC Host");
  auto osc_port_input = Input(&osc_port, "OSC Port");

  auto save_button = Button("Save", [&] {
    config.port = seial_port;
    config.baudrate = std::stoi(baudrate);
    config.osc_host = osc_host;
    config.osc_port = std::stoi(osc_port);
    config.save();
  });

  auto input_container = Container::Vertical({
      serial_port_input,
      baudrate_input,
      osc_host_input,
      osc_port_input,
      save_button,
  });

  auto config_ui = Renderer(input_container, [&] {
    return vbox({
        hbox(text("Serial Port: "), serial_port_input->Render()),
        hbox(text("Baudrate: "), baudrate_input->Render()),
        hbox(text("OSC Host: "), osc_host_input->Render()),
        hbox(text("OSC Port: "), osc_port_input->Render()),
        save_button->Render(),
    });
  });

  auto tab_container = Container::Tab({dashboard, config_ui}, &tab_selected);

  auto root_container = Container::Vertical({
      tab_toggle,
      tab_container,
  });

  auto renderer = Renderer(root_container, [&] {
    return vbox({
               tab_toggle->Render(),
               separator(),
               tab_container->Render(),
           }) |
           border;
  });
  auto event_handler = CatchEvent(renderer, [&](Event event) {
    if (event == Event::Character('q')) {
      screen.Exit();
      return true;
    }
    return false;
  });

  std::atomic<bool> refresh_ui_continue = true;
  std::thread refresh_ui([&] {
    while (refresh_ui_continue) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(0.05s);

      screen.PostEvent(Event::Custom);
    }
  });
  screen.Loop(event_handler);

  refresh_ui_continue = false;
  if (refresh_ui.joinable()) {
    refresh_ui.join();
  }
}
} // namespace rplidar_scanner

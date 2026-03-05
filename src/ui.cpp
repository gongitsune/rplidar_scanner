#include "ftxui/dom/elements.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <string>
#include <ui.hpp>
#include <vector>

namespace rplidar_scanner {

UI::UI(rplidar_scanner::Config &config, rplidar_scanner::Log &log) {
  using namespace ftxui;

  auto screen = App::Fullscreen();

  // Tab labels
  std::vector<std::string> tab_labels = {"Dashboard", "Config"};
  int tab_selected = 0;
  auto tab_toggle = Toggle(&tab_labels, &tab_selected);

  // Dashboard content
  auto lidar_canvas = Canvas(100, 100);
  auto log_output = Renderer([&] {
    auto logs = log.get_recent_logs(10);
    auto elems = std::vector<Element>{};
    elems.push_back(text("Recent Logs:"));
    for (const auto &entry : logs) {
      elems.push_back(paragraph(entry));
    }

    return vbox(std::move(elems));
  });
  auto dashboard = Renderer([&] {
    return vbox({
        text("LIDAR Canvas: "),
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

  screen.Loop(event_handler);
}
} // namespace rplidar_scanner

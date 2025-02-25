#pragma once

#include "esphome/core/component.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace sht4x_custom {

class SHT4XTextSensor : public Component, public text_sensor::TextSensor {
 public:
  /** 
   * Dump configuration for debugging.
   */
  void dump_config() override;

  /**
   * Called by the SHT4x component when a new serial number is read.
   * @param serial The serial number as a string.
   */
  void on_serial(const std::string &serial) {
    publish_state(serial);
  }
};

}  // namespace sht4x_custom
}  // namespace esphome
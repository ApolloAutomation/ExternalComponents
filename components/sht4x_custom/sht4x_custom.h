#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/sensirion_common/i2c_sensirion.h"

namespace esphome {
namespace sht4x_custom {

class SHT4XCustomComponent : public PollingComponent, public sensirion_common::SensirionI2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  void update() override;

  // Numeric sensors
  void set_temp_sensor(sensor::Sensor *temp_sensor) { temp_sensor_ = temp_sensor; }
  void set_humidity_sensor(sensor::Sensor *h_sensor) { humidity_sensor_ = h_sensor; }

  // Text sensor
  void set_serial_text_sensor(text_sensor::TextSensor *t_sensor) { serial_text_sensor_ = t_sensor; }

 protected:
  sensor::Sensor *temp_sensor_{nullptr};
  sensor::Sensor *humidity_sensor_{nullptr};

  // text sensor for serial
  text_sensor::TextSensor *serial_text_sensor_{nullptr};

  void read_serial_();
};

}  // namespace sht4x_custom
}  // namespace esphome
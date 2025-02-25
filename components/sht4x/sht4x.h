#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/sensirion_common/i2c_sensirion.h"

namespace esphome {
namespace sht4x {

enum SHT4XPRECISION { SHT4X_PRECISION_HIGH = 0, SHT4X_PRECISION_MED, SHT4X_PRECISION_LOW };
enum SHT4XHEATERPOWER { SHT4X_HEATERPOWER_HIGH, SHT4X_HEATERPOWER_MED, SHT4X_HEATERPOWER_LOW };
enum SHT4XHEATERTIME { SHT4X_HEATERTIME_LONG = 1100, SHT4X_HEATERTIME_SHORT = 110 };

class SHT4XComponent : public PollingComponent, public sensirion_common::SensirionI2CDevice {
 public:
  float get_setup_priority() const override { return setup_priority::DATA; }
  void setup() override;
  void dump_config() override;
  void update() override;

  void set_precision_value(SHT4XPRECISION precision) { precision_ = precision; }
  void set_heater_power_value(SHT4XHEATERPOWER heater_power) { heater_power_ = heater_power; }
  void set_heater_time_value(SHT4XHEATERTIME heater_time) { heater_time_ = heater_time; }
  void set_heater_duty_value(float duty_cycle) { duty_cycle_ = duty_cycle; }

  void set_temp_sensor(sensor::Sensor *temp_sensor) { temp_sensor_ = temp_sensor; }
  void set_humidity_sensor(sensor::Sensor *humidity_sensor) { humidity_sensor_ = humidity_sensor; }

  // NEW: set_serial_sensor
  void set_serial_sensor(sensor::Sensor *serial_sensor) { serial_sensor_ = serial_sensor; }

 protected:
  SHT4XPRECISION precision_;
  SHT4XHEATERPOWER heater_power_;
  SHT4XHEATERTIME heater_time_;
  float duty_cycle_{0.0f};
  uint8_t heater_command_{0};

  sensor::Sensor *temp_sensor_{nullptr};
  sensor::Sensor *humidity_sensor_{nullptr};

  // optional sensor for the serial
  sensor::Sensor *serial_sensor_{nullptr};

  void start_heater_();
  void read_serial_();  // read 0x89, log & publish
};

}  // namespace sht4x
}  // namespace esphome
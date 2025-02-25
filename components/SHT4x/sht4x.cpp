#include "sht4x.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sht4x {

static const char *const TAG = "sht4x";

// For T/RH measurement
static const uint8_t MEASURECOMMANDS[] = {0xFD, 0xF6, 0xE0};
// For read serial: 0x89
static const uint8_t READ_SERIAL_CMD = 0x89;

void SHT4XComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up SHT4x...");

  // Read and log the serial once
  this->read_serial_();

  // Setup optional heater logic
  if (this->duty_cycle_ > 0.0f) {
    uint32_t heater_interval = (uint32_t)(this->heater_time_ / this->duty_cycle_);
    ESP_LOGD(TAG, "Heater interval: %u", heater_interval);

    if (this->heater_power_ == SHT4X_HEATERPOWER_HIGH) {
      if (this->heater_time_ == SHT4X_HEATERTIME_LONG) {
        this->heater_command_ = 0x39;
      } else {
        this->heater_command_ = 0x32;
      }
    } else if (this->heater_power_ == SHT4X_HEATERPOWER_MED) {
      if (this->heater_time_ == SHT4X_HEATERTIME_LONG) {
        this->heater_command_ = 0x2F;
      } else {
        this->heater_command_ = 0x24;
      }
    } else {
      if (this->heater_time_ == SHT4X_HEATERTIME_LONG) {
        this->heater_command_ = 0x1E;
      } else {
        this->heater_command_ = 0x15;
      }
    }

    ESP_LOGD(TAG, "Heater command: 0x%02X", this->heater_command_);
    this->set_interval(heater_interval, [this]() { this->start_heater_(); });
  }
}

void SHT4XComponent::dump_config() {
  LOG_I2C_DEVICE(this);
}

void SHT4XComponent::update() {
  // Send measure command
  this->write_command(MEASURECOMMANDS[this->precision_]);

  // Wait ~10ms for measurement
  this->set_timeout(10, [this]() {
    uint16_t buffer[2];
    if (!this->read_data(buffer, 2)) {
      ESP_LOGW(TAG, "Sensor read failed");
      return;
    }

    // buffer[0] = raw temp, buffer[1] = raw RH
    if (this->temp_sensor_ != nullptr) {
      float raw_temp = buffer[0];
      float temp_c = -45.0f + 175.0f * raw_temp / 65535.0f;
      this->temp_sensor_->publish_state(temp_c);
    }

    if (this->humidity_sensor_ != nullptr) {
      float raw_rh = buffer[1];
      float rh = -6.0f + 125.0f * raw_rh / 65535.0f;
      this->humidity_sensor_->publish_state(rh);
    }
  });
}

void SHT4XComponent::start_heater_() {
  uint8_t cmd[] = {this->heater_command_};
  ESP_LOGD(TAG, "Heater turning on (cmd=0x%02X)", cmd[0]);
  this->write(cmd, 1);
}

void SHT4XComponent::read_serial_() {
  // 1) send 0x89
  this->write_command(READ_SERIAL_CMD);

  // 2) short delay
  delay(5);

  // 3) read 6 bytes => 2 words + 2 CRC
  uint16_t buffer[2];
  if (!this->read_data(buffer, 2)) {
    ESP_LOGE(TAG, "Error reading SHT4x serial number");
    return;
  }

  // 4) combine into 32-bit
  uint32_t serial = ((uint32_t)buffer[0] << 16) | buffer[1];

  ESP_LOGCONFIG(TAG, "SHT4x Serial Number: 0x%08X", serial);

  // 5) publish if user configured the sensor
  if (this->serial_sensor_ != nullptr) {
    // Cast to float. May lose some bits for large serials.
    this->serial_sensor_->publish_state((float) serial);
  }
}

}  // namespace sht4x
}  // namespace esphome
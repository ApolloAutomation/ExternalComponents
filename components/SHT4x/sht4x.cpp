#include "sht4x.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sht4x {

static const char *const TAG = "sht4x";

// Existing measure commands (for temperature/humidity)
static const uint8_t MEASURECOMMANDS[] = {0xFD, 0xF6, 0xE0};

// New command for reading serial
static const uint8_t READ_SERIAL_CMD = 0x89;

void SHT4XComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up sht4x...");

  // 1) Optionally read and log the sensor's serial number
  this->read_serial_();

  // 2) Heater logic setup as before
  if (this->duty_cycle_ > 0.0) {
    uint32_t heater_interval = (uint32_t)(this->heater_time_ / this->duty_cycle_);
    ESP_LOGD(TAG, "Heater interval: %" PRIu32, heater_interval);

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

    ESP_LOGD(TAG, "Heater command: %x", this->heater_command_);

    // Schedule heater intervals
    this->set_interval(heater_interval, std::bind(&SHT4XComponent::start_heater_, this));
  }
}

void SHT4XComponent::dump_config() {
  LOG_I2C_DEVICE(this);
}

void SHT4XComponent::update() {
  // Send the appropriate measure command based on precision setting
  this->write_command(MEASURECOMMANDS[this->precision_]);

  // Use set_timeout() to wait ~10ms for measurement to complete
  this->set_timeout(10, [this]() {
    uint16_t buffer[2];

    // Read T/RH
    bool read_status = this->read_data(buffer, 2);
    if (!read_status) {
      ESP_LOGD(TAG, "Sensor read failed");
      return;
    }

    // Convert raw data to temperature
    if (this->temp_sensor_ != nullptr) {
      float sensor_value_temp = buffer[0];
      float temp = -45.0f + 175.0f * sensor_value_temp / 65535.0f;
      this->temp_sensor_->publish_state(temp);
    }

    // Convert raw data to RH
    if (this->humidity_sensor_ != nullptr) {
      float sensor_value_rh = buffer[1];
      float rh = -6.0f + 125.0f * sensor_value_rh / 65535.0f;
      this->humidity_sensor_->publish_state(rh);
    }
  });
}

void SHT4XComponent::start_heater_() {
  uint8_t cmd[] = {static_cast<uint8_t>(this->heater_command_)};

  ESP_LOGD(TAG, "Heater turning on");
  this->write(cmd, 1);
}

/**
 *  read_serial_()
 *  Sends command 0x89, reads 6 bytes (two 16-bit words + CRC).
 *  Then logs the 32-bit serial number in hex.
 */
void SHT4XComponent::read_serial_() {
  // 1) Write command
  this->write_command(READ_SERIAL_CMD);

  // 2) Short delay (2ms) to allow sensor to respond
  delay(2);

  // 3) We expect two 16-bit words + 2 CRC bytes => 6 bytes total
  //    Under the hood, read_data(buffer, 2) reads 2 words,
  //    verifying CRC automatically in the driver code.
  uint16_t buffer[2];
  bool status = this->read_data(buffer, 2);
  if (!status) {
    ESP_LOGE(TAG, "Error reading SHT4x serial number");
    return;
  }

  // 4) Combine the two 16-bit words into a 32-bit serial
  uint32_t serial = ((uint32_t)buffer[0] << 16) | buffer[1];

  // 5) Log it
  ESP_LOGI(TAG, "SHT4x Serial Number: 0x%08X", serial);
}

}  // namespace sht4x
}  // namespace esphome
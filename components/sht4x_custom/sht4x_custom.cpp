#include "sht4x.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sht4x {

static const char *const TAG = "sht4x";

static const uint8_t MEASURECOMMANDS[] = {0xFD, 0xF6, 0xE0};
static const uint8_t READ_SERIAL_CMD = 0x89;

void SHT4XComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up SHT4x...");

  // If you still want a single immediate attempt, you can do it here:
  // this->try_read_serial_();

  // Now, schedule repeated attempts every 10s:
  this->set_interval(10000, [this]() { 
    this->try_read_serial_(); 
  });

  // Existing heater or other logic...
  if (this->duty_cycle_ > 0.0f) {
    // ...
  }
}

void SHT4XComponent::dump_config() {
  LOG_I2C_DEVICE(this);
}

void SHT4XComponent::update() {
  // Normal T/RH measurement code
  this->write_command(MEASURECOMMANDS[this->precision_]);

  this->set_timeout(10, [this]() {
    uint16_t buffer[2];
    if (!this->read_data(buffer, 2)) {
      ESP_LOGW(TAG, "Sensor read for T/RH failed");
      return;
    }
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
  // ...
}

/** 
 * Called every 10s. Tries to read serial with 0x89 command.
 */
void SHT4XComponent::try_read_serial_() {
  // 1) Send command
  this->write_command(READ_SERIAL_CMD);

  // 2) Wait asynchronously ~5ms before reading
  this->set_timeout(5, [this]() {
    uint16_t buffer[2];
    if (!this->read_data(buffer, 2)) {
      ESP_LOGE(TAG, "Error reading SHT4x serial number");
      return;
    }

    uint32_t serial = ((uint32_t) buffer[0] << 16) | buffer[1];
    ESP_LOGI(TAG, "SHT4x Serial Number: 0x%08X", serial);

    // If you have a serial_number sensor, publish here
    if (this->serial_sensor_ != nullptr) {
      this->serial_sensor_->publish_state((float) serial);
    }
  });
}

}  // namespace sht4x
}  // namespace esphome
#include "sht4x_custom.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sht4x_custom {

static const char *const TAG = "sht4x_custom";

static const uint8_t MEASURE_CMD = 0xFD;
static const uint8_t SERIAL_CMD  = 0x89;

void SHT4XCustomComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up SHT4x Custom...");
  // Optionally read serial once
  this->read_serial_();
}

void SHT4XCustomComponent::dump_config() {
  LOG_I2C_DEVICE(this);
}

void SHT4XCustomComponent::update() {
  // 1) Send measure T/H command
  this->write_command(MEASURE_CMD);

  // 2) Wait ~10ms, read 2 words
  this->set_timeout(10, [this]() {
    uint16_t buf[2];
    bool ok = this->read_data(buf, 2);
    if (!ok) {
      ESP_LOGW(TAG, "Temperature/Humidity read failed");
      return;
    }
    // Convert raw to temp/hum
    if (this->temp_sensor_ != nullptr) {
      float raw_t = buf[0];
      float temp_c = -45.0f + 175.0f * raw_t / 65535.0f;
      this->temp_sensor_->publish_state(temp_c);
    }
    if (this->humidity_sensor_ != nullptr) {
      float raw_h = buf[1];
      float rh = -6.0f + 125.0f * raw_h / 65535.0f;
      this->humidity_sensor_->publish_state(rh);
    }
  });
}

void SHT4XCustomComponent::read_serial_() {
  // Write 0x89
  this->write_command(SERIAL_CMD);

  // Wait ~5ms, read two 16-bit words + 2 CRC
  this->set_timeout(5, [this]() {
    uint16_t buf[2];
    bool ok = this->read_data(buf, 2);
    if (!ok) {
      ESP_LOGE(TAG, "Error reading SHT4x serial");
      return;
    }

    // Combine 2 words
    uint32_t serial = ((uint32_t)buf[0] << 16) | buf[1];
    ESP_LOGI(TAG, "SHT4x Serial: 0x%08X", serial);

    // If we have a text sensor for the serial, publish
    if (this->serial_text_sensor_ != nullptr) {
      // Format as hex or decimal
      char hex_str[11];
      sprintf(hex_str, "0x%08X", serial);
      this->serial_text_sensor_->publish_state(hex_str);
    }
  });
}

}  // namespace sht4x_custom
}  // namespace esphome
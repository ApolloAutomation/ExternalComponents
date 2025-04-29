// radon_uart_sensor.h
#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include <inttypes.h>

namespace esphome {
namespace radon_uart_sensor {

class RadonUARTSensor : public esphome::PollingComponent, public esphome::uart::UARTDevice {
 public:
  RadonUARTSensor(esphome::uart::UARTComponent *parent) : PollingComponent(10000), UARTDevice(parent) {} // Poll every 10 seconds

  void setup() override {
    ESP_LOGI("radon_uart_sensor", "Initializing Radon UART Sensor");
    
    // Set names for sensors
    running_time_sensor_.set_name("Running Time");
    running_time_sensor_.set_unit_of_measurement("s");
    
    cumulative_radon_sensor_.set_name("Cumulative Radon Concentration");
    cumulative_radon_sensor_.set_unit_of_measurement("Bq/m³");
    
    last10_radon_sensor_.set_name("Last 10 Minutes Radon Concentration");
    last10_radon_sensor_.set_unit_of_measurement("Bq/m³");
    
    last1_radon_sensor_.set_name("Last 1-Hour Radon Concentration");
    last1_radon_sensor_.set_unit_of_measurement("Bq/m³");
    
    last12_radon_sensor_.set_name("Last 12-Hours Radon Concentration");
    last12_radon_sensor_.set_unit_of_measurement("Bq/m³");
    
    last24_radon_sensor_.set_name("Last 24-Hours Radon Concentration");
    last24_radon_sensor_.set_unit_of_measurement("Bq/m³");
    
    last48_radon_sensor_.set_name("Last 48-Hours Radon Concentration");
    last48_radon_sensor_.set_unit_of_measurement("Bq/m³");
    
    last96_radon_sensor_.set_name("Last 96-Hours Radon Concentration");
    last96_radon_sensor_.set_unit_of_measurement("Bq/m³");
    
    status_binary_sensor_.set_name("Radon Sensor Online");
  }

  void update() override {
    // Handle Welcome String
    handle_welcome_string();

    // After Welcome, process data frames
    if (available() >= 28) {
      std::vector<uint8_t> data_frame;
      for (int i = 0; i < 28; ++i) {
        data_frame.push_back(read());
      }

      // Verify CRC
      uint32_t received_crc = (data_frame[24]) |
                               (data_frame[25] << 8) |
                               (data_frame[26] << 16) |
                               (data_frame[27] << 24);
      uint32_t calculated_crc = calculate_crc32(data_frame.data(), 24);
      if (received_crc != calculated_crc) {
        ESP_LOGE("radon_uart_sensor", "CRC mismatch: received 0x%08" PRIx32 ", calculated 0x%08" PRIx32, received_crc, calculated_crc);
        crc_fail_count_++;
        if (crc_fail_count_ >= max_crc_failures_) {
          ESP_LOGE("radon_uart_sensor", "Max CRC failures reached.");
          // Implement corrective actions if necessary (e.g., reset UART, notify Home Assistant)
          crc_fail_count_ = 0;
        }
        return;
      }

      // Reset CRC failure count on success
      crc_fail_count_ = 0;

      // Parse data fields
      parse_data_frame(data_frame);

      // Update sensor online status
      status_binary_sensor_.publish_state(true);
    } else {
      // If data frames are not received within the expected interval, set status to offline
      status_binary_sensor_.publish_state(false);
    }
  }

  // Getter methods for sensors
  esphome::sensor::Sensor *get_running_time_sensor() { return &running_time_sensor_; }
  esphome::sensor::Sensor *get_cumulative_radon_sensor() { return &cumulative_radon_sensor_; }
  esphome::sensor::Sensor *get_last10_radon_sensor() { return &last10_radon_sensor_; }
  esphome::sensor::Sensor *get_last1_radon_sensor() { return &last1_radon_sensor_; }
  esphome::sensor::Sensor *get_last12_radon_sensor() { return &last12_radon_sensor_; }
  esphome::sensor::Sensor *get_last24_radon_sensor() { return &last24_radon_sensor_; }
  esphome::sensor::Sensor *get_last48_radon_sensor() { return &last48_radon_sensor_; }
  esphome::sensor::Sensor *get_last96_radon_sensor() { return &last96_radon_sensor_; }
  esphome::binary_sensor::BinarySensor *get_status_binary_sensor() { return &status_binary_sensor_; } // For online/offline status
  
  // Setter methods for sensors
  void set_running_time_sensor(esphome::sensor::Sensor *sens) { running_time_sensor_ = *sens; }
  void set_cumulative_radon_sensor(esphome::sensor::Sensor *sens) { cumulative_radon_sensor_ = *sens; }
  void set_last10_radon_sensor(esphome::sensor::Sensor *sens) { last10_radon_sensor_ = *sens; }
  void set_last1_radon_sensor(esphome::sensor::Sensor *sens) { last1_radon_sensor_ = *sens; }
  void set_last12_radon_sensor(esphome::sensor::Sensor *sens) { last12_radon_sensor_ = *sens; }
  void set_last24_radon_sensor(esphome::sensor::Sensor *sens) { last24_radon_sensor_ = *sens; }
  void set_last48_radon_sensor(esphome::sensor::Sensor *sens) { last48_radon_sensor_ = *sens; }
  void set_last96_radon_sensor(esphome::sensor::Sensor *sens) { last96_radon_sensor_ = *sens; }
  void set_status_binary_sensor(esphome::binary_sensor::BinarySensor *sens) { status_binary_sensor_ = *sens; }

 protected:
  // Handle the "Welcome" string sent upon power-up
  void handle_welcome_string() {
    while (available()) {
      uint8_t byte = read();
      welcome_buffer_.push_back(byte);

      // Check if "Welcome" string is received
      if (welcome_buffer_.size() >= welcome_string_.length()) {
        std::string received_str(welcome_buffer_.begin(), welcome_buffer_.end());
        if (received_str.find(welcome_string_) != std::string::npos) {
          ESP_LOGI("radon_uart_sensor", "Received Welcome String");
          welcome_buffer_.clear();
          break;
        } else {
          // Not the welcome string, shift buffer to avoid false positives
          welcome_buffer_.erase(welcome_buffer_.begin());
        }
      }
    }
  }

  // Parse the 28-byte data frame
  void parse_data_frame(const std::vector<uint8_t> &data_frame) {
    // CRC check is already done in the update() method
    // The last 4 bytes (24-27) are the CRC-32

    // (1) Running Time: bytes 0-3 (Little-Endian)
    uint32_t running_time = data_frame[0] |
                             (data_frame[1] << 8) |
                             (data_frame[2] << 16) |
                             (data_frame[3] << 24);
    running_time_sensor_.publish_state(static_cast<float>(running_time));

    // (2) Cumulative Radon Concentration Average: bytes 4-7 (Little-Endian)
    uint32_t cumulative_radon = data_frame[4] |
                                 (data_frame[5] << 8) |
                                 (data_frame[6] << 16) |
                                 (data_frame[7] << 24);
    cumulative_radon_sensor_.publish_state(static_cast<float>(cumulative_radon));

    // (3) Last 10 Minutes Radon Concentration: bytes 8-9 (Little-Endian)
    uint16_t last10_radon = data_frame[8] | (data_frame[9] << 8);
    last10_radon_sensor_.publish_state(static_cast<float>(last10_radon));

    // (4) Reserved: bytes 10-11 (No action)

    // (5) Last 1-Hour Radon Concentration: bytes 12-13 (Little-Endian)
    uint16_t last1_radon = data_frame[12] | (data_frame[13] << 8);
    last1_radon_sensor_.publish_state(static_cast<float>(last1_radon));

    // (6) Last 12-Hours Radon Concentration: bytes 14-15 (Little-Endian)
    uint16_t last12_radon = data_frame[14] | (data_frame[15] << 8);
    last12_radon_sensor_.publish_state(static_cast<float>(last12_radon));

    // (7) Last 24-Hours Radon Concentration: bytes 16-17 (Little-Endian)
    uint16_t last24_radon = data_frame[16] | (data_frame[17] << 8);
    last24_radon_sensor_.publish_state(static_cast<float>(last24_radon));

    // (8) Last 48-Hours Radon Concentration: bytes 18-19 (Little-Endian)
    uint16_t last48_radon = data_frame[18] | (data_frame[19] << 8);
    last48_radon_sensor_.publish_state(static_cast<float>(last48_radon));

    // (9) Reserved: bytes 20-21 (No action)

    // (10) Last 96-Hours Radon Concentration: bytes 22-23 (Little-Endian)
    uint16_t last96_radon = data_frame[22] | (data_frame[23] << 8);
    last96_radon_sensor_.publish_state(static_cast<float>(last96_radon));

    // Optional: Log all parsed values
    ESP_LOGI("radon_uart_sensor", "Parsed Data - Running Time: %u s, Cumulative Radon: %.2f Bq/m³, Last10: %.2f Bq/m³, Last1: %.2f Bq/m³, Last12: %.2f Bq/m³, Last24: %.2f Bq/m³, Last48: %.2f Bq/m³, Last96: %.2f Bq/m³",
             running_time,
             cumulative_radon_sensor_.state,
             last10_radon_sensor_.state,
             last1_radon_sensor_.state,
             last12_radon_sensor_.state,
             last24_radon_sensor_.state,
             last48_radon_sensor_.state,
             last96_radon_sensor_.state);
  }

  // Calculate CRC-32 using Ethernet polynomial 0x4C11DB7 (reversed = 0xEDB88320)
  uint32_t calculate_crc32(const uint8_t *data, size_t length) {
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < length; ++i) {
      crc ^= data[i];
      for (int j = 0; j < 8; ++j) {
        if (crc & 1)
          crc = (crc >> 1) ^ 0xEDB88320; // Reversed Ethernet CRC-32 polynomial
        else
          crc = crc >> 1;
      }
    }
    return ~crc;
  }

  // Define sensors
  esphome::sensor::Sensor running_time_sensor_{};
  esphome::sensor::Sensor cumulative_radon_sensor_{};
  esphome::sensor::Sensor last10_radon_sensor_{};
  esphome::sensor::Sensor last1_radon_sensor_{};
  esphome::sensor::Sensor last12_radon_sensor_{};
  esphome::sensor::Sensor last24_radon_sensor_{};
  esphome::sensor::Sensor last48_radon_sensor_{};
  esphome::sensor::Sensor last96_radon_sensor_{};
  
  // Binary sensor for online/offline status
  esphome::binary_sensor::BinarySensor status_binary_sensor_{};

  // Buffers and counters
  std::vector<uint8_t> welcome_buffer_;
  const std::string welcome_string_ = "Welcome";
  int crc_fail_count_ = 0;
  const int max_crc_failures_ = 5;
};

}  // namespace radon_uart_sensor
}  // namespace esphome
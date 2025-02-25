#include "sht4x_custom_text_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sht4x_custom {

static const char *const TAG = "sht4x_custom.text_sensor";

void SHT4XTextSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "SHT4X Custom Serial Text Sensor:");
  LOG_TEXT_SENSOR("", "Serial", this);
}

}  // namespace sht4x_custom
}  // namespace esphome
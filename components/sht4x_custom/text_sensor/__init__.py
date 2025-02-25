import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import CONF_ID, ICON_CHIP

# Use the same namespace as your main sht4x_custom component
sht4x_custom_ns = cg.esphome_ns.namespace("sht4x_custom")

# Declare the SHT4XTextSensor class.
SHT4XTextSensor = sht4x_custom_ns.class_(
    "SHT4XTextSensor", text_sensor.TextSensor, cg.Component
)

CONF_SERIAL_NUMBER = "serial_number"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(SHT4XTextSensor),
    cv.Optional(CONF_SERIAL_NUMBER): text_sensor.text_sensor_schema(
        icon=ICON_CHIP
    ),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    if CONF_SERIAL_NUMBER in config:
        ts = await text_sensor.new_text_sensor(config[CONF_SERIAL_NUMBER])
        cg.add(var.publish_state(ts))  # This is a placeholder. In your main SHT4x component you'll call the text sensor's on_serial.
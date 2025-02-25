import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor, text_sensor, sensirion_common
from esphome.const import (
    CONF_ID,
    CONF_TEMPERATURE,
    CONF_HUMIDITY,
    ICON_THERMOMETER,
    ICON_WATER_PERCENT,
    UNIT_CELSIUS,
    UNIT_PERCENT,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_HUMIDITY,
    STATE_CLASS_MEASUREMENT,
)

# Our custom namespace
sht4x_ns = cg.esphome_ns.namespace("sht4x_custom")

# Our main class in C++
SHT4XCustomComponent = sht4x_ns.class_(
    "SHT4XCustomComponent", 
    cg.PollingComponent,  # or whatever base you like
    sensirion_common.SensirionI2CDevice
)

CONF_SERIAL_NUMBER = "serial_number"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SHT4XCustomComponent),
            cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                icon=ICON_THERMOMETER,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_HUMIDITY): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,
                icon=ICON_WATER_PERCENT,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_HUMIDITY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            # For the serial number, we define a text_sensor schema
            cv.Optional(CONF_SERIAL_NUMBER): text_sensor.text_sensor_schema(),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(0x44))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    # If user provides "temperature", create a numeric sensor
    if CONF_TEMPERATURE in config:
        temp = await sensor.new_sensor(config[CONF_TEMPERATURE])
        cg.add(var.set_temp_sensor(temp))

    # If user provides "humidity", create a numeric sensor
    if CONF_HUMIDITY in config:
        hum = await sensor.new_sensor(config[CONF_HUMIDITY])
        cg.add(var.set_humidity_sensor(hum))

    # If user provides "serial_number", create a text sensor
    if CONF_SERIAL_NUMBER in config:
        sn = await text_sensor.new_text_sensor(config[CONF_SERIAL_NUMBER])
        cg.add(var.set_serial_text_sensor(sn))
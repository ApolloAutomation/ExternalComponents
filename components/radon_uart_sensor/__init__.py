import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor, binary_sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_DURATION,
    DEVICE_CLASS_EMPTY,
    ENTITY_CATEGORY_DIAGNOSTIC,
    STATE_CLASS_MEASUREMENT,
    UNIT_SECOND,
    UNIT_EMPTY,
    ICON_RADIOACTIVE,
    ICON_TIMER,
)

CODEOWNERS = ["@apollo-automation"]
DEPENDENCIES = ["uart"]
AUTO_LOAD = ["sensor", "binary_sensor"]

# Domain for this component
DOMAIN = "radon_uart_sensor"
radon_uart_sensor_ns = cg.esphome_ns.namespace("radon_uart_sensor")
RadonUARTSensor = radon_uart_sensor_ns.class_(
    "RadonUARTSensor", cg.PollingComponent, uart.UARTDevice
)

# Configuration entries
CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(RadonUARTSensor),
            cv.Optional(f"{DOMAIN}_running_time"): sensor.sensor_schema(
                unit_of_measurement=UNIT_SECOND,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_DURATION,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
                icon=ICON_TIMER,
            ),
            cv.Optional(f"{DOMAIN}_cumulative_radon"): sensor.sensor_schema(
                unit_of_measurement="Bq/m³",
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_EMPTY,
                state_class=STATE_CLASS_MEASUREMENT,
                icon=ICON_RADIOACTIVE,
            ),
            cv.Optional(f"{DOMAIN}_last10_radon"): sensor.sensor_schema(
                unit_of_measurement="Bq/m³",
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_EMPTY,
                state_class=STATE_CLASS_MEASUREMENT,
                icon=ICON_RADIOACTIVE,
            ),
            cv.Optional(f"{DOMAIN}_last1_radon"): sensor.sensor_schema(
                unit_of_measurement="Bq/m³",
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_EMPTY,
                state_class=STATE_CLASS_MEASUREMENT,
                icon=ICON_RADIOACTIVE,
            ),
            cv.Optional(f"{DOMAIN}_last12_radon"): sensor.sensor_schema(
                unit_of_measurement="Bq/m³",
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_EMPTY,
                state_class=STATE_CLASS_MEASUREMENT,
                icon=ICON_RADIOACTIVE,
            ),
            cv.Optional(f"{DOMAIN}_last24_radon"): sensor.sensor_schema(
                unit_of_measurement="Bq/m³",
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_EMPTY,
                state_class=STATE_CLASS_MEASUREMENT,
                icon=ICON_RADIOACTIVE,
            ),
            cv.Optional(f"{DOMAIN}_last48_radon"): sensor.sensor_schema(
                unit_of_measurement="Bq/m³",
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_EMPTY,
                state_class=STATE_CLASS_MEASUREMENT,
                icon=ICON_RADIOACTIVE,
            ),
            cv.Optional(f"{DOMAIN}_last96_radon"): sensor.sensor_schema(
                unit_of_measurement="Bq/m³",
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_EMPTY,
                state_class=STATE_CLASS_MEASUREMENT,
                icon=ICON_RADIOACTIVE,
            ),
            cv.Optional(f"{DOMAIN}_status"): binary_sensor.binary_sensor_schema(),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    if f"{DOMAIN}_running_time" in config:
        sens = await sensor.new_sensor(config[f"{DOMAIN}_running_time"])
        cg.add(var.set_running_time_sensor(sens))
    
    if f"{DOMAIN}_cumulative_radon" in config:
        sens = await sensor.new_sensor(config[f"{DOMAIN}_cumulative_radon"])
        cg.add(var.set_cumulative_radon_sensor(sens))
    
    if f"{DOMAIN}_last10_radon" in config:
        sens = await sensor.new_sensor(config[f"{DOMAIN}_last10_radon"])
        cg.add(var.set_last10_radon_sensor(sens))
    
    if f"{DOMAIN}_last1_radon" in config:
        sens = await sensor.new_sensor(config[f"{DOMAIN}_last1_radon"])
        cg.add(var.set_last1_radon_sensor(sens))
    
    if f"{DOMAIN}_last12_radon" in config:
        sens = await sensor.new_sensor(config[f"{DOMAIN}_last12_radon"])
        cg.add(var.set_last12_radon_sensor(sens))
    
    if f"{DOMAIN}_last24_radon" in config:
        sens = await sensor.new_sensor(config[f"{DOMAIN}_last24_radon"])
        cg.add(var.set_last24_radon_sensor(sens))
    
    if f"{DOMAIN}_last48_radon" in config:
        sens = await sensor.new_sensor(config[f"{DOMAIN}_last48_radon"])
        cg.add(var.set_last48_radon_sensor(sens))
    
    if f"{DOMAIN}_last96_radon" in config:
        sens = await sensor.new_sensor(config[f"{DOMAIN}_last96_radon"])
        cg.add(var.set_last96_radon_sensor(sens))
    
    if f"{DOMAIN}_status" in config:
        sens = await binary_sensor.new_binary_sensor(config[f"{DOMAIN}_status"])
        cg.add(var.set_status_binary_sensor(sens))
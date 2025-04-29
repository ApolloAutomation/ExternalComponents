import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, uart
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

from . import (
    DOMAIN,
    radon_uart_sensor_ns,
    RadonUARTSensor,
    CONFIG_SCHEMA as RADON_UART_SCHEMA,
)

# Define dependencies for the sensor platform
DEPENDENCIES = ["radon_uart_sensor"]

# Define the sensor platform schema
# This extends the component's schema defined in __init__.py
SENSOR_SCHEMA = RADON_UART_SCHEMA.extend({})

# Validate and register with ESPHome
CONFIG_SCHEMA = cv.All(SENSOR_SCHEMA)

async def to_code(config):
    """Set up the radon UART sensor component."""
    # Get the component variable that was created in __init__.py
    var = await cg.get_variable(config[CONF_ID])
    
    # Since most of the setup is handled in __init__.py,
    # we only need to register the component as a sensor platform
    
    # Register each sensor entity with their respective functions
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
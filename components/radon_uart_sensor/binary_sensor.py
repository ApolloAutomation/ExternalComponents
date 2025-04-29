import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor, uart
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_CONNECTIVITY,
)

from . import (
    DOMAIN,
    radon_uart_sensor_ns,
    RadonUARTSensor,
    CONFIG_SCHEMA as RADON_UART_SCHEMA,
)

# Define dependencies for the binary sensor platform
DEPENDENCIES = ["radon_uart_sensor"]

# Define the binary sensor platform schema
# This extends the component's schema defined in __init__.py
BINARY_SENSOR_SCHEMA = RADON_UART_SCHEMA.extend({})

# Validate and register with ESPHome
CONFIG_SCHEMA = cv.All(BINARY_SENSOR_SCHEMA)

async def to_code(config):
    """Set up the radon UART binary sensor component."""
    # Get the component variable that was created in __init__.py
    var = await cg.get_variable(config[CONF_ID])
    
    # Since most of the setup is handled in __init__.py,
    # we only need to register the component as a binary sensor platform
    
    # Register the status binary sensor
    if f"{DOMAIN}_status" in config:
        sens = await binary_sensor.new_binary_sensor(config[f"{DOMAIN}_status"])
        cg.add(var.set_status_binary_sensor(sens))
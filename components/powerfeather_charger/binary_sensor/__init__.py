import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import (
    DEVICE_CLASS_POWER,
    ENTITY_CATEGORY_DIAGNOSTIC,
)
from .. import (
    CONF_POWERFEATHER_CHARGER_ID,
    POWERFEATHER_CHARGER_COMPONENT_SCHEMA,
)

CONF_SUPPLY_GOOD_SENSOR = "supply_good"
CONF_SUPPLY_PRESENT_SENSOR = "supply_present"

CONFIG_SCHEMA = POWERFEATHER_CHARGER_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_SUPPLY_GOOD_SENSOR): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_POWER,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_SUPPLY_PRESENT_SENSOR): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_POWER,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    }
)


async def to_code(config):
    charger = await cg.get_variable(config[CONF_POWERFEATHER_CHARGER_ID])

    if cfg := config.get(CONF_SUPPLY_GOOD_SENSOR):
        sens = await binary_sensor.new_binary_sensor(cfg)
        cg.add(charger.set_supply_good_sensor(sens))
    if cfg := config.get(CONF_SUPPLY_PRESENT_SENSOR):
        sens = await binary_sensor.new_binary_sensor(cfg)
        cg.add(charger.set_supply_present_sensor(sens))


import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import ENTITY_CATEGORY_DIAGNOSTIC
from .. import (
    CONF_POWERFEATHER_FUEL_GAUGE_ID,
    POWERFEATHER_FUEL_GAUGE_COMPONENT_SCHEMA,
)

CONF_FUEL_GAUGE_STATUS = "fuel_gauge_status"

CONFIG_SCHEMA = POWERFEATHER_FUEL_GAUGE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_FUEL_GAUGE_STATUS): text_sensor.text_sensor_schema(
            icon="mdi:battery-unknown",
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    }
)


async def to_code(config):
    fuel_gauge = await cg.get_variable(config[CONF_POWERFEATHER_FUEL_GAUGE_ID])

    if cfg := config.get(CONF_FUEL_GAUGE_STATUS):
        sens = await text_sensor.new_text_sensor(cfg)
        cg.add(fuel_gauge.set_status_sensor(sens))

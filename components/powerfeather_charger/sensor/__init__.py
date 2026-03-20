import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    DEVICE_CLASS_VOLTAGE,
    DEVICE_CLASS_CURRENT,
    STATE_CLASS_MEASUREMENT,
    ENTITY_CATEGORY_DIAGNOSTIC,
)
from esphome.components.powerfeather_mainboard import (
    ICON_VOLTAGE,
    ICON_CURRENT_DC,
    UNIT_MILLIAMPERE,
    UNIT_MILLIVOLT,
)
from .. import (
    CONF_POWERFEATHER_CHARGER_ID,
    POWERFEATHER_CHARGER_COMPONENT_SCHEMA,
)

CONF_SUPPLY_VOLTAGE_SENSOR = "supply_voltage"
CONF_SUPPLY_CURRENT_SENSOR = "supply_current"
CONF_BATTERY_VOLTAGE_SENSOR = "battery_voltage"
CONF_BATTERY_CURRENT_SENSOR = "battery_current"

CONFIG_SCHEMA = POWERFEATHER_CHARGER_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_SUPPLY_VOLTAGE_SENSOR): sensor.sensor_schema(
            unit_of_measurement=UNIT_MILLIVOLT,
            icon=ICON_VOLTAGE,
            device_class=DEVICE_CLASS_VOLTAGE,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_SUPPLY_CURRENT_SENSOR): sensor.sensor_schema(
            unit_of_measurement=UNIT_MILLIAMPERE,
            icon=ICON_CURRENT_DC,
            device_class=DEVICE_CLASS_CURRENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_VOLTAGE_SENSOR): sensor.sensor_schema(
            unit_of_measurement=UNIT_MILLIVOLT,
            icon=ICON_VOLTAGE,
            device_class=DEVICE_CLASS_VOLTAGE,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_CURRENT_SENSOR): sensor.sensor_schema(
            unit_of_measurement=UNIT_MILLIAMPERE,
            icon=ICON_CURRENT_DC,
            device_class=DEVICE_CLASS_CURRENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    }
)


async def to_code(config):
    charger = await cg.get_variable(config[CONF_POWERFEATHER_CHARGER_ID])

    if cfg := config.get(CONF_SUPPLY_VOLTAGE_SENSOR):
        sens = await sensor.new_sensor(cfg)
        cg.add(charger.set_supply_voltage_sensor(sens))
    if cfg := config.get(CONF_SUPPLY_CURRENT_SENSOR):
        sens = await sensor.new_sensor(cfg)
        cg.add(charger.set_supply_current_sensor(sens))
    if cfg := config.get(CONF_BATTERY_VOLTAGE_SENSOR):
        sens = await sensor.new_sensor(cfg)
        cg.add(charger.set_battery_voltage_sensor(sens))
    if cfg := config.get(CONF_BATTERY_CURRENT_SENSOR):
        sens = await sensor.new_sensor(cfg)
        cg.add(charger.set_battery_current_sensor(sens))


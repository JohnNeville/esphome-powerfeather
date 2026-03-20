import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    ICON_BATTERY,
    ICON_THERMOMETER,
    ICON_PERCENT,
    ICON_TIMER,
    ICON_EMPTY,
    UNIT_PERCENT,
    UNIT_EMPTY,
    UNIT_CELSIUS,
    DEVICE_CLASS_BATTERY,
    DEVICE_CLASS_DURATION,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    ENTITY_CATEGORY_DIAGNOSTIC,
)
from esphome.components.powerfeather_mainboard import UNIT_MINUTES
from .. import (
    CONF_POWERFEATHER_FUEL_GAUGE_ID,
    POWERFEATHER_FUEL_GAUGE_COMPONENT_SCHEMA,
)

CONF_BATTERY_CHARGE_SENSOR = "battery_charge"
CONF_BATTERY_HEALTH_SENSOR = "battery_health"
CONF_BATTERY_CYCLES_SENSOR = "battery_cycles"
CONF_BATTERY_TIME_LEFT_SENSOR = "battery_time_left"
CONF_BATTERY_TEMPERATURE_SENSOR = "battery_temperature"

CONFIG_SCHEMA = POWERFEATHER_FUEL_GAUGE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_BATTERY_CHARGE_SENSOR): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            icon=ICON_BATTERY,
            device_class=DEVICE_CLASS_BATTERY,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_HEALTH_SENSOR): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            icon=ICON_PERCENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_CYCLES_SENSOR): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_EMPTY,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_TIME_LEFT_SENSOR): sensor.sensor_schema(
            unit_of_measurement=UNIT_MINUTES,
            icon=ICON_TIMER,
            device_class=DEVICE_CLASS_DURATION,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_TEMPERATURE_SENSOR): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_THERMOMETER,
            device_class=DEVICE_CLASS_TEMPERATURE,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    }
)


async def to_code(config):
    fuel_gauge = await cg.get_variable(config[CONF_POWERFEATHER_FUEL_GAUGE_ID])

    if cfg := config.get(CONF_BATTERY_CHARGE_SENSOR):
        sens = await sensor.new_sensor(cfg)
        cg.add(fuel_gauge.set_battery_charge_sensor(sens))
    if cfg := config.get(CONF_BATTERY_HEALTH_SENSOR):
        sens = await sensor.new_sensor(cfg)
        cg.add(fuel_gauge.set_battery_health_sensor(sens))
    if cfg := config.get(CONF_BATTERY_CYCLES_SENSOR):
        sens = await sensor.new_sensor(cfg)
        cg.add(fuel_gauge.set_battery_cycles_sensor(sens))
    if cfg := config.get(CONF_BATTERY_TIME_LEFT_SENSOR):
        sens = await sensor.new_sensor(cfg)
        cg.add(fuel_gauge.set_battery_time_left_sensor(sens))
    if cfg := config.get(CONF_BATTERY_TEMPERATURE_SENSOR):
        sens = await sensor.new_sensor(cfg)
        cg.add(fuel_gauge.set_battery_temperature_sensor(sens))


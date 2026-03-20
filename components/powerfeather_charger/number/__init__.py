import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import (
    DEVICE_CLASS_VOLTAGE,
    DEVICE_CLASS_CURRENT,
    ENTITY_CATEGORY_CONFIG,
)
from esphome.components.powerfeather_mainboard import (
    ICON_VOLTAGE,
    ICON_CURRENT_DC,
    UNIT_MILLIVOLT,
    UNIT_MILLIAMPERE,
)
from .. import (
    CONF_POWERFEATHER_CHARGER_ID,
    POWERFEATHER_CHARGER_COMPONENT_SCHEMA,
    TASK_UPDATE_TYPES,
    powerfeather_charger_ns,
)

# Limits from SDK BQ2562x constants
SUPPLY_MAINTAIN_VOLTAGE_MIN = 4600
SUPPLY_MAINTAIN_VOLTAGE_MAX = 16800
SUPPLY_MAINTAIN_VOLTAGE_STEP = 40   # SDK resolution: 40 mV per step
BATTERY_CHARGING_CURRENT_MIN = 40
BATTERY_CHARGING_CURRENT_MAX = 2000
BATTERY_CHARGING_CURRENT_STEP = 40  # SDK resolution: 40 mA per step

CONF_SUPPLY_MAINTAIN_VOLTAGE_VALUE = "supply_maintain_voltage"
CONF_BATTERY_CHARGING_MAX_CURRENT_VALUE = "battery_charging_max_current"

PowerFeatherChargerValue = powerfeather_charger_ns.class_(
    "PowerFeatherChargerValue", number.Number, cg.Component
)

CONFIG_SCHEMA = POWERFEATHER_CHARGER_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_SUPPLY_MAINTAIN_VOLTAGE_VALUE): number.number_schema(
            PowerFeatherChargerValue,
            icon=ICON_VOLTAGE,
            unit_of_measurement=UNIT_MILLIVOLT,
            device_class=DEVICE_CLASS_VOLTAGE,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_BATTERY_CHARGING_MAX_CURRENT_VALUE): number.number_schema(
            PowerFeatherChargerValue,
            icon=ICON_CURRENT_DC,
            unit_of_measurement=UNIT_MILLIAMPERE,
            device_class=DEVICE_CLASS_CURRENT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
    }
)


async def to_code(config):
    charger = await cg.get_variable(config[CONF_POWERFEATHER_CHARGER_ID])

    if CONF_SUPPLY_MAINTAIN_VOLTAGE_VALUE in config:
        val = await number.new_number(
            config[CONF_SUPPLY_MAINTAIN_VOLTAGE_VALUE],
            min_value=SUPPLY_MAINTAIN_VOLTAGE_MIN,
            max_value=SUPPLY_MAINTAIN_VOLTAGE_MAX,
            step=SUPPLY_MAINTAIN_VOLTAGE_STEP,
        )
        await cg.register_parented(val, charger)
        cg.add(val.set_update_type(TASK_UPDATE_TYPES["SUPPLY_MAINTAIN_VOLTAGE"]))
        cg.add(charger.set_supply_maintain_voltage_value(val))

    if CONF_BATTERY_CHARGING_MAX_CURRENT_VALUE in config:
        val = await number.new_number(
            config[CONF_BATTERY_CHARGING_MAX_CURRENT_VALUE],
            min_value=BATTERY_CHARGING_CURRENT_MIN,
            max_value=BATTERY_CHARGING_CURRENT_MAX,
            step=BATTERY_CHARGING_CURRENT_STEP,
        )
        await cg.register_parented(val, charger)
        cg.add(val.set_update_type(TASK_UPDATE_TYPES["BATTERY_CHARGING_MAX_CURRENT"]))
        cg.add(charger.set_battery_charging_max_current_value(val))


import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import (
    DEVICE_CLASS_SWITCH,
    ENTITY_CATEGORY_CONFIG,
    ICON_THERMOMETER,
)
from esphome.components.powerfeather_mainboard import (
    ICON_ENERGY,
    ICON_LED_ON,
)
from .. import (
    CONF_POWERFEATHER_CHARGER_ID,
    POWERFEATHER_CHARGER_COMPONENT_SCHEMA,
    TASK_UPDATE_TYPES,
    powerfeather_charger_ns,
)

CONF_ENABLE_BATTERY_TEMP_SENSE_SWITCH = "enable_battery_temp_sense"
CONF_ENABLE_BATTERY_CHARGING_SWITCH = "enable_battery_charging"
CONF_ENABLE_STAT_SWITCH = "enable_stat"

PowerFeatherChargerSwitch = powerfeather_charger_ns.class_(
    "PowerFeatherChargerSwitch", switch.Switch, cg.Component
)

CONFIG_SCHEMA = POWERFEATHER_CHARGER_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_ENABLE_BATTERY_TEMP_SENSE_SWITCH): switch.switch_schema(
            PowerFeatherChargerSwitch,
            icon=ICON_THERMOMETER,
            device_class=DEVICE_CLASS_SWITCH,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_ENABLE_BATTERY_CHARGING_SWITCH): switch.switch_schema(
            PowerFeatherChargerSwitch,
            icon=ICON_ENERGY,
            device_class=DEVICE_CLASS_SWITCH,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_ENABLE_STAT_SWITCH): switch.switch_schema(
            PowerFeatherChargerSwitch,
            icon=ICON_LED_ON,
            device_class=DEVICE_CLASS_SWITCH,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
    }
)


async def to_code(config):
    charger = await cg.get_variable(config[CONF_POWERFEATHER_CHARGER_ID])

    if CONF_ENABLE_BATTERY_TEMP_SENSE_SWITCH in config:
        sw = await switch.new_switch(config[CONF_ENABLE_BATTERY_TEMP_SENSE_SWITCH])
        await cg.register_parented(sw, charger)
        cg.add(sw.set_update_type(TASK_UPDATE_TYPES["ENABLE_BATTERY_TEMP_SENSE"]))
        cg.add(charger.set_enable_battery_temp_sense_switch(sw))

    if CONF_ENABLE_BATTERY_CHARGING_SWITCH in config:
        sw = await switch.new_switch(config[CONF_ENABLE_BATTERY_CHARGING_SWITCH])
        await cg.register_parented(sw, charger)
        cg.add(sw.set_update_type(TASK_UPDATE_TYPES["ENABLE_BATTERY_CHARGING"]))
        cg.add(charger.set_enable_battery_charging_switch(sw))

    if CONF_ENABLE_STAT_SWITCH in config:
        sw = await switch.new_switch(config[CONF_ENABLE_STAT_SWITCH])
        await cg.register_parented(sw, charger)
        cg.add(sw.set_update_type(TASK_UPDATE_TYPES["ENABLE_STAT"]))
        cg.add(charger.set_enable_stat_switch(sw))


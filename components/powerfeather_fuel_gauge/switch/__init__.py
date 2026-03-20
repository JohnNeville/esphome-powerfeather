import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import (
    ICON_GAUGE,
    DEVICE_CLASS_SWITCH,
    ENTITY_CATEGORY_CONFIG,
)
from .. import (
    CONF_POWERFEATHER_FUEL_GAUGE_ID,
    POWERFEATHER_FUEL_GAUGE_COMPONENT_SCHEMA,
    TASK_UPDATE_TYPES,
    powerfeather_fuel_gauge_ns,
)

CONF_ENABLE_BATTERY_FUEL_GAUGE_SWITCH = "enable_battery_fuel_gauge"

PowerFeatherFuelGaugeSwitch = powerfeather_fuel_gauge_ns.class_(
    "PowerFeatherFuelGaugeSwitch", switch.Switch, cg.Component
)

CONFIG_SCHEMA = POWERFEATHER_FUEL_GAUGE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_ENABLE_BATTERY_FUEL_GAUGE_SWITCH): switch.switch_schema(
            PowerFeatherFuelGaugeSwitch,
            icon=ICON_GAUGE,
            device_class=DEVICE_CLASS_SWITCH,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
    }
)


async def to_code(config):
    fuel_gauge = await cg.get_variable(config[CONF_POWERFEATHER_FUEL_GAUGE_ID])

    if CONF_ENABLE_BATTERY_FUEL_GAUGE_SWITCH in config:
        sw = await switch.new_switch(config[CONF_ENABLE_BATTERY_FUEL_GAUGE_SWITCH])
        await cg.register_parented(sw, fuel_gauge)
        cg.add(sw.set_update_type(TASK_UPDATE_TYPES["ENABLE_BATTERY_FUEL_GAUGE"]))
        cg.add(fuel_gauge.set_enable_battery_fuel_gauge_switch(sw))


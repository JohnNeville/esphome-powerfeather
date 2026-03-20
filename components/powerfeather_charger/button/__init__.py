import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import (
    ICON_RESTART,
    DEVICE_CLASS_RESTART,
    ENTITY_CATEGORY_CONFIG,
    ENTITY_CATEGORY_DIAGNOSTIC,
)
from .. import (
    CONF_POWERFEATHER_CHARGER_ID,
    POWERFEATHER_CHARGER_COMPONENT_SCHEMA,
    TASK_UPDATE_TYPES,
    powerfeather_charger_ns,
)

CONF_SHIP_MODE_BUTTON = "ship_mode"
CONF_SHUTDOWN_BUTTON = "shutdown"
CONF_POWER_CYCLE_BUTTON = "powercycle"

PowerFeatherChargerButton = powerfeather_charger_ns.class_(
    "PowerFeatherChargerButton", button.Button, cg.Component
)

CONFIG_SCHEMA = POWERFEATHER_CHARGER_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_SHIP_MODE_BUTTON): button.button_schema(
            PowerFeatherChargerButton,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_SHUTDOWN_BUTTON): button.button_schema(
            PowerFeatherChargerButton,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_POWER_CYCLE_BUTTON): button.button_schema(
            PowerFeatherChargerButton,
            icon=ICON_RESTART,
            device_class=DEVICE_CLASS_RESTART,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    }
)


async def to_code(config):
    charger = await cg.get_variable(config[CONF_POWERFEATHER_CHARGER_ID])

    if CONF_SHIP_MODE_BUTTON in config:
        btn = await button.new_button(config[CONF_SHIP_MODE_BUTTON])
        await cg.register_parented(btn, charger)
        cg.add(btn.set_update_type(TASK_UPDATE_TYPES["SHIP_MODE"]))
        cg.add(charger.set_ship_mode_button(btn))

    if CONF_SHUTDOWN_BUTTON in config:
        btn = await button.new_button(config[CONF_SHUTDOWN_BUTTON])
        await cg.register_parented(btn, charger)
        cg.add(btn.set_update_type(TASK_UPDATE_TYPES["SHUTDOWN"]))
        cg.add(charger.set_shutdown_button(btn))

    if CONF_POWER_CYCLE_BUTTON in config:
        btn = await button.new_button(config[CONF_POWER_CYCLE_BUTTON])
        await cg.register_parented(btn, charger)
        cg.add(btn.set_update_type(TASK_UPDATE_TYPES["POWERCYCLE"]))
        cg.add(charger.set_powercycle_button(btn))


import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_UPDATE_INTERVAL
from esphome.components.powerfeather_mainboard import (
    CONF_POWERFEATHER_MAINBOARD_ID,
    PowerFeatherMainboard,
    powerfeather_ns,
)

CODEOWNERS = ["powerfeatherdev (dev@powerfeather.dev)"]
AUTO_LOAD = ["sensor", "binary_sensor", "switch", "button", "number"]
DEPENDENCIES = ["powerfeather_mainboard"]

CONF_POWERFEATHER_CHARGER_ID = "charger_id"

powerfeather_charger_ns = cg.esphome_ns.namespace("powerfeather_charger")
PowerFeatherCharger = powerfeather_charger_ns.class_(
    "PowerFeatherCharger", cg.PollingComponent
)

# Re-export shared task update types so sub-platforms can reference them
TaskUpdateType = powerfeather_ns.enum("TaskUpdateType")
TASK_UPDATE_TYPES = {
    "CHARGER_SENSORS":              TaskUpdateType.CHARGER_SENSORS,
    "ENABLE_BATTERY_TEMP_SENSE":    TaskUpdateType.ENABLE_BATTERY_TEMP_SENSE,
    "ENABLE_BATTERY_CHARGING":      TaskUpdateType.ENABLE_BATTERY_CHARGING,
    "ENABLE_STAT":                  TaskUpdateType.ENABLE_STAT,
    "SHIP_MODE":                    TaskUpdateType.SHIP_MODE,
    "SHUTDOWN":                     TaskUpdateType.SHUTDOWN,
    "POWERCYCLE":                   TaskUpdateType.POWERCYCLE,
    "SUPPLY_MAINTAIN_VOLTAGE":      TaskUpdateType.SUPPLY_MAINTAIN_VOLTAGE,
    "BATTERY_CHARGING_MAX_CURRENT": TaskUpdateType.BATTERY_CHARGING_MAX_CURRENT,
}

# Schema fragment used by charger sub-platform __init__.py files
POWERFEATHER_CHARGER_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_POWERFEATHER_CHARGER_ID): cv.use_id(PowerFeatherCharger),
    }
)

UPDATE_INTERVAL_MINIMUM = "500ms"


def validate_update_interval(value):
    value = cv.positive_time_period_milliseconds(value)
    if value < cv.time_period(UPDATE_INTERVAL_MINIMUM):
        raise cv.Invalid(
            "Update interval must be at least {}".format(UPDATE_INTERVAL_MINIMUM)
        )
    return value


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(PowerFeatherCharger),
        cv.Required(CONF_POWERFEATHER_MAINBOARD_ID): cv.use_id(PowerFeatherMainboard),
        cv.Optional(CONF_UPDATE_INTERVAL, "10s"): validate_update_interval,
    }
)


async def to_code(config):
    charger = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(charger, config)

    mainboard = await cg.get_variable(config[CONF_POWERFEATHER_MAINBOARD_ID])
    cg.add(charger.set_parent(mainboard))


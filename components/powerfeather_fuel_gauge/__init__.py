import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_UPDATE_INTERVAL
from esphome.components.powerfeather_mainboard import (
    CONF_POWERFEATHER_MAINBOARD_ID,
    PowerFeatherMainboard,
    powerfeather_ns,
)

CODEOWNERS = ["powerfeatherdev (dev@powerfeather.dev)"]
AUTO_LOAD = ["sensor", "switch", "text_sensor"]
DEPENDENCIES = ["powerfeather_mainboard"]

CONF_POWERFEATHER_FUEL_GAUGE_ID = "fuel_gauge_id"

powerfeather_fuel_gauge_ns = cg.esphome_ns.namespace("powerfeather_fuel_gauge")
PowerFeatherFuelGauge = powerfeather_fuel_gauge_ns.class_(
    "PowerFeatherFuelGauge", cg.PollingComponent
)

# Re-export shared task update types so sub-platforms can reference them
TaskUpdateType = powerfeather_ns.enum("TaskUpdateType")
TASK_UPDATE_TYPES = {
    "FUEL_GAUGE_SENSORS":           TaskUpdateType.FUEL_GAUGE_SENSORS,
    "ENABLE_BATTERY_FUEL_GAUGE":    TaskUpdateType.ENABLE_BATTERY_FUEL_GAUGE,
}

# Schema fragment used by fuel gauge sub-platform __init__.py files
POWERFEATHER_FUEL_GAUGE_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_POWERFEATHER_FUEL_GAUGE_ID): cv.use_id(PowerFeatherFuelGauge),
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
        cv.GenerateID(): cv.declare_id(PowerFeatherFuelGauge),
        cv.Required(CONF_POWERFEATHER_MAINBOARD_ID): cv.use_id(PowerFeatherMainboard),
        cv.Optional(CONF_UPDATE_INTERVAL, "10s"): validate_update_interval,
    }
)


async def to_code(config):
    fuel_gauge = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(fuel_gauge, config)

    # This forces ESPHome/PlatformIO to pull the library for this component
    cg.add_library("PowerFeather-SDK",None)

    mainboard = await cg.get_variable(config[CONF_POWERFEATHER_MAINBOARD_ID])
    cg.add(fuel_gauge.set_parent(mainboard))


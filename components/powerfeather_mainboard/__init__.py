import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_UPDATE_INTERVAL

ICON_CURRENT_DC = "mdi:current-dc"
ICON_VOLTAGE = "mdi:sine-wave"
ICON_ENERGY = "mdi:lightning-bolt"
ICON_LED_ON = "mdi:led-on"
UNIT_MILLIAMPERE = "mA"
UNIT_MILLIVOLT = "mV"
UNIT_MINUTES = "min"

CODEOWNERS = ["powerfeatherdev (dev@powerfeather.dev)"]
AUTO_LOAD = ["switch"]

CONF_POWERFEATHER_MAINBOARD_ID = "mainboard_id"

powerfeather_ns = cg.esphome_ns.namespace("powerfeather_mainboard")
PowerFeatherMainboard = powerfeather_ns.class_(
    "PowerFeatherMainboard", cg.PollingComponent,
)

UPDATE_INTERVAL_MINIMUM = "500ms"

# Definitions from SDK — must be kept in sync with the SDK enum
BATTERY_CAPACITY_MINIMUM = 50
BatteryType = powerfeather_ns.enum("BatteryType")
BATTERY_TYPES = {
    "Generic_3V7" : BatteryType.Generic_3V7,
    "ICR18650_26H" : BatteryType.ICR18650_26H,
    "UR18650ZY" : BatteryType.UR18650ZY
}

# Shared TaskUpdateType enum — exported so charger/fuel gauge can reference it
TaskUpdateType = powerfeather_ns.enum("TaskUpdateType")
TASK_UPDATE_TYPES = {
    "ENABLE_EN"    : TaskUpdateType.ENABLE_EN,
    "ENABLE_3V3"   : TaskUpdateType.ENABLE_3V3,
    "ENABLE_VSQT"  : TaskUpdateType.ENABLE_VSQT,
}

CONF_BATTERY_CAPACITY = "battery_capacity"
CONF_BATTERY_TYPE = "battery_type"

# Schema fragment used by powerfeather_mainboard sub-platform __init__.py files
# (e.g. switch/) to reference the parent mainboard component.
POWERFEATHER_MAINBOARD_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_POWERFEATHER_MAINBOARD_ID): cv.use_id(PowerFeatherMainboard),
    }
)

def validate_update_interval(value):
    value = cv.positive_time_period_milliseconds(value)
    if value < cv.time_period(UPDATE_INTERVAL_MINIMUM):
        raise cv.Invalid(
            "Update interval must be at least {}".format(UPDATE_INTERVAL_MINIMUM)
        )
    return value

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(PowerFeatherMainboard),
        cv.Optional(CONF_BATTERY_CAPACITY, default=0): cv.Any(
            cv.Range(min=BATTERY_CAPACITY_MINIMUM),
            cv.Range(max=0)
        ),
        cv.Optional(CONF_BATTERY_TYPE, "Generic_3V7"): cv.enum(BATTERY_TYPES),
        cv.Optional(CONF_UPDATE_INTERVAL, "10s"): validate_update_interval,
    }
)

async def to_code(config):
    mainboard = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(mainboard, config)

    if battery_capacity_config := config.get(CONF_BATTERY_CAPACITY):
        cg.add(mainboard.set_battery_capacity(battery_capacity_config))
    if battery_type_config := config.get(CONF_BATTERY_TYPE):
        cg.add(mainboard.set_battery_type(battery_type_config))


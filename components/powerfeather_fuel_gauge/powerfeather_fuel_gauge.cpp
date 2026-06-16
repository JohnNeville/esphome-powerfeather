
#include "esphome/core/log.h"
#include "powerfeather_fuel_gauge.h"

#include <PowerFeather.h>

namespace esphome
{
  namespace powerfeather_fuel_gauge
  {
    static const char *TAG = "fuel_gauge";

    static const char *result_to_cstr(PowerFeather::Result r)
    {
      switch (r)
      {
        case PowerFeather::Result::InvalidState: return "InvalidState (gauge disabled/not ready)";
        case PowerFeather::Result::Failure:      return "Failure (I2C/hardware error)";
        case PowerFeather::Result::NotReady:     return "NotReady";
        case PowerFeather::Result::LockFailed:   return "LockFailed";
        case PowerFeather::Result::Timeout:      return "Timeout";
        case PowerFeather::Result::InvalidArg:   return "InvalidArg";
        default:                                 return "Unknown error";
      }
    }

    void PowerFeatherFuelGauge::setup()
    {
      if (!this->parent_->is_board_ready())
      {
        ESP_LOGE(TAG, "Board not ready — fuel gauge (LC709204F) setup skipped");
        mark_failed();
        return;
      }

      // Fuel gauge is enabled by default after Board.init(); publish that state
      if (enable_battery_fuel_gauge_switch_)
      {
        enable_battery_fuel_gauge_switch_->publish_state(enable_battery_fuel_gauge_);
      }

      // Register with mainboard so FreeRTOS task routes updates here
      this->parent_->register_subcomponent(this);
    }

    void PowerFeatherFuelGauge::loop()
    {
      uint32_t now = millis();
      if (!sensors_updated_ && now >= ((sensors_publish_time_ + update_interval_) - SENSOR_PREFETCH_MS_))
      {
        TaskUpdate update;
        update.type = TaskUpdateType::FUEL_GAUGE_SENSORS;
        this->parent_->send_task_update(update);
        sensors_updated_ = true;
      }
    }

    void PowerFeatherFuelGauge::update()
    {
      publish_sensors_();
      sensors_publish_time_ = millis();
      sensors_updated_ = false;
    }

    void PowerFeatherFuelGauge::update_sensors()
    {
      // Called from FreeRTOS task — all I2C reads happen here.
      if (!enable_battery_fuel_gauge_)
      {
        battery_charge_ = NAN;
        battery_health_ = NAN;
        battery_cycles_ = NAN;
        battery_time_left_ = NAN;
        battery_status_ = NAN;
        battery_low_charge_alarm_ = NAN;
        battery_low_voltage_alarm_ = NAN;
        battery_high_voltage_alarm_ = NAN;
        status_message_ = "Disabled";
        return;
      }

      uint8_t  pct = 0;
      uint16_t u16 = 0;
      int      minutes = 0;
      bool     alarm = false;
      bool     any_error = false;
      PowerFeather::Result r;

      // Helper: record first error and warn, return true on success.
      auto check = [&](PowerFeather::Result result, const char *name) -> bool {
        if (result == PowerFeather::Result::Ok)
          return true;
        if (!any_error)
        {
          status_message_ = std::string(name) + ": " + result_to_cstr(result);
          any_error = true;
          ESP_LOGW(TAG, "Fuel gauge read failed for '%s': %s", name, result_to_cstr(result));
        }
        return false;
      };

      if (battery_charge_sensor_)
        if (check(PowerFeather::Board.getBatteryCharge(pct), "charge"))
          battery_charge_ = static_cast<float>(pct);

      if (battery_health_sensor_)
        if (check(PowerFeather::Board.getBatteryHealth(pct), "health"))
          battery_health_ = static_cast<float>(pct);

      if (battery_cycles_sensor_)
        if (check(PowerFeather::Board.getBatteryCycles(u16), "cycles"))
          battery_cycles_ = static_cast<float>(u16);

      if (battery_time_left_sensor_)
      {
        r = PowerFeather::Board.getBatteryTimeLeft(minutes);
        if (r == PowerFeather::Result::Ok)
          battery_time_left_ = static_cast<float>(minutes);
        else if (r != PowerFeather::Result::NotReady) // NotReady is normal until 10% swing seen
          check(r, "time_left");
      }

      if (battery_low_charge_alarm_sensor_)
        if (check(PowerFeather::Board.getBatteryLowChargeAlarm(alarm), "low_charge_alarm"))
          battery_low_charge_alarm_ = alarm ? 1.0f : 0.0f;

      if (battery_low_voltage_alarm_sensor_)
        if (check(PowerFeather::Board.getBatteryLowVoltageAlarm(alarm), "low_voltage_alarm"))
          battery_low_voltage_alarm_ = alarm ? 1.0f : 0.0f;

      if (battery_high_voltage_alarm_sensor_)
        if (check(PowerFeather::Board.getBatteryHighVoltageAlarm(alarm), "high_voltage_alarm"))
          battery_high_voltage_alarm_ = alarm ? 1.0f : 0.0f;

      if (!any_error)
      {
        status_message_ = "OK";
        if (battery_low_charge_alarm_sensor_  && battery_low_charge_alarm_ > 0.5f)  status_message_ += " [low-charge-alarm]";
        if (battery_low_voltage_alarm_sensor_ && battery_low_voltage_alarm_ > 0.5f) status_message_ += " [low-voltage-alarm]";
        if (battery_high_voltage_alarm_sensor_&& battery_high_voltage_alarm_ > 0.5f)status_message_ += " [high-voltage-alarm]";
      }
    }

    void PowerFeatherFuelGauge::handle_update(const TaskUpdate &update)
    {
      switch (update.type)
      {
      case TaskUpdateType::FUEL_GAUGE_SENSORS:
        update_sensors();
        break;

      case TaskUpdateType::ENABLE_BATTERY_FUEL_GAUGE:
        enable_battery_fuel_gauge_ = update.data.b;
        PowerFeather::Board.enableBatteryFuelGauge(enable_battery_fuel_gauge_);
        break;

      default:
        break;
      }
    }

    void PowerFeatherFuelGauge::publish_sensors_()
    {
      if (status_sensor_)               status_sensor_->publish_state(status_message_);
      if (battery_charge_sensor_)       battery_charge_sensor_->publish_state(battery_charge_);
      if (battery_health_sensor_)       battery_health_sensor_->publish_state(battery_health_);
      if (battery_cycles_sensor_)       battery_cycles_sensor_->publish_state(battery_cycles_);
      if (battery_time_left_sensor_)    battery_time_left_sensor_->publish_state(battery_time_left_);
      if (battery_low_charge_alarm_sensor_) battery_low_charge_alarm_sensor_->publish_state(battery_low_charge_alarm_);
      if (battery_low_voltage_alarm_sensor_) battery_low_voltage_alarm_sensor_->publish_state(battery_low_voltage_alarm_);
      if (battery_high_voltage_alarm_sensor_) battery_high_voltage_alarm_sensor_->publish_state(battery_high_voltage_alarm_);
    }

    void PowerFeatherFuelGauge::dump_config()
    {
      ESP_LOGCONFIG(TAG, "PowerFeather Fuel Gauge (LC709204F)");
    }

  } // namespace powerfeather_fuel_gauge
} // namespace esphome


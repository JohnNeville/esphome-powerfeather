
#include "esphome/core/log.h"
#include "powerfeather_fuel_gauge.h"

#include <PowerFeather.h>

namespace esphome
{
  namespace powerfeather_fuel_gauge
  {
    static const char *TAG = "fuel_gauge";

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
      // Called from FreeRTOS task — all I2C reads happen here
      uint8_t  pct = 0;
      uint16_t u16 = 0;
      int      minutes = 0;
      float    temperature = 0;

      if (battery_charge_sensor_)
        if (PowerFeather::Board.getBatteryCharge(pct) == PowerFeather::Result::Ok)
          battery_charge_ = static_cast<float>(pct);

      if (battery_health_sensor_)
        if (PowerFeather::Board.getBatteryHealth(pct) == PowerFeather::Result::Ok)
          battery_health_ = static_cast<float>(pct);

      if (battery_cycles_sensor_)
        if (PowerFeather::Board.getBatteryCycles(u16) == PowerFeather::Result::Ok)
          battery_cycles_ = static_cast<float>(u16);

      if (battery_time_left_sensor_)
        if (PowerFeather::Board.getBatteryTimeLeft(minutes) == PowerFeather::Result::Ok)
          battery_time_left_ = static_cast<float>(minutes);

      if (battery_temperature_sensor_)
        if (PowerFeather::Board.getBatteryTemperature(temperature) == PowerFeather::Result::Ok)
          battery_temperature_ = temperature;
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
      if (battery_charge_sensor_)       battery_charge_sensor_->publish_state(battery_charge_);
      if (battery_health_sensor_)       battery_health_sensor_->publish_state(battery_health_);
      if (battery_cycles_sensor_)       battery_cycles_sensor_->publish_state(battery_cycles_);
      if (battery_time_left_sensor_)    battery_time_left_sensor_->publish_state(battery_time_left_);
      if (battery_temperature_sensor_)  battery_temperature_sensor_->publish_state(battery_temperature_);
    }

    void PowerFeatherFuelGauge::dump_config()
    {
      ESP_LOGCONFIG(TAG, "PowerFeather Fuel Gauge (LC709204F)");
    }

  } // namespace powerfeather_fuel_gauge
} // namespace esphome


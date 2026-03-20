
#include "esphome/core/log.h"
#include "powerfeather_charger.h"

#include <PowerFeather.h>

namespace esphome
{
  namespace powerfeather_charger
  {
    static const char *TAG = "charger";

    void PowerFeatherCharger::setup()
    {
      if (!this->parent_->is_board_ready())
      {
        ESP_LOGE(TAG, "Board not ready — charger (BQ2562x) setup skipped");
        mark_failed();
        return;
      }

      // Read initial switch states from hardware
      bool enabled = false;
      if (enable_stat_switch_)
      {
        if (PowerFeather::Board.getCharger().getSTATEnabled(enabled))
        {
          enable_stat_ = enabled;
          enable_stat_switch_->publish_state(enabled);
        }
      }
      if (enable_battery_charging_switch_)
      {
        if (PowerFeather::Board.getCharger().getChargingEnabled(enabled))
        {
          enable_battery_charging_ = enabled;
          enable_battery_charging_switch_->publish_state(enabled);
        }
      }
      if (enable_battery_temp_sense_switch_)
      {
        if (PowerFeather::Board.getCharger().getTSEnabled(enabled))
        {
          enable_battery_temp_sense_ = enabled;
          enable_battery_temp_sense_switch_->publish_state(enabled);
        }
      }

      // Register with mainboard so FreeRTOS task routes updates here
      this->parent_->register_subcomponent(this);
    }

    void PowerFeatherCharger::loop()
    {
      uint32_t now = millis();
      if (!sensors_updated_ && now >= ((sensors_publish_time_ + update_interval_) - SENSOR_PREFETCH_MS_))
      {
        TaskUpdate update;
        update.type = TaskUpdateType::CHARGER_SENSORS;
        this->parent_->send_task_update(update);
        sensors_updated_ = true;
      }
    }

    void PowerFeatherCharger::update()
    {
      publish_sensors_();
      sensors_publish_time_ = millis();
      sensors_updated_ = false;
    }

    void PowerFeatherCharger::update_sensors()
    {
      // Called from FreeRTOS task — all I2C reads happen here
      uint16_t v = 0;
      int16_t  c = 0;
      bool good = false;

      if (supply_voltage_sensor_)
        if (PowerFeather::Board.getSupplyVoltage(v) == PowerFeather::Result::Ok)
          supply_voltage_ = static_cast<float>(v);

      if (supply_current_sensor_)
        if (PowerFeather::Board.getSupplyCurrent(c) == PowerFeather::Result::Ok)
          supply_current_ = static_cast<float>(c);

      if (supply_good_sensor_)
        if (PowerFeather::Board.checkSupplyGood(good) == PowerFeather::Result::Ok)
          supply_good_ = good;

      if (battery_voltage_sensor_)
        if (PowerFeather::Board.getBatteryVoltage(v) == PowerFeather::Result::Ok)
          battery_voltage_ = static_cast<float>(v);

      if (battery_current_sensor_)
        if (PowerFeather::Board.getBatteryCurrent(c) == PowerFeather::Result::Ok)
          battery_current_ = static_cast<float>(c);
    }

    void PowerFeatherCharger::handle_update(const TaskUpdate &update)
    {
      switch (update.type)
      {
      case TaskUpdateType::CHARGER_SENSORS:
        update_sensors();
        break;

      case TaskUpdateType::ENABLE_BATTERY_TEMP_SENSE:
        enable_battery_temp_sense_ = update.data.b;
        PowerFeather::Board.enableBatteryTempSense(enable_battery_temp_sense_);
        break;

      case TaskUpdateType::ENABLE_BATTERY_CHARGING:
        enable_battery_charging_ = update.data.b;
        PowerFeather::Board.enableBatteryCharging(enable_battery_charging_);
        break;

      case TaskUpdateType::ENABLE_STAT:
        enable_stat_ = update.data.b;
        PowerFeather::Board.enableSTAT(enable_stat_);
        break;

      case TaskUpdateType::POWERCYCLE:
        PowerFeather::Board.doPowerCycle();
        break;

      case TaskUpdateType::SHIP_MODE:
        PowerFeather::Board.enterShipMode();
        break;

      case TaskUpdateType::SHUTDOWN:
        PowerFeather::Board.enterShutdownMode();
        break;

      case TaskUpdateType::SUPPLY_MAINTAIN_VOLTAGE:
        supply_maintain_voltage_ = update.data.f;
        PowerFeather::Board.setSupplyMaintainVoltage(static_cast<uint16_t>(supply_maintain_voltage_));
        break;

      case TaskUpdateType::BATTERY_CHARGING_MAX_CURRENT:
        battery_charging_max_current_ = update.data.f;
        PowerFeather::Board.setBatteryChargingMaxCurrent(static_cast<uint16_t>(battery_charging_max_current_));
        break;

      default:
        break;
      }
    }

    void PowerFeatherCharger::publish_sensors_()
    {
      if (supply_voltage_sensor_)  supply_voltage_sensor_->publish_state(supply_voltage_);
      if (supply_current_sensor_)  supply_current_sensor_->publish_state(supply_current_);
      if (supply_good_sensor_)     supply_good_sensor_->publish_state(supply_good_);
      if (battery_voltage_sensor_) battery_voltage_sensor_->publish_state(battery_voltage_);
      if (battery_current_sensor_) battery_current_sensor_->publish_state(battery_current_);
    }

    void PowerFeatherCharger::dump_config()
    {
      ESP_LOGCONFIG(TAG, "PowerFeather Charger (BQ2562x)");
    }

  } // namespace powerfeather_charger
} // namespace esphome


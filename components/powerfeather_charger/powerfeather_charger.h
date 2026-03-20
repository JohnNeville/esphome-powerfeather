#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/number/number.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/button/button.h"

#include "../powerfeather_mainboard/powerfeather_mainboard.h"

namespace esphome
{
  namespace powerfeather_charger
  {
    // Re-export shared types so charger sub-platform headers can use them
    // without pulling in the mainboard header themselves.
    using powerfeather_mainboard::TaskUpdate;
    using powerfeather_mainboard::TaskUpdateType;
    using powerfeather_mainboard::PowerFeatherUpdateable;

    class PowerFeatherCharger
      : public PollingComponent
      , public Parented<powerfeather_mainboard::PowerFeatherMainboard>
      , public powerfeather_mainboard::PowerFeatherSubcomponent
    {
    public:
      void setup() override;
      void loop() override;
      void update() override;
      void dump_config() override;

      // PowerFeatherSubcomponent interface
      void update_sensors() override;
      void handle_update(const TaskUpdate &update) override;

      // Forwards to the mainboard's shared queue so charger switches/buttons
      // can be Parented<PowerFeatherCharger> and still reach the queue.
      void send_task_update(TaskUpdate update)
      {
        this->parent_->send_task_update(update);
      }

      void set_battery_capacity(int32_t v) { battery_capacity_ = v; }

      void set_supply_voltage_sensor(sensor::Sensor *s) { supply_voltage_sensor_ = s; }
      void set_supply_current_sensor(sensor::Sensor *s) { supply_current_sensor_ = s; }
      void set_battery_voltage_sensor(sensor::Sensor *s) { battery_voltage_sensor_ = s; }
      void set_battery_current_sensor(sensor::Sensor *s) { battery_current_sensor_ = s; }
      void set_supply_good_sensor(binary_sensor::BinarySensor *s) { supply_good_sensor_ = s; }

      void set_enable_stat_switch(switch_::Switch *sw) { enable_stat_switch_ = sw; }
      void set_enable_battery_charging_switch(switch_::Switch *sw) { enable_battery_charging_switch_ = sw; }
      void set_enable_battery_temp_sense_switch(switch_::Switch *sw) { enable_battery_temp_sense_switch_ = sw; }

      void set_ship_mode_button(button::Button *b) { ship_mode_button_ = b; }
      void set_shutdown_button(button::Button *b) { shutdown_button_ = b; }
      void set_powercycle_button(button::Button *b) { powercycle_button_ = b; }

      void set_supply_maintain_voltage_value(number::Number *n) { supply_maintain_voltage_value_ = n; }
      void set_battery_charging_max_current_value(number::Number *n) { battery_charging_max_current_value_ = n; }

    private:
      static const uint32_t SENSOR_PREFETCH_MS_ = 150;

      int32_t battery_capacity_ = 0;

      bool supply_good_ = false;
      bool enable_stat_ = false;
      bool enable_battery_charging_ = false;
      bool enable_battery_temp_sense_ = false;
      float supply_voltage_ = 0;
      float supply_current_ = 0;
      float battery_voltage_ = 0;
      float battery_current_ = 0;
      float supply_maintain_voltage_ = 0;
      float battery_charging_max_current_ = 0;

      binary_sensor::BinarySensor *supply_good_sensor_ = nullptr;
      sensor::Sensor *supply_voltage_sensor_ = nullptr;
      sensor::Sensor *supply_current_sensor_ = nullptr;
      sensor::Sensor *battery_voltage_sensor_ = nullptr;
      sensor::Sensor *battery_current_sensor_ = nullptr;

      switch_::Switch *enable_stat_switch_ = nullptr;
      switch_::Switch *enable_battery_charging_switch_ = nullptr;
      switch_::Switch *enable_battery_temp_sense_switch_ = nullptr;

      button::Button *ship_mode_button_ = nullptr;
      button::Button *shutdown_button_ = nullptr;
      button::Button *powercycle_button_ = nullptr;

      number::Number *supply_maintain_voltage_value_ = nullptr;
      number::Number *battery_charging_max_current_value_ = nullptr;

      void publish_sensors_();

      uint32_t sensors_publish_time_ = 0;
      bool sensors_updated_ = false;
    };
  } // namespace powerfeather_charger
} // namespace esphome


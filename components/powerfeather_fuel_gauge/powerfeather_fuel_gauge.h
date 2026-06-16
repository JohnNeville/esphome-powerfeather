#pragma once

#include <string>

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text_sensor/text_sensor.h"

#include "../powerfeather_mainboard/powerfeather_mainboard.h"

namespace esphome
{
  namespace powerfeather_fuel_gauge
  {
    // Re-export shared types for sub-platform headers
    using powerfeather_mainboard::TaskUpdate;
    using powerfeather_mainboard::TaskUpdateType;
    using powerfeather_mainboard::PowerFeatherUpdateable;

    class PowerFeatherFuelGauge
      : public PollingComponent
      , public Parented<powerfeather_mainboard::PowerFeatherMainboard>
      , public powerfeather_mainboard::PowerFeatherSubcomponent
    {
    public:
      void setup() override;
      void loop() override;
      void update() override;
      void dump_config() override;
      float get_setup_priority() const override { return setup_priority::HARDWARE; }

      // PowerFeatherSubcomponent interface
      void update_sensors() override;
      void handle_update(const TaskUpdate &update) override;

      // Forwards to the mainboard's shared queue
      void send_task_update(TaskUpdate update)
      {
        this->parent_->send_task_update(update);
      }

      void set_enable_battery_fuel_gauge_switch(switch_::Switch *sw) { enable_battery_fuel_gauge_switch_ = sw; }

      void set_status_sensor(text_sensor::TextSensor *s) { status_sensor_ = s; }

      void set_battery_charge_sensor(sensor::Sensor *s)    { battery_charge_sensor_ = s; }
      void set_battery_health_sensor(sensor::Sensor *s)    { battery_health_sensor_ = s; }
      void set_battery_cycles_sensor(sensor::Sensor *s)    { battery_cycles_sensor_ = s; }
      void set_battery_time_left_sensor(sensor::Sensor *s) { battery_time_left_sensor_ = s; }
      void set_battery_low_charge_alarm_sensor(sensor::Sensor *s) { battery_low_charge_alarm_sensor_ = s; }
      void set_battery_low_voltage_alarm_sensor(sensor::Sensor *s) { battery_low_voltage_alarm_sensor_ = s; }
      void set_battery_high_voltage_alarm_sensor(sensor::Sensor *s) { battery_high_voltage_alarm_sensor_ = s; }

    private:
      static const uint32_t SENSOR_PREFETCH_MS_ = 200;

      bool enable_battery_fuel_gauge_ = true;
      std::string status_message_{"Initializing"};

      float battery_charge_ = NAN;
      float battery_health_ = NAN;
      float battery_cycles_ = NAN;
      float battery_time_left_ = NAN;
      float battery_low_charge_alarm_ = NAN;
      float battery_low_voltage_alarm_ = NAN;
      float battery_high_voltage_alarm_ = NAN;

      switch_::Switch *enable_battery_fuel_gauge_switch_ = nullptr;
      text_sensor::TextSensor *status_sensor_ = nullptr;

      sensor::Sensor *battery_charge_sensor_ = nullptr;
      sensor::Sensor *battery_health_sensor_ = nullptr;
      sensor::Sensor *battery_cycles_sensor_ = nullptr;
      sensor::Sensor *battery_time_left_sensor_ = nullptr;
      sensor::Sensor *battery_low_charge_alarm_sensor_ = nullptr;
      sensor::Sensor *battery_low_voltage_alarm_sensor_ = nullptr;
      sensor::Sensor *battery_high_voltage_alarm_sensor_ = nullptr;

      void publish_sensors_();

      uint32_t sensors_publish_time_ = 0;
      bool sensors_updated_ = false;
    };
  } // namespace powerfeather_fuel_gauge
} // namespace esphome


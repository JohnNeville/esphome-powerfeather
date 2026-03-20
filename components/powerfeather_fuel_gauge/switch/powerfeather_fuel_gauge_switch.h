#pragma once

#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"

#include "../powerfeather_fuel_gauge.h"

namespace esphome
{
  namespace powerfeather_fuel_gauge
  {
    class PowerFeatherFuelGaugeSwitch
      : public switch_::Switch
      , public Parented<PowerFeatherFuelGauge>
      , public PowerFeatherUpdateable
    {
    public:
      PowerFeatherFuelGaugeSwitch() = default;

    protected:
      void write_state(bool state) override
      {
        TaskUpdate update;
        update.type = type_;
        update.data.b = state;
        this->parent_->send_task_update(update);
        this->publish_state(state);
      }
    };
  } // namespace powerfeather_fuel_gauge
} // namespace esphome


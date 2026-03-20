#pragma once

#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"

#include "../powerfeather_charger.h"

namespace esphome
{
  namespace powerfeather_charger
  {
    class PowerFeatherChargerSwitch
      : public switch_::Switch
      , public Parented<PowerFeatherCharger>
      , public PowerFeatherUpdateable
    {
    public:
      PowerFeatherChargerSwitch() = default;

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
  } // namespace powerfeather_charger
} // namespace esphome


#pragma once

#include "esphome/core/component.h"
#include "esphome/components/number/number.h"

#include "../powerfeather_charger.h"

namespace esphome
{
  namespace powerfeather_charger
  {
    class PowerFeatherChargerValue
      : public number::Number
      , public Parented<PowerFeatherCharger>
      , public PowerFeatherUpdateable
    {
    public:
      PowerFeatherChargerValue() = default;

    protected:
      void control(float value) override
      {
        TaskUpdate update;
        update.type = type_;
        update.data.f = value;
        this->parent_->send_task_update(update);
        this->publish_state(value);
      }
    };
  } // namespace powerfeather_charger
} // namespace esphome


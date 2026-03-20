#pragma once

#include "esphome/core/component.h"
#include "esphome/components/button/button.h"

#include "../powerfeather_charger.h"

namespace esphome
{
  namespace powerfeather_charger
  {
    class PowerFeatherChargerButton
      : public button::Button
      , public Parented<PowerFeatherCharger>
      , public PowerFeatherUpdateable
    {
    public:
      PowerFeatherChargerButton() = default;

    protected:
      void press_action() override
      {
        TaskUpdate update;
        update.type = type_;
        this->parent_->send_task_update(update);
      }
    };
  } // namespace powerfeather_charger
} // namespace esphome


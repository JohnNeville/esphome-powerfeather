#pragma once

#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"

#include <PowerFeather.h>
#include <vector>

namespace esphome
{
  namespace powerfeather_mainboard
  {
    enum BatteryType
    {
      Generic_3V7,
      ICR18650_26H,
      UR18650ZY,
    };

    enum TaskUpdateType
    {
      // Sensor pre-fetch triggers (sent by each subcomponent's loop())
      CHARGER_SENSORS = 0,
      FUEL_GAUGE_SENSORS,
      // Mainboard GPIO updates
      ENABLE_EN,
      ENABLE_3V3,
      ENABLE_VSQT,
      // BQ2562x charger control
      ENABLE_BATTERY_TEMP_SENSE,
      ENABLE_BATTERY_CHARGING,
      ENABLE_STAT,
      POWERCYCLE,
      SHIP_MODE,
      SHUTDOWN,
      SUPPLY_MAINTAIN_VOLTAGE,
      BATTERY_CHARGING_MAX_CURRENT,
      // LC709204F fuel gauge control
      ENABLE_BATTERY_FUEL_GAUGE,
    };

    typedef struct
    {
      TaskUpdateType type;
      union
      {
        bool b;
        int32_t i;
        float f;
        uint32_t u;
      } data;
    } TaskUpdate;

    class PowerFeatherUpdateable
    {
    public:
      PowerFeatherUpdateable() = default;
      void set_update_type(TaskUpdateType type) { type_ = type; }
    protected:
      TaskUpdateType type_;
    };

    // Abstract interface that charger and fuel gauge subcomponents implement.
    // The shared FreeRTOS task in PowerFeatherMainboard calls into each
    // registered subcomponent so that all I2C traffic is serialized.
    class PowerFeatherSubcomponent
    {
    public:
      virtual ~PowerFeatherSubcomponent() = default;
      virtual void update_sensors() = 0;
      virtual void handle_update(const TaskUpdate &update) = 0;
    };

    class PowerFeatherMainboard : public PollingComponent
    {
    public:
      void setup() override;
      void loop() override;
      void update() override;
      void dump_config() override;

      void set_battery_capacity(int32_t battery_capacity) { battery_capacity_ = battery_capacity; }
      void set_battery_type(BatteryType battery_type)
      {
        switch (battery_type)
        {
        case ICR18650_26H:
          battery_type_ = PowerFeather::Mainboard::BatteryType::ICR18650_26H;
          break;
        case UR18650ZY:
          battery_type_ = PowerFeather::Mainboard::BatteryType::UR18650ZY;
          break;
        case Generic_3V7:
        default:
          battery_type_ = PowerFeather::Mainboard::BatteryType::Generic_3V7;
          break;
        }
      }

      // GPIO switches (always available regardless of charger/fuel gauge state)
      void set_enable_3V3_switch(switch_::Switch *sw) { enable_3V3_switch_ = sw; }
      void set_enable_VSQT_switch(switch_::Switch *sw) { enable_VSQT_switch_ = sw; }
      void set_enable_EN_switch(switch_::Switch *sw) { enable_EN_switch_ = sw; }

      // Subcomponent registration — called by charger/fuel gauge during their setup()
      // if (and only if) their own initialisation succeeds.
      void register_subcomponent(PowerFeatherSubcomponent *subcomp);

      // Returns true if Board.init() succeeded.  Charger and fuel gauge components
      // check this before doing any I2C work so their setup() can fail gracefully.
      bool is_board_ready() const { return board_ready_; }

      void send_task_update(TaskUpdate update);

    private:
      static const size_t UPDATE_TASK_STACK_SIZE_ = 3192;
      static const size_t UPDATE_TASK_QUEUE_SIZE_ = 10;

      int32_t battery_capacity_ = 0;
      PowerFeather::Mainboard::BatteryType battery_type_ = PowerFeather::Mainboard::BatteryType::Generic_3V7;

      bool board_ready_ = false;
      bool enable_EN_ = false;
      bool enable_3V3_ = false;
      bool enable_VSQT_ = false;

      QueueHandle_t update_task_queue_ = NULL;

      switch_::Switch *enable_EN_switch_ = nullptr;
      switch_::Switch *enable_3V3_switch_ = nullptr;
      switch_::Switch *enable_VSQT_switch_ = nullptr;

      std::vector<PowerFeatherSubcomponent *> subcomponents_;

      static void update_task_(void *param);

      void setup_mainboard_();
      void handle_mainboard_update_(const TaskUpdate &update);
    };
  } // namespace powerfeather_mainboard
} // namespace esphome
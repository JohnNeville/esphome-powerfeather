
#include "esphome/core/log.h"
#include "powerfeather_mainboard.h"

#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_timer.h"

namespace esphome
{
  namespace powerfeather_mainboard
  {
    static const char *TAG = "mainboard";

    // =========================================================================
    // Mainboard (GPIO / Common) Functions
    // =========================================================================

    void PowerFeatherMainboard::setup_mainboard_()
    {
      if (enable_3V3_switch_)
      {
        static constexpr gpio_num_t EN_3V3 = GPIO_NUM_4;
        enable_3V3_ = rtc_gpio_get_level(EN_3V3);
        enable_3V3_switch_->publish_state(enable_3V3_);
      }

      if (enable_VSQT_switch_)
      {
        static constexpr gpio_num_t EN_VSQT = GPIO_NUM_14;
        enable_VSQT_ = rtc_gpio_get_level(EN_VSQT);
        enable_VSQT_switch_->publish_state(enable_VSQT_);
      }

      if (enable_EN_switch_)
      {
        static constexpr gpio_num_t EN0 = GPIO_NUM_13;
        enable_EN_ = rtc_gpio_get_level(EN0);
        enable_EN_switch_->publish_state(enable_EN_);
      }
    }

    void PowerFeatherMainboard::handle_mainboard_update_(const TaskUpdate &update)
    {
      switch (update.type)
      {
      case TaskUpdateType::ENABLE_EN:
        ESP_LOGD(TAG, "Recieved EN enable state: %d", update.data.b);
        enable_EN_ = update.data.b;
        PowerFeather::Board.setEN(enable_EN_);
        break;

      case TaskUpdateType::ENABLE_3V3:
        ESP_LOGD(TAG, "Recieved EN enable state: %d", update.data.b);
        enable_3V3_ = update.data.b;
        PowerFeather::Board.enable3V3(enable_3V3_);
        break;

      case TaskUpdateType::ENABLE_VSQT:
        ESP_LOGD(TAG, "Recieved VSQT enable state: %d", update.data.b);
        enable_VSQT_ = update.data.b;
        PowerFeather::Board.enableVSQT(enable_VSQT_);
        break;

      default:
        break;
      }
    }

    // =========================================================================
    // Task and Lifecycle (orchestration)
    // =========================================================================

    void PowerFeatherMainboard::register_subcomponent(PowerFeatherSubcomponent *subcomp)
    {
      subcomponents_.push_back(subcomp);
    }

    void PowerFeatherMainboard::update_task_(void *param)
    {
      PowerFeatherMainboard *mainboard =
        reinterpret_cast<PowerFeatherMainboard *>(param);

      while (true)
      {
        TaskUpdate update;
        update.type = TaskUpdateType::CHARGER_SENSORS;
        xQueueReceive(mainboard->update_task_queue_, &update, portMAX_DELAY);

        switch (update.type)
        {
        // Mainboard GPIO — handled directly, no subcomponent needed
        case TaskUpdateType::ENABLE_EN:
        case TaskUpdateType::ENABLE_3V3:
        case TaskUpdateType::ENABLE_VSQT:
          mainboard->handle_mainboard_update_(update);
          break;

        // Sensor pre-fetch and all subcomponent control updates are routed to
        // every registered subcomponent; each one handles only its own types.
        default:
          for (auto *sub : mainboard->subcomponents_)
          {
            sub->handle_update(update);
          }
          break;
        }
      }
    }

    void PowerFeatherMainboard::setup()
    {
      ESP_LOGD(TAG, "Initializing board, capacity: %d mAh and type: %u",
               this->battery_capacity_, static_cast<uint32_t>(this->battery_type_));

      // Board.init() may fail (e.g. I2C chips not responding). This is NOT fatal
      // for the mainboard itself — GPIO switches always work. Charger and fuel
      // gauge components will check is_board_ready() and fail gracefully.
      board_ready_ = (PowerFeather::Board.init(this->battery_capacity_, this->battery_type_)
                      == PowerFeather::Result::Ok);
      if (!board_ready_)
      {
        ESP_LOGE(TAG, "Board.init() failed — charger and fuel gauge will not be available");
      }

      setup_mainboard_();

      update_task_queue_ = xQueueCreate(UPDATE_TASK_QUEUE_SIZE_, sizeof(TaskUpdate));
      if (update_task_queue_ == NULL)
      {
        ESP_LOGE(TAG, "Failed to create PowerFeather task queue");
        mark_failed();
        return;
      }

      if (xTaskCreate(update_task_, "powerfeather_mainboard", UPDATE_TASK_STACK_SIZE_,
                      this, uxTaskPriorityGet(NULL), NULL) != pdTRUE)
      {
        ESP_LOGE(TAG, "Failed to create PowerFeather update task");
        mark_failed();
        return;
      }
    }

    void PowerFeatherMainboard::loop()
    {
      // No sensor pre-fetch needed on the mainboard itself — subcomponents
      // manage their own loop() timing and send their own sensor triggers.
    }

    void PowerFeatherMainboard::update()
    {
      // Sensor publishing is done by each subcomponent in its own update().
    }

    void PowerFeatherMainboard::dump_config()
    {
      ESP_LOGCONFIG(TAG, "Battery Capacity: %u", battery_capacity_);
      const char* battery_type_str;
      switch (battery_type_)
      {
      case PowerFeather::Mainboard::BatteryType::Generic_3V7:
        battery_type_str = "Generic_3V7";
        break;

      case PowerFeather::Mainboard::BatteryType::ICR18650_26H:
        battery_type_str = "ICR18650_26H";
        break;

      case PowerFeather::Mainboard::BatteryType::UR18650ZY:
        battery_type_str = "UR18650ZY";
        break;
      
      default:
        battery_type_str = "Invalid";
        break;
      }
      ESP_LOGCONFIG(TAG, "Battery Type: %s", battery_type_str);
    }

    void PowerFeatherMainboard::send_task_update(TaskUpdate update)
    {
      xQueueSend(update_task_queue_, &update, portMAX_DELAY);
    }

  } // namespace powerfeather_mainboard
} // namespace esphome
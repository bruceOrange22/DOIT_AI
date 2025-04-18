#ifndef BUTTON_H_
#define BUTTON_H_

#include <driver/gpio.h>
#include <iot_button.h>
#include <functional>

class Button {
public:
#if CONFIG_SOC_ADC_SUPPORTED
    Button(const button_adc_config_t& cfg);
#endif
    Button(gpio_num_t gpio_num, bool active_high = false, int long_press_time = 1000);
    ~Button();

    void OnPressDown(std::function<void()> callback);
    void OnPressUp(std::function<void()> callback);
    void OnLongPress(std::function<void()> callback);
    void OnClick(std::function<void()> callback);
    void OnDoubleClick(std::function<void()> callback);
    void OnPressRepeat(std::function<void(uint16_t)> callback);
    void OnPressRepeaDone(std::function<void(uint16_t)> callback);
private:
    gpio_num_t gpio_num_;
    button_handle_t button_handle_ = nullptr;


    std::function<void()> on_press_down_;
    std::function<void()> on_press_up_;
    std::function<void()> on_long_press_;
    std::function<void()> on_click_;
    std::function<void()> on_double_click_;
    std::function<void(uint16_t)> on_press_repeat_;
    std::function<void(uint16_t)> on_press_repeat_done_;
};

#endif // BUTTON_H_

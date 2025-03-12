
#include "wifi_board.h"
#include "audio_codecs/vb6824_audio_codec.h"
#include "opus_codecs/opus_codec.h"
#include "opus_codecs/no_opus_codec.h"
#include "opus_codecs/only_dec_opus_code.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "iot/thing_manager.h"

#include <wifi_station.h>
#include <esp_log.h>
#include <esp_lcd_panel_vendor.h>
#include <driver/spi_common.h>

#define TAG "CustomBoard"

class CustomBoard : public WifiBoard {
private:
    Button boot_button_;
    VbAduioCodec audio_codec;

    void InitializeButtons() {
        boot_button_.OnDoubleClick([this]() {
            // auto& app = Application::GetInstance();
            // if (app.GetDeviceState() == kDeviceStateStarting && !WifiStation::GetInstance().IsConnected()) {
                ResetWifiConfiguration();
            // }
        });
        boot_button_.OnPressDown([this]() {
            Application::GetInstance().StartListening();
        });
        boot_button_.OnPressUp([this]() {
            Application::GetInstance().StopListening();
        });
    }

    // 物联网初始化，添加对 AI 可见设备
    void InitializeIot() {
        auto& thing_manager = iot::ThingManager::GetInstance();
        thing_manager.AddThing(iot::CreateThing("Speaker"));
    }

public:
    CustomBoard() : boot_button_(BOOT_BUTTON_GPIO), audio_codec(CODEC_TX_GPIO, CODEC_RX_GPIO){          
        InitializeButtons();
        InitializeIot();
        audio_codec.OnWakeUp([this](const std::string& command) {
            if (command == "你好小智"){
                if(Application::GetInstance().GetDeviceState() != kDeviceStateListening){
                    Application::GetInstance().WakeWordInvoke("你好小智");
                }
            }else if (command == "开始配网"){
                ResetWifiConfiguration();
            }
        });
    }

    virtual AudioCodec* GetAudioCodec() override {
        return &audio_codec;
    }

    virtual OpusCodec* GetOpusCodec() override {
#if defined(CONFIG_OPUS_CODEC_TYPE_NO_CODEC)
        static NoOpusCodec opus_codec;
#elif defined(CONFIG_OPUS_CODEC_TYPE_ONLY_DECODE)
        static OnlyDecOpusCodec opus_codec;
#else
        static OpusCodec opus_codec;
#endif
        return &opus_codec;
    }

};

DECLARE_BOARD(CustomBoard);


#include "wifi_board.h"
#include "audio_codecs/vb6824_audio_codec.h"
#include "opus_codecs/no_opus_codec.h"
#include "display/lcd_display.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "iot/thing_manager.h"

#include <wifi_station.h>
#include <esp_log.h>
#include <esp_lcd_panel_vendor.h>
#include <driver/spi_common.h>

#include "esp_lcd_st7735.h"

#define TAG "CustomBoard"

LV_FONT_DECLARE(font_puhui_14_1);
LV_FONT_DECLARE(font_awesome_14_1);

class CustomBoard : public WifiBoard {
private:
    Button boot_button_;
    VbAduioCodec audio_codec;
    // LcdDisplay* display_;

    // void InitializeSpi() {
    //     spi_bus_config_t buscfg = {};
    //     buscfg.mosi_io_num = DISPLAY_MOSI_PIN;
    //     buscfg.miso_io_num = GPIO_NUM_NC;
    //     buscfg.sclk_io_num = DISPLAY_CLK_PIN;
    //     buscfg.quadwp_io_num = GPIO_NUM_NC;
    //     buscfg.quadhd_io_num = GPIO_NUM_NC;
    //     buscfg.max_transfer_sz = DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint16_t);
    //     // buscfg.flags = SPICOMMON_BUSFLAG_MASTER;
    //     ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    // }

    // void InitializeLcdDisplay() {
    //     esp_lcd_panel_io_handle_t panel_io = nullptr;
    //     esp_lcd_panel_handle_t panel = nullptr;
    //     // // // 液晶屏控制IO初始化
    //     // ESP_LOGD(TAG, "Install panel IO");
    //     // esp_lcd_panel_io_spi_config_t io_config = {};
    //     // io_config.cs_gpio_num = DISPLAY_CS_PIN;
    //     // io_config.dc_gpio_num = DISPLAY_DC_PIN;
    //     // io_config.spi_mode = 2;
    //     // io_config.pclk_hz = 80 * 1000 * 1000;
    //     // io_config.trans_queue_depth = 10;
    //     // io_config.lcd_cmd_bits = 8;
    //     // io_config.lcd_param_bits = 8;
    //     // ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(SPI2_HOST, &io_config, &panel_io));

    //     // // 初始化液晶屏驱动芯片
    //     // ESP_LOGD(TAG, "Install LCD driver");
    //     // esp_lcd_panel_dev_config_t panel_config = {};
    //     // panel_config.reset_gpio_num = DISPLAY_RST_PIN;
    //     // panel_config.rgb_ele_order = DISPLAY_RGB_ORDER;
    //     // panel_config.bits_per_pixel = 16;
    //     // ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(panel_io, &panel_config, &panel));

    //     const esp_lcd_panel_io_spi_config_t io_config =
    //         st7735_PANEL_IO_SPI_CONFIG(DISPLAY_CS_PIN, DISPLAY_DC_PIN, NULL, NULL);
    //     ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST, &io_config, &panel_io));

    //     st7735_vendor_config_t vendor_config = {
    //         .init_cmds = NULL,
    //         .init_cmds_size = 0,
    //     };
    //     const esp_lcd_panel_dev_config_t panel_config = {
    //         .reset_gpio_num = DISPLAY_RST_PIN,
    //         .rgb_ele_order =
    //             LCD_RGB_ELEMENT_ORDER_RGB,
    //         .bits_per_pixel =
    //             16,
    //         .vendor_config = &vendor_config,
    //     };
    //     ESP_ERROR_CHECK(esp_lcd_new_panel_st7735(panel_io, &panel_config, &panel));
        
    //     esp_lcd_panel_reset(panel);

    //     esp_lcd_panel_init(panel);
    //     esp_lcd_panel_invert_color(panel, DISPLAY_INVERT_COLOR);
    //     esp_lcd_panel_swap_xy(panel, DISPLAY_SWAP_XY);
    //     esp_lcd_panel_mirror(panel, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y);
    //     display_ = new LcdDisplay(panel_io, panel, DISPLAY_BACKLIGHT_PIN, DISPLAY_BACKLIGHT_OUTPUT_INVERT,
    //                                 DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_OFFSET_X, DISPLAY_OFFSET_Y, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y, DISPLAY_SWAP_XY,
    //                                 {
    //                                     .text_font = &font_puhui_14_1,
    //                                     .icon_font = &font_awesome_14_1,
    //                                     .emoji_font = DISPLAY_HEIGHT >= 240 ? font_emoji_64_init() : font_emoji_32_init(),
    //                                 });
    // }

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
        // InitializeSpi();
        // InitializeLcdDisplay();
        audio_codec.OnWakeUp([this]() {
            if(Application::GetInstance().GetDeviceState() != kDeviceStateListening){
                Application::GetInstance().WakeWordInvoke("你好小智");
            }
        });
    }

    virtual AudioCodec* GetAudioCodec() override {
        return &audio_codec;
    }

    // virtual Display* GetDisplay() override {
    //     return display_;
    // }

    virtual OpusCodec* GetOpusCodec() override {
        static NoOpusCodec opus_codec;
        return &opus_codec;
    }
};

DECLARE_BOARD(CustomBoard);

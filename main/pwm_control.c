#include "pwm_control.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include <stdio.h>

// 前向声明OLED函数
void oled_clear(void);
void oled_draw_string(int x, int y, const char *str);
void oled_refresh(void);

// 存储当前PWM占空比状态
static uint32_t io_duty[4] = {0, 0, 0, 0};

// 初始化PWM控制
void pwm_init(void)
{
    // 配置LEDC定时器
    ledc_timer_config_t timer_config = {
        .duty_resolution = LEDC_TIMER_10_BIT,  // 10位分辨率（0-1023）
        .freq_hz = 1000,                        // 1kHz频率
        .speed_mode = LEDC_LOW_SPEED_MODE,      // 低速模式
        .timer_num = LEDC_TIMER_0,              // 使用定时器0
        .clk_cfg = LEDC_AUTO_CLK,               // 自动时钟
    };
    
    esp_err_t err = ledc_timer_config(&timer_config);
    
    // 配置IO0通道
    ledc_channel_config_t io0_config = {
        .channel = LEDC_CHANNEL_0,
        .duty = 0,                              // 初始占空比0
        .gpio_num = IO0_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel = LEDC_TIMER_0,
    };
    ledc_channel_config(&io0_config);
    
    // 配置IO1通道
    ledc_channel_config_t io1_config = {
        .channel = LEDC_CHANNEL_1,
        .duty = 0,                              // 初始占空比0
        .gpio_num = IO1_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel = LEDC_TIMER_0,
    };
    ledc_channel_config(&io1_config);
    
    // 配置IO2通道
    ledc_channel_config_t io2_config = {
        .channel = LEDC_CHANNEL_2,
        .duty = 0,                              // 初始占空比0
        .gpio_num = IO2_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel = LEDC_TIMER_0,
    };
    ledc_channel_config(&io2_config);
    
    // 配置IO3通道
    ledc_channel_config_t io3_config = {
        .channel = LEDC_CHANNEL_3,
        .duty = 0,                              // 初始占空比0
        .gpio_num = IO3_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel = LEDC_TIMER_0,
    };
    ledc_channel_config(&io3_config);
    
    // 初始化占空比状态
    io_duty[0] = 0;
    io_duty[1] = 0;
    io_duty[2] = 0;
    io_duty[3] = 0;
}

// 设置IO的PWM占空比
void pwm_set_duty(uint8_t io_num, uint32_t duty)
{
    if (io_num >= 4) return;
    
    io_duty[io_num] = duty;
    
    switch (io_num) {
        case 0:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            break;
        case 1:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
            break;
        case 2:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
            break;
        case 3:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3, duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3);
            break;
    }
}

// 获取IO的当前PWM占空比
uint32_t pwm_get_duty(uint8_t io_num)
{
    if (io_num >= 4) return 0;
    return io_duty[io_num];
}

// 获取IO的当前PWM占空比百分比
int pwm_get_duty_percent(uint8_t io_num)
{
    if (io_num >= 4) return 0;
    return (io_duty[io_num] * 100) / 1023;
}

// 显示所有IO的PWM状态
void pwm_display_status(void)
{
    char buffer1[32];
    char buffer2[32];
    int io0_percent = pwm_get_duty_percent(0);
    int io1_percent = pwm_get_duty_percent(1);
    int io2_percent = pwm_get_duty_percent(2);
    int io3_percent = pwm_get_duty_percent(3);
    
    // 格式化显示内容
    snprintf(buffer1, sizeof(buffer1), "IO0:%d%%,IO1:%d%%", io0_percent, io1_percent);
    snprintf(buffer2, sizeof(buffer2), "IO2:%d%%,IO3:%d%%", io2_percent, io3_percent);
    
    // 显示在屏幕上，IO0和IO1在第一行，IO2和IO3在第二行
    oled_clear();
    oled_draw_string(0, 0, buffer1);
    oled_draw_string(0, 10, buffer2);
    oled_refresh();
}

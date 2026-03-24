#include "oled.h"
#include "pwm_control.h"
#include <math.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// 函数前向声明
void init_io(void);
void control_io(void);

// 初始化IO口（使用PWM控制模块）
void init_io(void)
{
    // 显示初始化IO的信息
    oled_clear();
    oled_draw_string(0, 0, "Initializing IO...");
    oled_refresh();
    vTaskDelay(pdMS_TO_TICKS(500));
    
    // 初始化PWM控制
    pwm_init();
    
    // 显示配置结果
    oled_clear();
    oled_draw_string(0, 0, "PWM config OK");
    oled_refresh();
    vTaskDelay(pdMS_TO_TICKS(500));
}

// 控制IO0、IO1、IO2和IO3的PWM占空比
void control_io(void)
{
    int count = 0;
    while(1) {
        count++;
        
        // 显示当前循环次数
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "Loop: %d", count);
        oled_clear();
        oled_draw_string(0, 0, buffer);
        oled_refresh();
        vTaskDelay(pdMS_TO_TICKS(500));
        
        // IO0=100%, IO1=0%, IO2=100%, IO3=0%
        pwm_set_duty(0, 1023);  // 100%占空比
        pwm_set_duty(1, 0);      // 0%占空比
        pwm_set_duty(2, 1023);  // 100%占空比
        pwm_set_duty(3, 0);      // 0%占空比
        // 显示状态
        pwm_display_status();
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        // IO0=0%, IO1=100%, IO2=0%, IO3=100%
        pwm_set_duty(0, 0);      // 0%占空比
        pwm_set_duty(1, 1023);  // 100%占空比
        pwm_set_duty(2, 0);      // 0%占空比
        pwm_set_duty(3, 1023);  // 100%占空比
        // 显示状态
        pwm_display_status();
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        // IO0=50%, IO1=50%, IO2=50%, IO3=50%
        pwm_set_duty(0, 512);    // 50%占空比
        pwm_set_duty(1, 512);    // 50%占空比
        pwm_set_duty(2, 512);    // 50%占空比
        pwm_set_duty(3, 512);    // 50%占空比
        // 显示状态
        pwm_display_status();
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        // IO0=0%, IO1=0%, IO2=0%, IO3=0%
        pwm_set_duty(0, 0);      // 0%占空比
        pwm_set_duty(1, 0);      // 0%占空比
        pwm_set_duty(2, 0);      // 0%占空比
        pwm_set_duty(3, 0);      // 0%占空比
        // 显示状态
        pwm_display_status();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    // 初始化OLED
    oled_init();
      
    // 初始化IO
    init_io();
    
    // 控制IO
    control_io();
}

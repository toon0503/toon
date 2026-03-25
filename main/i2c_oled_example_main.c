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
        
        // 停止：IO0-3的PWM都为0
        oled_clear();
        oled_draw_string(0, 0, "Stop");
        oled_refresh();
        vTaskDelay(pdMS_TO_TICKS(500));
        pwm_set_duty(0, 0);      // 0%占空比
        pwm_set_duty(1, 0);      // 0%占空比
        pwm_set_duty(2, 0);      // 0%占空比
        pwm_set_duty(3, 0);      // 0%占空比
        // 显示状态
        pwm_display_status();
        vTaskDelay(pdMS_TO_TICKS(2000));
        
        // 前进：IO1和IO2的PWM为500，IO0和IO3的PWM为0
        oled_clear();
        oled_draw_string(0, 0, "Forward");
        oled_refresh();
        vTaskDelay(pdMS_TO_TICKS(500));
        pwm_set_duty(0, 0);      // 0%占空比
        pwm_set_duty(1, 500);    // 500占空比
        pwm_set_duty(2, 500);    // 500占空比
        pwm_set_duty(3, 0);      // 0%占空比
        // 显示状态
        pwm_display_status();
        vTaskDelay(pdMS_TO_TICKS(2000));
        
        // 后退：IO2和IO3的PWM为500，IO0和IO1的PWM为0
        oled_clear();
        oled_draw_string(0, 0, "Backward");
        oled_refresh();
        vTaskDelay(pdMS_TO_TICKS(500));
        pwm_set_duty(0, 500);      // 0%占空比
        pwm_set_duty(1, 0);      // 0%占空比
        pwm_set_duty(2, 0);    // 500占空比
        pwm_set_duty(3, 500);    // 500占空比
        // 显示状态
        pwm_display_status();
        vTaskDelay(pdMS_TO_TICKS(2000));
        
        // 左转：IO0-2的PWM为0，IO3的PWM为500
        oled_clear();
        oled_draw_string(0, 0, "Left");
        oled_refresh();
        vTaskDelay(pdMS_TO_TICKS(500));
        pwm_set_duty(0, 0);      // 0%占空比
        pwm_set_duty(1, 0);      // 0%占空比
        pwm_set_duty(2, 0);      // 0%占空比
        pwm_set_duty(3, 500);    // 500占空比
        // 显示状态
        pwm_display_status();
        vTaskDelay(pdMS_TO_TICKS(2000));
        
        // 右转：IO1-3的PWM为0，IO0的PWM为500
        oled_clear();
        oled_draw_string(0, 0, "Right");
        oled_refresh();
        vTaskDelay(pdMS_TO_TICKS(500));
        pwm_set_duty(0, 500);    // 500占空比
        pwm_set_duty(1, 0);      // 0%占空比
        pwm_set_duty(2, 0);      // 0%占空比
        pwm_set_duty(3, 0);      // 0%占空比
        // 显示状态
        pwm_display_status();
        vTaskDelay(pdMS_TO_TICKS(2000));
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

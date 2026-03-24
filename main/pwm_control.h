#ifndef PWM_CONTROL_H
#define PWM_CONTROL_H

#include <stdint.h>

// IO定义
#define IO0_PIN 0
#define IO1_PIN 1
#define IO2_PIN 2
#define IO3_PIN 3

// 初始化PWM控制
void pwm_init(void);

// 设置IO的PWM占空比
// io_num: IO引脚号
// duty: 占空比 (0-1023)
void pwm_set_duty(uint8_t io_num, uint32_t duty);

// 获取IO的当前PWM占空比
// io_num: IO引脚号
// 返回: 占空比 (0-1023)
uint32_t pwm_get_duty(uint8_t io_num);

// 获取IO的当前PWM占空比百分比
// io_num: IO引脚号
// 返回: 百分比 (0-100)
int pwm_get_duty_percent(uint8_t io_num);

// 显示所有IO的PWM状态
void pwm_display_status(void);

#endif // PWM_CONTROL_H

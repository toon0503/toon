#include "oled.h"
#include <math.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// 屏幕分辨率定义
#define EXAMPLE_LCD_H_RES 128
#define EXAMPLE_LCD_V_RES 64

// 函数前向声明

// 空心圆渐变到五角星动画
void test_circle_to_star_animation(void)
{
    int x = 64, y = 32;  // 中心点
    int radius = 20;        // 半径
    
    // 第一阶段：显示空心圆
    oled_clear();
    oled_draw_circle(x, y, radius);
    oled_refresh();
    vTaskDelay(pdMS_TO_TICKS(500));
    
    // 第二阶段：渐变动画（从圆到五角星）
    for (int i = 0; i <= 10; i++) {
        oled_clear();
        
        // 绘制过渡形状
        float ratio = (float)i / 10.0;  // 渐变比例 0.0 -> 1.0
        
        // 计算过渡顶点
        int points[10]; // 存储5个顶点坐标
        float angle = -90.0 * 3.1415926 / 180.0; // 从-90度开始（顶点朝上）
        float step = 72.0 * 3.1415926 / 180.0; // 72度的弧度
        
        for (int j = 0; j < 5; j++) {
            // 外顶点：从圆形逐渐变为五角星外顶点
            float outer_r = radius * (1.0 - ratio * 0.618); // 圆形半径 -> 五角星外顶点
            points[j*2] = x + (int)(outer_r * cos(angle));
            points[j*2+1] = y + (int)(outer_r * sin(angle));
            angle += step;
        }
        
        // 绘制过渡形状（跳过一个顶点连接）
        oled_draw_line(points[0], points[1], points[4], points[5]);
        oled_draw_line(points[4], points[5], points[8], points[9]);
        oled_draw_line(points[8], points[9], points[2], points[3]);
        oled_draw_line(points[2], points[3], points[6], points[7]);
        oled_draw_line(points[6], points[7], points[0], points[1]);
        
        oled_refresh();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
    // 第三阶段：显示完整五角星
    oled_clear();
    oled_draw_star(x, y, radius);
    oled_refresh();
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    oled_clear();
    vTaskDelay(pdMS_TO_TICKS(500));
}

// 五角星顺时针旋转动画（1度/秒）
void test_star_rotation(void)
{
    int x = 64, y = 32;  // 中心点
    int radius = 20;        // 半径
    float current_angle = -90.0 * 3.1415926 / 180.0; // 初始角度（顶点朝上）
    float rotation_speed = 18.0 * 3.1415926 / 180.0; // 旋转速度：1度/秒
    float step = 72.0 * 3.1415926 / 180.0; // 72度的弧度
    
    // 旋转360度
    for (int frame = 0; frame < 360; frame++) {
        oled_clear();
        
        // 计算当前角度的五角星顶点
        int points[10]; // 存储5个外顶点坐标
        float angle = current_angle;
        
        for (int i = 0; i < 5; i++) {
            points[i*2] = x + (int)(radius * cos(angle));
            points[i*2+1] = y + (int)(radius * sin(angle));
            angle += step;
        }
        
        // 绘制五角星（跳过一个顶点连接）
        oled_draw_line(points[0], points[1], points[4], points[5]);
        oled_draw_line(points[4], points[5], points[8], points[9]);
        oled_draw_line(points[8], points[9], points[2], points[3]);
        oled_draw_line(points[2], points[3], points[6], points[7]);
        oled_draw_line(points[6], points[7], points[0], points[1]);
        
        oled_refresh();
        
        // 更新角度（顺时针旋转）
        current_angle += rotation_speed;
        
        // 延迟1秒（1度/秒）
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    
    oled_clear();
    vTaskDelay(pdMS_TO_TICKS(500));
}




void app_main(void)
{
    // 初始化OLED
    oled_init();
    oled_clear();
    oled_draw_string(0, 0, "Initializing OLED...");
    oled_refresh();
    vTaskDelay(pdMS_TO_TICKS(2000));
    
    // 测试空心圆渐变到五角星动画

    
    // 测试五角星旋转动画
    test_star_rotation();
    
    while(1)
    {

    }
}


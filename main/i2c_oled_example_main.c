#include "oled.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// 屏幕分辨率定义
#define EXAMPLE_LCD_H_RES 128
#define EXAMPLE_LCD_V_RES 64

// 函数前向声明
void test_ellipse_animation(void);



void app_main(void)
{
    // 初始化OLED
    oled_init();
    oled_clear();
    oled_draw_string(0, 0, "Initializing OLED...");
    oled_refresh();
    vTaskDelay(pdMS_TO_TICKS(2000));
    oled_clear();
    
    while(1)
    {
        test_ellipse_animation();
    }
}

// 利用 椭圆动画   眨眼的表情
void test_ellipse_animation(void) {
    oled_clear();
    int x1 = 32, y1 = 32;  // 第二个椭圆的初始位置
    int a1 = 20, b1 = 20;  // 第二个椭圆的长半轴和短半轴   
    int x2 = 96, y2 = 32;  // 第二个椭圆的初始位置
    int a2 = 20, b2 = 20;  // 第二个椭圆的长半轴和短半轴
    oled_fill_ellipse(x1, y1, a1, b1);    // 左侧椭圆
    oled_fill_ellipse(x2, y2, a2, b2);    // 右侧椭圆
    oled_refresh();      
     vTaskDelay(pdMS_TO_TICKS(100));  
    // 第一阶段：x轴从20增加到40，y轴从20减少到5
    for (int i = 0; i < 5; i++) {
        oled_clear();
                // 绘制左侧圆和右侧椭圆

        oled_fill_ellipse(x1, y1, a1, b1);    // 左侧椭圆
        oled_fill_ellipse(x2, y2, a2, b2);    // 右侧椭圆
        
        oled_refresh();
        vTaskDelay(pdMS_TO_TICKS(20));
        
        // 更新右侧椭圆的尺寸
        if (a2 < 40) {
            a2 = a2 + 4;  // x轴逐步增加到40
        }
        if (b2 > 5) {
            b2 = b2 - 3;  // y轴逐步减少到5
        }
    }
    
    // 第二阶段：x轴从40恢复到20，y轴从5恢复到20
     for (int i = 0; i < 5; i++) {
        oled_clear();
        
        // 绘制左侧圆和右侧椭圆
        oled_fill_ellipse(x1, y1,  b1, a1);    // 左侧椭圆
        oled_fill_ellipse(x2, y2, a2, b2);    // 右侧椭圆
        
        oled_refresh();
        vTaskDelay(pdMS_TO_TICKS(30));
        
        // 更新右侧椭圆的尺寸（恢复）
        if (a2 > 20) {
            a2 = a2 - 4;  // x轴逐步恢复到20
        }
        if (b2 < 20) {
            b2 = b2 + 3;  // y轴逐步恢复到20
        }
    } 
    
    vTaskDelay(pdMS_TO_TICKS(100));
    oled_clear();
    oled_fill_ellipse(x1, y1, a1, b1);    // 左侧椭圆
    oled_fill_ellipse(x2, y2, a2, b2);    // 右侧椭圆
    oled_refresh(); 
    vTaskDelay(pdMS_TO_TICKS(100));
}
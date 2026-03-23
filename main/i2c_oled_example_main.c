#include "oled.h"
#include <math.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"





void app_main(void)
{
    // 初始化OLED
    oled_init();
    oled_clear();
    oled_draw_string(0, 0, "Initializing OLED...");
    oled_refresh();
    vTaskDelay(pdMS_TO_TICKS(2000));
    

    
    while(1)
    {

    }
}


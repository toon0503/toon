#include "oled.h"
#include "font8x8.h"
#include "font16x16.h"
#include <math.h>
#include <string.h>
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "driver/i2c_master.h"
#include "esp_lcd_panel_vendor.h"

#define I2C_BUS_PORT  0             // I2C总线端口号

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LCD spec //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ    (400 * 1000)  // LCD像素时钟频率：400kHz
#define EXAMPLE_PIN_NUM_SDA           1             // I2C SDA引脚号
#define EXAMPLE_PIN_NUM_SCL           0             // I2C SCL引脚号
#define EXAMPLE_PIN_NUM_RST           -1            // 复位引脚号，-1表示不使用
#define EXAMPLE_I2C_HW_ADDR           0x3C          // I2C设备地址

#define EXAMPLE_LCD_H_RES              128          // 水平分辨率：128像素
#define EXAMPLE_LCD_V_RES              64           // 垂直分辨率：64像素

#define EXAMPLE_LCD_CMD_BITS           8             // 命令位数：8位
#define EXAMPLE_LCD_PARAM_BITS         8             // 参数位数：8位

// OLED显示缓冲区
static uint8_t oled_buffer[EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES / 8];

// OLED面板句柄
static esp_lcd_panel_handle_t g_panel_handle = NULL;

// 初始化I2C和OLED
void oled_init(void)
{
    i2c_master_bus_handle_t i2c_bus = NULL;
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .i2c_port = I2C_BUS_PORT,
        .sda_io_num = EXAMPLE_PIN_NUM_SDA,
        .scl_io_num = EXAMPLE_PIN_NUM_SCL,
        .flags.enable_internal_pullup = true,
    };
    i2c_new_master_bus(&bus_config, &i2c_bus);

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = EXAMPLE_I2C_HW_ADDR,
        .scl_speed_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
        .control_phase_bytes = 1,
        .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,
        .lcd_param_bits = EXAMPLE_LCD_CMD_BITS,
        .dc_bit_offset = 6,
    };
    esp_lcd_new_panel_io_i2c(i2c_bus, &io_config, &io_handle);

    esp_lcd_panel_dev_config_t panel_config = {
        .bits_per_pixel = 1,
        .reset_gpio_num = EXAMPLE_PIN_NUM_RST,
    };
    esp_lcd_panel_ssd1306_config_t ssd1306_config = {
        .height = EXAMPLE_LCD_V_RES,
    };
    panel_config.vendor_config = &ssd1306_config;
    esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &g_panel_handle);

    esp_lcd_panel_reset(g_panel_handle);
    esp_lcd_panel_init(g_panel_handle);
    esp_lcd_panel_disp_on_off(g_panel_handle, true);
}

// 清空显示缓冲区
void oled_clear(void)
{
    memset(oled_buffer, 0, sizeof(oled_buffer));
}

// 在指定位置绘制一个像素
void oled_draw_pixel(int x, int y, int color)
{
    if (x < 0 || x >= EXAMPLE_LCD_H_RES || y < 0 || y >= EXAMPLE_LCD_V_RES) {
        return;
    }
    int byte_index = (y / 8) * EXAMPLE_LCD_H_RES + x;
    int bit_index = y % 8;
    if (color) {
        oled_buffer[byte_index] |= (1 << bit_index);
    } else {
        oled_buffer[byte_index] &= ~(1 << bit_index);
    }
}

// 在指定位置绘制一个字符
void oled_draw_char(int x, int y, char c)
{
    const uint8_t *char_data = font8x8_get_char(c);
    for (int i = 0; i < FONT_HEIGHT; i++) {
        uint8_t line = char_data[i];
        for (int j = 0; j < FONT_WIDTH; j++) {
            int pixel = (line >> (7 - j)) & 1;
            oled_draw_pixel(x + j, y + i, pixel);
        }
    }
}

// 在指定位置通过汉字名称绘制一个汉字 (16x16)
int oled_draw_chinese_by_name(int x, int y, const char *name)
{
    const GBK16_FontTypeDef *font = font16x16_get_by_name(name);
    if (font == NULL) {
        return -1;
    }

    const uint8_t *char_data = font->Bitmap;
    for (int col = 0; col < 16; col++) {
        uint8_t byte1 = char_data[col * 2];
        uint8_t byte2 = char_data[col * 2 + 1];
        for (int row = 0; row < 8; row++) {
            int pixel = (byte1 >> (7 - row)) & 1;
            oled_draw_pixel(x + col, y + row, pixel);
        }
        for (int row = 0; row < 8; row++) {
            int pixel = (byte2 >> (7 - row)) & 1;
            oled_draw_pixel(x + col, y + 8 + row, pixel);
        }
    }
    return 0;
}

// 在指定位置绘制字符串（支持自动换行）
void oled_draw_string(int x, int y, const char *str)
{
    int cursor_x = x;
    int cursor_y = y;
    int chars_per_line = (EXAMPLE_LCD_H_RES - x) / FONT_WIDTH;
    int current_char = 0;

    while (*str) {
        if (current_char >= chars_per_line) {
            cursor_x = 0;
            cursor_y += FONT_HEIGHT;
            current_char = 0;
            chars_per_line = EXAMPLE_LCD_H_RES / FONT_WIDTH;
            if (cursor_y + FONT_HEIGHT > EXAMPLE_LCD_V_RES) {
                break;
            }
        }
        oled_draw_char(cursor_x, cursor_y, *str);
        cursor_x += FONT_WIDTH;
        current_char++;
        str++;
    }
}

// 在指定位置绘制中英文混合字符串（支持自动换行）
void oled_draw_string_cn(int x, int y, const char *str)
{
    int cursor_x = x;
    int cursor_y = y;
    int line_height = FONT_HEIGHT; // 默认使用ASCII字符高度
    int use_cn_height = 0; // 标记是否需要使用中文字符高度

    while (*str) {
        uint8_t c = (uint8_t)*str;

        // 判断是否为UTF-8编码的汉字（首字节0xE0-0xEF）
        if (c >= 0xE0 && c <= 0xEF) {
            // UTF-8汉字（3字节）
            if (str[1] == '\0' || str[2] == '\0') break;
            
            // 标记需要使用中文字符高度
            use_cn_height = 1;
            line_height = FONT_CN_HEIGHT;
            
            // 检查是否需要换行
            if (cursor_x + FONT_CN_WIDTH > EXAMPLE_LCD_H_RES) {
                cursor_x = 0;
                // 按中文字符高度换行
                cursor_y += line_height;
                if (cursor_y + line_height > EXAMPLE_LCD_V_RES) {
                    break;
                }
            }

            // 构造汉字名称字符串（使用UTF-8编码）
            char chn_name[4] = {c, str[1], str[2], 0};
            // 绘制汉字
            if (oled_draw_chinese_by_name(cursor_x, cursor_y, chn_name) == 0) {
                cursor_x += FONT_CN_WIDTH;
            } else {
                // 找不到字模，显示问号
                oled_draw_char(cursor_x, cursor_y + (line_height - FONT_HEIGHT), '?');
                cursor_x += FONT_WIDTH;
            }
            str += 3;
        }
        // ASCII字符
        else if (c < 0x80) {
            // 检查是否需要换行
            if (cursor_x + FONT_WIDTH > EXAMPLE_LCD_H_RES) {
                cursor_x = 0;
                // 按当前行高换行（如果之前有汉字，使用中文字符高度）
                cursor_y += line_height;
                if (cursor_y + line_height > EXAMPLE_LCD_V_RES) {
                    break;
                }
            }

            // 绘制ASCII字符，与汉字底对齐
            oled_draw_char(cursor_x, cursor_y + (line_height - FONT_HEIGHT), *str);
            cursor_x += FONT_WIDTH;
            str++;
        }
        // 其他情况，跳过
        else {
            str++;
        }
    }
}

// 刷新显示缓冲区到OLED屏幕
void oled_refresh(void)
{
    esp_lcd_panel_draw_bitmap(g_panel_handle, 0, 0, EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES, oled_buffer);
}

// 绘制直线（Bresenham算法）
void oled_draw_line(int x1, int y1, int x2, int y2)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (1) {
        oled_draw_pixel(x1, y1, 1);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// 绘制矩形
void oled_draw_rect(int x1, int y1, int x2, int y2)
{
    // 确保x1 < x2, y1 < y2
    if (x1 > x2) {
        int temp = x1;
        x1 = x2;
        x2 = temp;
    }
    if (y1 > y2) {
        int temp = y1;
        y1 = y2;
        y2 = temp;
    }

    // 绘制四条边
    for (int x = x1; x <= x2; x++) {
        oled_draw_pixel(x, y1, 1);
        oled_draw_pixel(x, y2, 1);
    }
    for (int y = y1; y <= y2; y++) {
        oled_draw_pixel(x1, y, 1);
        oled_draw_pixel(x2, y, 1);
    }
}

// 绘制实心矩形
void oled_fill_rect(int x1, int y1, int x2, int y2)
{
    // 确保x1 < x2, y1 < y2
    if (x1 > x2) {
        int temp = x1;
        x1 = x2;
        x2 = temp;
    }
    if (y1 > y2) {
        int temp = y1;
        y1 = y2;
        y2 = temp;
    }

    // 填充矩形
    for (int y = y1; y <= y2; y++) {
        for (int x = x1; x <= x2; x++) {
            oled_draw_pixel(x, y, 1);
        }
    }
}

// 绘制圆形（Bresenham算法）
void oled_draw_circle(int x0, int y0, int radius)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        oled_draw_pixel(x0 + x, y0 + y, 1);
        oled_draw_pixel(x0 + y, y0 + x, 1);
        oled_draw_pixel(x0 - y, y0 + x, 1);
        oled_draw_pixel(x0 - x, y0 + y, 1);
        oled_draw_pixel(x0 - x, y0 - y, 1);
        oled_draw_pixel(x0 - y, y0 - x, 1);
        oled_draw_pixel(x0 + y, y0 - x, 1);
        oled_draw_pixel(x0 + x, y0 - y, 1);

        if (err <= 0) {
            y++;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x--;
            err -= 2 * x + 1;
        }
    }
}

// 绘制实心圆形
void oled_fill_circle(int x0, int y0, int radius)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        // 绘制水平扫描线
        for (int i = x0 - x; i <= x0 + x; i++) {
            oled_draw_pixel(i, y0 + y, 1);
            oled_draw_pixel(i, y0 - y, 1);
        }
        for (int i = x0 - y; i <= x0 + y; i++) {
            oled_draw_pixel(i, y0 + x, 1);
            oled_draw_pixel(i, y0 - x, 1);
        }

        if (err <= 0) {
            y++;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x--;
            err -= 2 * x + 1;
        }
    }
}

// 绘制三角形
void oled_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
    oled_draw_line(x1, y1, x2, y2);
    oled_draw_line(x2, y2, x3, y3);
    oled_draw_line(x3, y3, x1, y1);
}

// 绘制实心三角形（扫描线填充算法）
void oled_fill_triangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
    // 排序顶点按y坐标从小到大
    if (y1 > y2) {
        int temp = y1; y1 = y2; y2 = temp;
        temp = x1; x1 = x2; x2 = temp;
    }
    if (y2 > y3) {
        int temp = y2; y2 = y3; y3 = temp;
        temp = x2; x2 = x3; x3 = temp;
    }
    if (y1 > y2) {
        int temp = y1; y1 = y2; y2 = temp;
        temp = x1; x1 = x2; x2 = temp;
    }

    // 计算边的斜率倒数
    float m12 = (y2 - y1) != 0 ? (float)(x2 - x1) / (y2 - y1) : 0;
    float m23 = (y3 - y2) != 0 ? (float)(x3 - x2) / (y3 - y2) : 0;
    float m13 = (y3 - y1) != 0 ? (float)(x3 - x1) / (y3 - y1) : 0;

    // 填充三角形
    for (int y = y1; y <= y3; y++) {
        int x_left, x_right;
        if (y <= y2) {
            x_left = x1 + (y - y1) * m12;
            x_right = x1 + (y - y1) * m13;
        } else {
            x_left = x2 + (y - y2) * m23;
            x_right = x1 + (y - y1) * m13;
        }
        if (x_left > x_right) {
            int temp = x_left;
            x_left = x_right;
            x_right = temp;
        }
        for (int x = x_left; x <= x_right; x++) {
            oled_draw_pixel(x, y, 1);
        }
    }
}

// 绘制圆角矩形
void oled_draw_round_rect(int x1, int y1, int x2, int y2, int radius)
{
    // 确保x1 < x2, y1 < y2
    if (x1 > x2) {
        int temp = x1;
        x1 = x2;
        x2 = temp;
    }
    if (y1 > y2) {
        int temp = y1;
        y1 = y2;
        y2 = temp;
    }

    // 限制圆角半径
    int max_radius = (x2 - x1) / 2;
    if (radius > max_radius) radius = max_radius;
    max_radius = (y2 - y1) / 2;
    if (radius > max_radius) radius = max_radius;

    // 绘制四条边
    for (int x = x1 + radius; x <= x2 - radius; x++) {
        oled_draw_pixel(x, y1, 1);
        oled_draw_pixel(x, y2, 1);
    }
    for (int y = y1 + radius; y <= y2 - radius; y++) {
        oled_draw_pixel(x1, y, 1);
        oled_draw_pixel(x2, y, 1);
    }

    // 绘制四个圆角（绘制四分之一圆弧）
    int r = radius;
    int x, y, err = 0;
    
    // 左上角圆角（绘制右下象限）
    x = r; y = 0; err = 0;
    while (x >= y) {
        oled_draw_pixel(x1 + r - x, y1 + r - y, 1);
        oled_draw_pixel(x1 + r - y, y1 + r - x, 1);
        
        if (err <= 0) {
            y++;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x--;
            err -= 2 * x + 1;
        }
    }
    
    // 右上角圆角（绘制左下象限）
    x = r; y = 0; err = 0;
    while (x >= y) {
        oled_draw_pixel(x2 - r + x, y1 + r - y, 1);
        oled_draw_pixel(x2 - r + y, y1 + r - x, 1);
        
        if (err <= 0) {
            y++;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x--;
            err -= 2 * x + 1;
        }
    }
    
    // 左下角圆角（绘制右上象限）
    x = r; y = 0; err = 0;
    while (x >= y) {
        oled_draw_pixel(x1 + r - x, y2 - r + y, 1);
        oled_draw_pixel(x1 + r - y, y2 - r + x, 1);
        
        if (err <= 0) {
            y++;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x--;
            err -= 2 * x + 1;
        }
    }
    
    // 右下角圆角（绘制左上象限）
    x = r; y = 0; err = 0;
    while (x >= y) {
        oled_draw_pixel(x2 - r + x, y2 - r + y, 1);
        oled_draw_pixel(x2 - r + y, y2 - r + x, 1);
        
        if (err <= 0) {
            y++;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x--;
            err -= 2 * x + 1;
        }
    }
}

// 绘制实心圆角矩形
void oled_fill_round_rect(int x1, int y1, int x2, int y2, int radius)
{
    // 确保x1 < x2, y1 < y2
    if (x1 > x2) {
        int temp = x1;
        x1 = x2;
        x2 = temp;
    }
    if (y1 > y2) {
        int temp = y1;
        y1 = y2;
        y2 = temp;
    }

    // 限制圆角半径
    int max_radius = (x2 - x1) / 2;
    if (radius > max_radius) radius = max_radius;
    max_radius = (y2 - y1) / 2;
    if (radius > max_radius) radius = max_radius;

    // 填充中间矩形部分
    oled_fill_rect(x1 + radius, y1, x2 - radius, y2);
    
    // 填充左右两侧矩形部分
    oled_fill_rect(x1, y1 + radius, x2, y2 - radius);

    // 填充四个圆角
    int r = radius;
    int x, y, err = 0;
    
    // 左上角圆角（填充右下象限）
    x = r; y = 0; err = 0;
    while (x >= y) {
        for (int i = x1 + r - x; i <= x1 + r; i++) {
            oled_draw_pixel(i, y1 + r - y, 1);
        }
        for (int i = x1 + r - y; i <= x1 + r; i++) {
            oled_draw_pixel(i, y1 + r - x, 1);
        }
        
        if (err <= 0) {
            y++;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x--;
            err -= 2 * x + 1;
        }
    }
    
    // 右上角圆角（填充左下象限）
    x = r; y = 0; err = 0;
    while (x >= y) {
        for (int i = x2 - r; i <= x2 - r + x; i++) {
            oled_draw_pixel(i, y1 + r - y, 1);
        }
        for (int i = x2 - r; i <= x2 - r + y; i++) {
            oled_draw_pixel(i, y1 + r - x, 1);
        }
        
        if (err <= 0) {
            y++;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x--;
            err -= 2 * x + 1;
        }
    }
    
    // 左下角圆角（填充右上象限）
    x = r; y = 0; err = 0;
    while (x >= y) {
        for (int i = x1 + r - x; i <= x1 + r; i++) {
            oled_draw_pixel(i, y2 - r + y, 1);
        }
        for (int i = x1 + r - y; i <= x1 + r; i++) {
            oled_draw_pixel(i, y2 - r + x, 1);
        }
        
        if (err <= 0) {
            y++;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x--;
            err -= 2 * x + 1;
        }
    }
    
    // 右下角圆角（填充左上象限）
    x = r; y = 0; err = 0;
    while (x >= y) {
        for (int i = x2 - r; i <= x2 - r + x; i++) {
            oled_draw_pixel(i, y2 - r + y, 1);
        }
        for (int i = x2 - r; i <= x2 - r + y; i++) {
            oled_draw_pixel(i, y2 - r + x, 1);
        }
        
        if (err <= 0) {
            y++;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x--;
            err -= 2 * x + 1;
        }
    }
}

// 绘制椭圆
void oled_draw_ellipse(int x0, int y0, int a, int b)
{
    int x = 0;
    int y = b;
    int a2 = a * a;
    int b2 = b * b;
    int two_a2 = 2 * a2;
    int two_b2 = 2 * b2;
    int err = 0;
    int dx = 0;
    int dy = two_a2 * y;

    while (dy >= dx) {
        oled_draw_pixel(x0 + x, y0 + y, 1);
        oled_draw_pixel(x0 - x, y0 + y, 1);
        oled_draw_pixel(x0 + x, y0 - y, 1);
        oled_draw_pixel(x0 - x, y0 - y, 1);

        if (err > 0) {
            y--;
            dy -= two_b2;
            err -= two_a2 * y + a2;
        }
        if (err <= 0) {
            x++;
            dx += two_b2;
            err += two_b2 * x + b2;
        }
    }
}

// 绘制实心椭圆
void oled_fill_ellipse(int x0, int y0, int a, int b)
{
    int x = 0;
    int y = b;
    int a2 = a * a;
    int b2 = b * b;
    int two_a2 = 2 * a2;
    int two_b2 = 2 * b2;
    int err = 0;
    int dx = 0;
    int dy = two_a2 * y;

    while (y >= 0) {
        int x_start = x0 - x;
        int x_end = x0 + x;
        
        for (int i = x_start; i <= x_end; i++) {
            oled_draw_pixel(i, y0 + y, 1);
            if (y != 0) {
                oled_draw_pixel(i, y0 - y, 1);
            }
        }

        if (err > 0) {
            y--;
            dy -= two_b2;
            err -= two_a2 * y + a2;
        }
        if (err <= 0) {
            x++;
            dx += two_b2;
            err += two_b2 * x + b2;
        }
    }
}

// 绘制五角星
void oled_draw_star(int x0, int y0, int radius)
{
    int points[10]; // 存储5个外顶点坐标
    float angle = -90.0 * 3.1415926 / 180.0; // 从-90度开始（顶点朝上）
    float step = 72.0 * 3.1415926 / 180.0; // 72度的弧度
    
    // 计算五角星的5个外顶点坐标
    for (int i = 0; i < 5; i++) {
        points[i*2] = x0 + (int)(radius * cos(angle));
        points[i*2+1] = y0 + (int)(radius * sin(angle));
        angle += step;
    }
    
    // 连接顶点绘制五角星（跳过一个顶点）
    oled_draw_line(points[0], points[1], points[4], points[5]);
    oled_draw_line(points[4], points[5], points[8], points[9]);
    oled_draw_line(points[8], points[9], points[2], points[3]);
    oled_draw_line(points[2], points[3], points[6], points[7]);
    oled_draw_line(points[6], points[7], points[0], points[1]);
}

// 绘制实心五角星
void oled_fill_star(int x0, int y0, int radius)
{
    int outer_points[10]; // 存储5个外顶点坐标
    int inner_points[10]; // 存储5个内顶点坐标
    float angle = -90.0 * 3.1415926 / 180.0; // 从-90度开始（顶点朝上）
    float step = 72.0 * 3.1415926 / 180.0; // 72度的弧度
    
    // 计算五角星的5个外顶点坐标
    for (int i = 0; i < 5; i++) {
        outer_points[i*2] = x0 + (int)(radius * cos(angle));
        outer_points[i*2+1] = y0 + (int)(radius * sin(angle));
        angle += step;
    }
    
    // 计算5个内顶点坐标（内接圆，半径为外接圆的0.382倍）
    angle = -90.0 * 3.1415926 / 180.0 + step / 2; // 内顶点从外顶点之间开始
    for (int i = 0; i < 5; i++) {
        inner_points[i*2] = x0 + (int)(radius * 0.382 * cos(angle));
        inner_points[i*2+1] = y0 + (int)(radius * 0.382 * sin(angle));
        angle += step;
    }
    
    // 填充五角星（每个角由一个外顶点和两个相邻的内顶点组成三角形）
    oled_fill_triangle(outer_points[0], outer_points[1], inner_points[0], inner_points[1], inner_points[4], inner_points[5]);
    oled_fill_triangle(outer_points[2], outer_points[3], inner_points[2], inner_points[3], inner_points[0], inner_points[1]);
    oled_fill_triangle(outer_points[4], outer_points[5], inner_points[4], inner_points[5], inner_points[2], inner_points[3]);
    oled_fill_triangle(outer_points[6], outer_points[7], inner_points[6], inner_points[7], inner_points[4], inner_points[5]);
    oled_fill_triangle(outer_points[8], outer_points[9], inner_points[8], inner_points[9], inner_points[6], inner_points[7]);
    
    // 填充中心的五边形（用多个三角形填充）
    oled_fill_triangle(inner_points[0], inner_points[1], inner_points[2], inner_points[3], inner_points[4], inner_points[5]);
    oled_fill_triangle(inner_points[0], inner_points[1], inner_points[4], inner_points[5], inner_points[6], inner_points[7]);
    oled_fill_triangle(inner_points[0], inner_points[1], inner_points[6], inner_points[7], inner_points[8], inner_points[9]);
}

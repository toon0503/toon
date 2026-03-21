#ifndef OLED_H
#define OLED_H

#include <stdint.h>

// 初始化OLED
void oled_init(void);

// 清空显示缓冲区
void oled_clear(void);

// 在指定位置绘制一个像素
// x: 横坐标 (0-127)
// y: 纵坐标 (0-63)
// color: 0-灭, 1-亮
void oled_draw_pixel(int x, int y, int color);

// 在指定位置绘制一个字符
// x: 横坐标 (0-127)
// y: 纵坐标 (0-63)
// c: ASCII字符
void oled_draw_char(int x, int y, char c);

// 在指定位置绘制字符串（支持自动换行）
// x: 起始横坐标 (0-127)
// y: 起始纵坐标 (0-63)
// str: 字符串指针
void oled_draw_string(int x, int y, const char *str);

// 在指定位置通过汉字名称绘制一个汉字 (16x16)
// x: 横坐标 (0-127)
// y: 纵坐标 (0-63)
// name: 汉字名称（UTF-8编码）
// 返回: 0-成功, -1-找不到字模
int oled_draw_chinese_by_name(int x, int y, const char *name);

// 在指定位置绘制中英文混合字符串（支持自动换行）
// x: 起始横坐标 (0-127)
// y: 起始纵坐标 (0-63)
// str: 字符串指针 (UTF-8编码)
void oled_draw_string_cn(int x, int y, const char *str);

// 绘制直线
// x1, y1: 起点坐标
// x2, y2: 终点坐标
void oled_draw_line(int x1, int y1, int x2, int y2);

// 绘制矩形
// x1, y1: 左上角坐标
// x2, y2: 右下角坐标
void oled_draw_rect(int x1, int y1, int x2, int y2);

// 绘制实心矩形
// x1, y1: 左上角坐标
// x2, y2: 右下角坐标
void oled_fill_rect(int x1, int y1, int x2, int y2);

// 绘制圆形
// x0, y0: 圆心坐标
// radius: 半径
void oled_draw_circle(int x0, int y0, int radius);

// 绘制实心圆形
// x0, y0: 圆心坐标
// radius: 半径
void oled_fill_circle(int x0, int y0, int radius);

// 绘制三角形
// x1, y1, x2, y2, x3, y3: 三个顶点坐标
void oled_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3);

// 绘制实心三角形
// x1, y1, x2, y2, x3, y3: 三个顶点坐标
void oled_fill_triangle(int x1, int y1, int x2, int y2, int x3, int y3);

// 绘制圆角矩形
// x1, y1: 左上角坐标
// x2, y2: 右下角坐标
// radius: 圆角半径
void oled_draw_round_rect(int x1, int y1, int x2, int y2, int radius);

// 绘制实心圆角矩形
// x1, y1: 左上角坐标
// x2, y2: 右下角坐标
// radius: 圆角半径
void oled_fill_round_rect(int x1, int y1, int x2, int y2, int radius);

// 绘制椭圆
// x0, y0: 椭圆中心坐标
// a: 长半轴（X方向半径）
// b: 短半轴（Y方向半径）
void oled_draw_ellipse(int x0, int y0, int a, int b);

// 绘制实心椭圆
// x0, y0: 椭圆中心坐标
// a: 长半轴（X方向半径）
// b: 短半轴（Y方向半径）
void oled_fill_ellipse(int x0, int y0, int a, int b);

// 绘制五角星
// x0, y0: 五角星中心坐标
// radius: 五角星外接圆半径
void oled_draw_star(int x0, int y0, int radius);

// 绘制实心五角星
// x0, y0: 五角星中心坐标
// radius: 五角星外接圆半径
void oled_fill_star(int x0, int y0, int radius);

// 刷新显示缓冲区到OLED屏幕
void oled_refresh(void);

#endif // OLED_H

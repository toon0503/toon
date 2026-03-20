#ifndef FONT8X8_H
#define FONT8X8_H

#include <stdint.h>

#define FONT_WIDTH  8    // 字体宽度：8像素
#define FONT_HEIGHT 8    // 字体高度：8像素

// 获取指定字符的字模数据
// 参数: c - ASCII字符 (0x20-0x7F)
// 返回: 字模数据指针，共8字节
const uint8_t* font8x8_get_char(char c);

#endif // FONT8X8_H

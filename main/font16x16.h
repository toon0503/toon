#ifndef FONT16X16_H
#define FONT16X16_H

#include <stdint.h>

#define FONT_CN_WIDTH  16    // 汉字宽度：16像素
#define FONT_CN_HEIGHT 16    // 汉字高度：16像素

// 16x16汉字字模结构体
typedef struct {
    uint8_t  Bitmap[32];     // 16x16点阵数据（32字节）
    char     ChnName[4];     // 汉字名称（UTF-8编码）
} GBK16_FontTypeDef;

// 通过汉字名称（UTF-8）检索字模数据
// 参数: name - 汉字字符串（UTF-8编码）
// 返回: 字模结构体指针，找不到返回NULL
const GBK16_FontTypeDef* font16x16_get_by_name(const char *name);

#endif // FONT16X16_H

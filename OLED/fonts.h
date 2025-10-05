#ifndef __FONTS_H__
#define __FONTS_H__

#include <stdint.h>

// Font 5x7 cơ bản (tương thích với nhiều màn hình nhỏ)
extern const uint8_t Font_5x7[95][5];

// Font 8x16 lớn hơn (cho text dễ đọc hơn)
extern const uint8_t Font_8x16[95][16];

// Khai báo font mặc định để sử dụng
#define DEFAULT_FONT Font_5x7

// Macro để lấy kích thước font
#define FONT_WIDTH(font)  font[0][0]  // Chiều rộng ký tự đầu tiên
#define FONT_HEIGHT(font) 16          // Chiều cao cố định

#endif /* __FONTS_H__ */
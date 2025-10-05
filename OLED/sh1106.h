#ifndef __SH1106_H__
#define __SH1106_H__

#include "stm32f1xx_hal.h"  // Điều chỉnh theo board STM32 của bạn

// Định nghĩa kích thước màn hình SH1106
#define SH1106_WIDTH    128
#define SH1106_HEIGHT   64

// Định nghĩa chân I2C (Điều chỉnh theo board của bạn)
#define SH1106_I2C      hi2c1  // Thay đổi theo I2C peripheral bạn sử dụng

// Địa chỉ I2C của SH1106
#define SH1106_I2C_ADDR 0x78  // Địa chỉ cơ bản của SH1106 (0x3C << 1)

// Các lệnh cơ bản của SH1106
#define SH1106_CMD_DISPLAY_OFF      0xAE
#define SH1106_CMD_DISPLAY_ON       0xAF
#define SH1106_CMD_SET_CONTRAST     0x81
#define SH1106_CMD_SET_NORMAL       0xA6
#define SH1106_CMD_SET_INVERSE      0xA7
#define SH1106_CMD_SET_MULTIPLEX    0xA8
#define SH1106_CMD_SET_DCDC         0xAD
#define SH1106_CMD_SET_PAGE         0xB0
#define SH1106_CMD_SET_COLUMN_LOW   0x00
#define SH1106_CMD_SET_COLUMN_HIGH  0x10
#define SH1106_CMD_SET_START_LINE   0x40
#define SH1106_CMD_SET_SEG_REMAP    0xA1  // Flip horizontally
#define SH1106_CMD_SET_COM_SCAN_DIR 0xC8  // Flip vertically
#define SH1106_CMD_SET_OFFSET       0xD3
#define SH1106_CMD_SET_CLOCK        0xD5
#define SH1106_CMD_SET_PRECHARGE    0xD9
#define SH1106_CMD_SET_VCOMH        0xDB
#define SH1106_CMD_SET_CHARGE_PUMP  0x8D

// Khai báo các hàm chính
void SH1106_Init(void);
void SH1106_SendCommand(uint8_t cmd);
void SH1106_SendData(uint8_t data);
void SH1106_Clear(void);
void SH1106_UpdateScreen(void);
void SH1106_SetCursor(uint8_t page, uint8_t column);
void SH1106_WriteChar(char ch, uint8_t *font);
void SH1106_WriteString(char *str, uint8_t *font);
void SH1106_DrawPixel(uint8_t x, uint8_t y);
void SH1106_WriteCharAt(uint8_t x, uint8_t y, char ch, uint8_t *font);
void SH1106_WriteStringAt(uint8_t x, uint8_t y, char *str, uint8_t *font);

#endif /* __SH1106_H__ */
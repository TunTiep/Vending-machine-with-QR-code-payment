#include "sh1106.h"
#include "fonts.h"  // Bao gồm font library

// Biến toàn cục để lưu trữ buffer màn hình
static uint8_t OLED_Buffer[SH1106_WIDTH * SH1106_HEIGHT / 8];

/**
 * @brief Khởi tạo màn hình SH1106
 */
void SH1106_Init(void)
{
    // Đợi một chút để màn hình sẵn sàng
    HAL_Delay(100);

    // Gửi các lệnh khởi tạo
    SH1106_SendCommand(SH1106_CMD_DISPLAY_OFF);

    SH1106_SendCommand(SH1106_CMD_SET_DCDC);
    SH1106_SendCommand(0x8B);  // Enable DC-DC

    SH1106_SendCommand(SH1106_CMD_SET_CLOCK);
    SH1106_SendCommand(0x80);  // Set clock

    SH1106_SendCommand(SH1106_CMD_SET_MULTIPLEX);
    SH1106_SendCommand(0x3F);  // 1/64 duty

    SH1106_SendCommand(SH1106_CMD_SET_OFFSET);
    SH1106_SendCommand(0x00);  // No offset

    SH1106_SendCommand(SH1106_CMD_SET_START_LINE | 0x00);

    SH1106_SendCommand(SH1106_CMD_SET_CHARGE_PUMP);
    SH1106_SendCommand(0x14);  // Enable charge pump

    SH1106_SendCommand(SH1106_CMD_SET_SEG_REMAP);  // Flip horizontally

    SH1106_SendCommand(SH1106_CMD_SET_COM_SCAN_DIR);  // Flip vertically

    SH1106_SendCommand(SH1106_CMD_SET_CONTRAST);
    SH1106_SendCommand(0x7F);  // Set contrast

    SH1106_SendCommand(SH1106_CMD_SET_PRECHARGE);
    SH1106_SendCommand(0x22);  // Set pre-charge period

    SH1106_SendCommand(SH1106_CMD_SET_VCOMH);
    SH1106_SendCommand(0x35);  // Set VCOMH

    SH1106_SendCommand(SH1106_CMD_SET_NORMAL);

    // Xóa màn hình
    SH1106_Clear();

    // Bật màn hình
    SH1106_SendCommand(SH1106_CMD_DISPLAY_ON);
}

/**
 * @brief Gửi lệnh đến SH1106
 * @param cmd Lệnh cần gửi
 */
void SH1106_SendCommand(uint8_t cmd)
{
    uint8_t data[2];
    data[0] = 0x00;  // Co = 0, D/C# = 0 (Command)
    data[1] = cmd;

    HAL_I2C_Master_Transmit(&SH1106_I2C, SH1106_I2C_ADDR, data, 2, HAL_MAX_DELAY);
}

/**
 * @brief Gửi dữ liệu đến SH1106
 * @param data Dữ liệu cần gửi
 */
void SH1106_SendData(uint8_t data)
{
    uint8_t buffer[2];
    buffer[0] = 0x40;  // Co = 0, D/C# = 1 (Data)
    buffer[1] = data;

    HAL_I2C_Master_Transmit(&SH1106_I2C, SH1106_I2C_ADDR, buffer, 2, HAL_MAX_DELAY);
}

/**
 * @brief Xóa toàn bộ màn hình
 */
void SH1106_Clear(void)
{
    memset(OLED_Buffer, 0x00, sizeof(OLED_Buffer));

    // Gửi dữ liệu xóa màn hình đến tất cả các trang
    for (uint8_t page = 0; page < 8; page++)
    {
        SH1106_SetCursor(page, 0);

        for (uint8_t col = 0; col < SH1106_WIDTH; col++)
        {
            SH1106_SendData(0x00);
        }
    }
}

/**
 * @brief Cập nhật toàn bộ màn hình từ buffer
 */
void SH1106_UpdateScreen(void)
{
    for (uint8_t page = 0; page < 8; page++)
    {
        SH1106_SetCursor(page, 0);

        for (uint8_t col = 0; col < SH1106_WIDTH; col++)
        {
            SH1106_SendData(OLED_Buffer[page * SH1106_WIDTH + col]);
        }
    }
}

/**
 * @brief Đặt vị trí con trỏ
 * @param page Trang (0-7)
 * @param column Cột (0-127)
 */
void SH1106_SetCursor(uint8_t page, uint8_t column)
{
    SH1106_SendCommand(SH1106_CMD_SET_PAGE | page);
    SH1106_SendCommand(SH1106_CMD_SET_COLUMN_LOW | (column & 0x0F));
    SH1106_SendCommand(SH1106_CMD_SET_COLUMN_HIGH | ((column >> 4) & 0x0F));
}

/**
 * @brief Vẽ một pixel tại vị trí xác định
 * @param x Tọa độ X (0-127)
 * @param y Tọa độ Y (0-63)
 */
void SH1106_DrawPixel(uint8_t x, uint8_t y)
{
    if (x >= SH1106_WIDTH || y >= SH1106_HEIGHT)
        return;

    uint16_t index = (y / 8) * SH1106_WIDTH + x;
    uint8_t bit = y % 8;

    OLED_Buffer[index] |= (1 << bit);
}

/**
 * @brief Hiển thị một ký tự tại vị trí hiện tại
 * @param ch Ký tự cần hiển thị
 * @param font Font chữ sử dụng
 */
void SH1106_WriteChar(char ch, uint8_t *font)
{
    uint8_t charWidth = font[0];  // Chiều rộng ký tự
    uint8_t charHeight = font[1]; // Chiều cao ký tự

    // Lấy dữ liệu bitmap của ký tự từ font
    uint16_t charOffset = (ch - 32) * ((charWidth * charHeight) / 8 + 1);
    uint8_t *charData = &font[2 + charOffset];

    for (uint8_t row = 0; row < charHeight; row++)
    {
        for (uint8_t col = 0; col < charWidth; col++)
        {
            uint8_t byteIndex = col / 8;
            uint8_t bitIndex = col % 8;

            if (charData[row * ((charWidth + 7) / 8) + byteIndex] & (1 << (7 - bitIndex)))
            {
                uint8_t x = col;
                uint8_t y = row;
                SH1106_DrawPixel(x, y);
            }
        }
    }
}

/**
 * @brief Hiển thị một ký tự tại vị trí xác định
 * @param x Tọa độ X
 * @param y Tọa độ Y
 * @param ch Ký tự cần hiển thị
 * @param font Font chữ sử dụng
 */
void SH1106_WriteCharAt(uint8_t x, uint8_t y, char ch, uint8_t *font)
{
    // Lưu vị trí hiện tại
    uint8_t currentPage = y / 8;
    uint8_t currentColumn = x;

    // Đặt con trỏ đến vị trí cần hiển thị
    SH1106_SetCursor(currentPage, currentColumn);

    // Hiển thị ký tự
    SH1106_WriteChar(ch, font);

    // Cập nhật màn hình
    SH1106_UpdateScreen();
}

/**
 * @brief Hiển thị chuỗi tại vị trí hiện tại
 * @param str Chuỗi cần hiển thị
 * @param font Font chữ sử dụng
 */
void SH1106_WriteString(char *str, uint8_t *font)
{
    while (*str)
    {
        SH1106_WriteChar(*str, font);
        str++;
    }
}

/**
 * @brief Hiển thị chuỗi tại vị trí xác định
 * @param x Tọa độ X
 * @param y Tọa độ Y
 * @param str Chuỗi cần hiển thị
 * @param font Font chữ sử dụng
 */
void SH1106_WriteStringAt(uint8_t x, uint8_t y, char *str, uint8_t *font)
{
    uint8_t originalX = x;
    uint8_t currentY = y;

    while (*str)
    {
        if (*str == '\n')
        {
            // Xuống dòng
            currentY += font[1];  // Chiều cao font
            x = originalX;
        }
        else
        {
            SH1106_WriteCharAt(x, currentY, *str, font);
            x += font[0];  // Chiều rộng font
        }
        str++;
    }
}
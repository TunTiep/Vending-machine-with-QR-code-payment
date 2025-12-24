/**
 * Hướng dẫn sử dụng thư viện SH1106 OLED cho STM32 - Keil C
 * 
 * Màn hình: SH1106 OLED 128x64
 * Địa chỉ I2C: 0x3C
 * Vi điều khiển: STM32F10x
 */

#include "main.h"
#include "sh1106.h"

// Khai báo extern cho I2C handle (được tạo bởi STM32CubeMX)
extern I2C_HandleTypeDef hi2c1;

int main(void)
{
    /* Khởi tạo HAL */
    HAL_Init();
    
    /* Cấu hình clock hệ thống */
    SystemClock_Config();
    
    /* Khởi tạo các peripheral */
    MX_GPIO_Init();
    MX_I2C1_Init();  // I2C1 được cấu hình trong STM32CubeMX
    
    /* Khởi tạo màn hình SH1106 */
    if (SH1106_Init()) 
    {
        /* Khởi tạo thành công */
        
        // Hiển thị Hello World đơn giản
        SH1106_HelloWorld();
        
        HAL_Delay(3000);  // Đợi 3 giây
        
        // Ví dụ hiển thị text tùy chỉnh
        SH1106_Clear();
        SH1106_GotoXY(10, 5);
        SH1106_Puts("SH1106 OLED", &Font_11x18, SH1106_COLOR_WHITE);
        
        SH1106_GotoXY(15, 25);
        SH1106_Puts("Dia chi: 0x3C", &Font_7x10, SH1106_COLOR_WHITE);
        
        SH1106_GotoXY(20, 40);
        SH1106_Puts("STM32 + Keil", &Font_7x10, SH1106_COLOR_WHITE);
        
        SH1106_GotoXY(25, 55);
        SH1106_Puts("Test OK!", &Font_7x10, SH1106_COLOR_WHITE);
        
        SH1106_UpdateScreen();
    } 
    else 
    {
        /* Khởi tạo thất bại */
        // Có thể nhấp nháy LED để báo lỗi
        while(1) 
        {
            HAL_Delay(200);
            // Xử lý lỗi ở đây
        }
    }

    /* Vòng lặp chính */
    while (1)
    {
        /* Code ứng dụng chính */
        HAL_Delay(100);
    }
}

/**
 * Hàm hiển thị thông tin hệ thống
 */
void HienThiThongTin(void) 
{
    SH1106_Clear();
    
    SH1106_GotoXY(5, 0);
    SH1106_Puts("== SH1106 ==", &Font_11x18, SH1106_COLOR_WHITE);
    
    SH1106_GotoXY(5, 20);
    SH1106_Puts("Kich thuoc: 128x64", &Font_7x10, SH1106_COLOR_WHITE);
    
    SH1106_GotoXY(5, 35);
    SH1106_Puts("I2C: 0x3C", &Font_7x10, SH1106_COLOR_WHITE);
    
    SH1106_GotoXY(5, 50);
    SH1106_Puts("Vi dieu khien: STM32", &Font_7x10, SH1106_COLOR_WHITE);
    
    SH1106_UpdateScreen();
}

/**
 * Hàm hiển thị số đếm
 */
void HienThiSoDem(uint32_t counter) 
{
    char buffer[20];
    
    SH1106_Clear();
    
    SH1106_GotoXY(20, 10);
    SH1106_Puts("Dem so:", &Font_11x18, SH1106_COLOR_WHITE);
    
    sprintf(buffer, "Count: %lu", counter);
    SH1106_GotoXY(25, 35);
    SH1106_Puts(buffer, &Font_7x10, SH1106_COLOR_WHITE);
    
    SH1106_UpdateScreen();
}

/**
 * Hàm test các chức năng vẽ
 */
void TestVe(void) 
{
    SH1106_Clear();
    
    // Vẽ hình chữ nhật
    SH1106_DrawRectangle(5, 5, 40, 25, SH1106_COLOR_WHITE);
    
    // Vẽ hình chữ nhật đặc
    SH1106_DrawFilledRectangle(50, 5, 30, 15, SH1106_COLOR_WHITE);
    
    // Vẽ đường thẳng
    SH1106_DrawLine(5, 35, 120, 35, SH1106_COLOR_WHITE);
    SH1106_DrawLine(5, 40, 120, 50, SH1106_COLOR_WHITE);
    
    // Hiển thị text
    SH1106_GotoXY(10, 55);
    SH1106_Puts("Test ve hinh", &Font_7x10, SH1106_COLOR_WHITE);
    
    SH1106_UpdateScreen();
}

/*
 * HƯỚNG DẪN SỬ DỤNG:
 * 
 * 1. Cấu hình STM32CubeMX:
 *    - Bật I2C1
 *    - SDA: PB7, SCL: PB6
 *    - Tốc độ: 100kHz (Standard) hoặc 400kHz (Fast)
 *    - Địa chỉ: 0x3C (đã được thiết lập trong sh1106.h)
 * 
 * 2. Thêm files vào project Keil:
 *    - sh1106.c
 *    - sh1106.h
 *    - fonts.c (từ GitHub repository)
 *    - fonts.h (từ GitHub repository)
 * 
 * 3. Kết nối phần cứng:
 *    VCC  -> 3.3V
 *    GND  -> GND  
 *    SCL  -> PB6
 *    SDA  -> PB7
 * 
 * 4. Sử dụng trong code:
 *    - Gọi SH1106_Init() để khởi tạo
 *    - Sử dụng SH1106_Puts() để hiển thị text
 *    - Gọi SH1106_UpdateScreen() để cập nhật màn hình
 *    - Sử dụng SH1106_Clear() để xóa màn hình
 * 
 * 5. Font có sẵn:
 *    - Font_7x10: Font nhỏ (7x10 pixels)
 *    - Font_11x18: Font trung bình (11x18 pixels) 
 *    - Font_16x26: Font lớn (16x26 pixels)
 */
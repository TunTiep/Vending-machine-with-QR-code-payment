# Thư viện SH1106 OLED cho STM32 - Keil C

## Tổng quan
Thư viện này được tạo dựa trên thư viện SSD1306 có sẵn và được điều chỉnh để hoạt động với màn hình OLED SH1106.

## Thông số kỹ thuật
- **Màn hình**: SH1106 OLED 128x64 pixels
- **Giao tiếp**: I2C
- **Địa chỉ I2C**: 0x3C (đã được cấu hình sẵn)
- **Vi điều khiển**: STM32F10x series
- **IDE**: Keil uVision

## Files trong thư viện

### Files chính:
- `sh1106.h` - File header với khai báo các hàm
- `sh1106.c` - File source chứa implementation
- `fonts.h` - File header cho fonts (từ GitHub repository gốc)
- `fonts.c` - File chứa dữ liệu fonts (từ GitHub repository gốc)

### Files ví dụ:
- `sh1106_example.c` - Ví dụ sử dụng cơ bản bằng tiếng Anh
- `huong_dan_sh1106.c` - Hướng dẫn và ví dụ bằng tiếng Việt

## Cấu hình phần cứng

### Kết nối dây:
```
SH1106 OLED    STM32F10x    Mô tả
VCC            3.3V         Nguồn cấp
GND            GND          Đất
SCL            PB6          Clock I2C
SDA            PB7          Data I2C
```

### Cấu hình STM32CubeMX:
1. Bật I2C1 peripheral
2. Cấu hình pins:
   - PB6: I2C1_SCL
   - PB7: I2C1_SDA
3. Tốc độ I2C: 100kHz (Standard Mode) hoặc 400kHz (Fast Mode)
4. Generate code

## Cài đặt trong Keil

### Bước 1: Thêm files vào project
1. Copy các files vào thư mục project:
   - `sh1106.h`
   - `sh1106.c`
   - `fonts.h` 
   - `fonts.c`

2. Trong Keil uVision:
   - Right-click vào Source Group
   - Add Existing Files to Group
   - Chọn `sh1106.c` và `fonts.c`

### Bước 2: Include path
- Đảm bảo thư mục chứa `sh1106.h` và `fonts.h` có trong include path

## Sử dụng thư viện

### Khởi tạo cơ bản:
```c
#include "sh1106.h"

int main(void)
{
    // Khởi tạo HAL và I2C
    HAL_Init();
    SystemClock_Config();
    MX_I2C1_Init();
    
    // Khởi tạo màn hình SH1106
    if (SH1106_Init()) {
        // Khởi tạo thành công
        SH1106_HelloWorld();  // Hiển thị Hello World
    }
    
    while(1) {
        // Main loop
    }
}
```

### Hiển thị text:
```c
// Xóa màn hình
SH1106_Clear();

// Đặt vị trí cursor
SH1106_GotoXY(10, 10);

// Hiển thị text với font 11x18
SH1106_Puts("Hello Vietnam!", &Font_11x18, SH1106_COLOR_WHITE);

// Cập nhật màn hình
SH1106_UpdateScreen();
```

### Vẽ hình:
```c
// Vẽ hình chữ nhật
SH1106_DrawRectangle(10, 10, 50, 30, SH1106_COLOR_WHITE);

// Vẽ hình chữ nhật đặc
SH1106_DrawFilledRectangle(70, 10, 30, 20, SH1106_COLOR_WHITE);

// Vẽ đường thẳng
SH1106_DrawLine(0, 0, 127, 63, SH1106_COLOR_WHITE);

// Cập nhật màn hình
SH1106_UpdateScreen();
```

## Các hàm chính

### Hàm khởi tạo và cơ bản:
- `SH1106_Init()` - Khởi tạo màn hình
- `SH1106_Clear()` - Xóa màn hình
- `SH1106_UpdateScreen()` - Cập nhật màn hình
- `SH1106_Fill(color)` - Tô đầy màn hình

### Hàm hiển thị text:
- `SH1106_GotoXY(x, y)` - Đặt vị trí cursor
- `SH1106_Putc(char, font, color)` - Hiển thị 1 ký tự
- `SH1106_Puts(string, font, color)` - Hiển thị chuỗi
- `SH1106_HelloWorld()` - Hiển thị Hello World mẫu

### Hàm vẽ hình:
- `SH1106_DrawPixel(x, y, color)` - Vẽ 1 pixel
- `SH1106_DrawLine(x0, y0, x1, y1, color)` - Vẽ đường thẳng
- `SH1106_DrawRectangle(x, y, w, h, color)` - Vẽ hình chữ nhật
- `SH1106_DrawFilledRectangle(x, y, w, h, color)` - Vẽ hình chữ nhật đặc

## Fonts có sẵn
- `Font_7x10` - Font nhỏ (7x10 pixels)
- `Font_11x18` - Font trung bình (11x18 pixels)  
- `Font_16x26` - Font lớn (16x26 pixels)

## Màu sắc
- `SH1106_COLOR_BLACK` - Màu đen (tắt pixel)
- `SH1106_COLOR_WHITE` - Màu trắng (bật pixel)

## Lưu ý quan trọng

1. **Luôn gọi `SH1106_UpdateScreen()`** sau khi vẽ để cập nhật màn hình
2. **Địa chỉ I2C** đã được thiết lập là 0x3C
3. **I2C Handle** phải là `hi2c1` (được tạo bởi STM32CubeMX)
4. **Tọa độ** bắt đầu từ (0,0) ở góc trên bên trái

## Troubleshooting

### Màn hình không hiển thị gì:
- Kiểm tra kết nối dây
- Kiểm tra địa chỉ I2C (0x3C)
- Kiểm tra cấu hình I2C trong STM32CubeMX
- Đảm bảo gọi `SH1106_UpdateScreen()`

### Lỗi compilation:
- Kiểm tra đã add đủ files vào project
- Kiểm tra include path
- Đảm bảo STM32 HAL được include

### Màn hình hiển thị sai:
- Thử thay đổi tốc độ I2C
- Kiểm tra nguồn cấp 3.3V ổn định
- Thử các địa chỉ I2C khác (0x78)

## Ví dụ hoàn chỉnh

Xem file `huong_dan_sh1106.c` để có ví dụ chi tiết bằng tiếng Việt.
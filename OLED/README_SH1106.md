# Thư viện SH1106 OLED Display cho STM32

Đây là thư viện điều khiển màn hình OLED SH1106 dành cho STM32 sử dụng Keil C và HAL library.

## Các tính năng

- ✅ Khởi tạo màn hình SH1106
- ✅ Hiển thị text với font tùy chỉnh
- ✅ Hiển thị ký tự tại vị trí xác định
- ✅ Xóa màn hình
- ✅ Điều khiển độ sáng và contrast
- ✅ Tương thích với nhiều board STM32

## Cấu trúc thư viện

```
/workspace/OLED/
├── sh1106.h          # Khai báo các hàm và macro
├── sh1106.c          # Triển khai các hàm chính
├── fonts.h           # Khai báo font chữ
├── fonts.c           # Dữ liệu font chữ
└── main_example.c    # Ví dụ sử dụng hiển thị "Hello World"
```

## Cách sử dụng

### 1. Cấu hình Hardware

Đảm bảo kết nối I2C giữa STM32 và SH1106:

```
STM32        SH1106
SCL (PB6) --- SCL
SDA (PB7) --- SDA
GND ------- GND
VCC ------- 3.3V hoặc 5V
```

### 2. Thêm thư viện vào dự án Keil C

1. Copy các file sau vào dự án của bạn:
   - `sh1106.h`
   - `sh1106.c`
   - `fonts.h`
   - `fonts.c`

2. Include các file header cần thiết trong `main.c`:
```c
#include "stm32f1xx_hal.h"
#include "sh1106.h"
#include "fonts.h"
```

### 3. Cấu hình I2C

Đảm bảo đã cấu hình I2C trong STM32CubeMX hoặc code:
```c
// Trong MX_I2C1_Init()
hi2c1.Instance = I2C1;
hi2c1.Init.ClockSpeed = 400000;  // 400kHz
hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
hi2c1.Init.OwnAddress1 = 0;
hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
hi2c1.Init.OwnAddress2 = 0;
hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
```

### 4. Ví dụ sử dụng cơ bản

```c
#include "stm32f1xx_hal.h"
#include "sh1106.h"
#include "fonts.h"

int main(void)
{
    // Khởi tạo HAL và peripherals
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();

    // Khởi tạo màn hình SH1106
    SH1106_Init();

    // Xóa màn hình
    SH1106_Clear();

    // Hiển thị "Hello World"
    SH1106_WriteStringAt(20, 20, "Hello", DEFAULT_FONT);
    SH1106_WriteStringAt(20, 35, "World!", DEFAULT_FONT);

    // Cập nhật màn hình
    SH1106_UpdateScreen();

    while (1) {
        // Chương trình chính của bạn
    }
}
```

## Các hàm chính

### Khởi tạo và cấu hình cơ bản

```c
void SH1106_Init(void);                    // Khởi tạo màn hình
void SH1106_Clear(void);                   // Xóa màn hình
void SH1106_UpdateScreen(void);            // Cập nhật toàn bộ màn hình
```

### Hiển thị text

```c
void SH1106_WriteCharAt(uint8_t x, uint8_t y, char ch, uint8_t *font);
                                           // Hiển thị ký tự tại vị trí (x,y)

void SH1106_WriteStringAt(uint8_t x, uint8_t y, char *str, uint8_t *font);
                                           // Hiển thị chuỗi tại vị trí (x,y)

void SH1106_WriteChar(char ch, uint8_t *font);
                                           // Hiển thị ký tự tại vị trí hiện tại

void SH1106_WriteString(char *str, uint8_t *font);
                                           // Hiển thị chuỗi tại vị trí hiện tại
```

### Điều khiển vị trí

```c
void SH1106_SetCursor(uint8_t page, uint8_t column);
                                           // Đặt vị trí con trỏ
```

## Các định nghĩa hữu ích

```c
// Kích thước màn hình
#define SH1106_WIDTH    128
#define SH1106_HEIGHT   64

// Địa chỉ I2C
#define SH1106_I2C_ADDR 0x78

// Font mặc định
#define DEFAULT_FONT Font_5x7
```

## Lưu ý quan trọng

1. **I2C Handle**: Đảm bảo thay đổi `SH1106_I2C` trong `sh1106.h` thành I2C handle thực tế của bạn (ví dụ: `hi2c1`, `hi2c2`, etc.)

2. **Font chữ**: Thư viện hỗ trợ font 5x7 và 8x16. Bạn có thể thêm font tùy chỉnh bằng cách mở rộng mảng `Font_5x7` hoặc `Font_8x16` trong `fonts.c`.

3. **Tối ưu hóa**: Với màn hình lớn, việc cập nhật toàn bộ màn hình (`SH1106_UpdateScreen()`) có thể chậm. Bạn có thể tối ưu hóa bằng cách chỉ cập nhật các vùng cần thiết.

4. **Độ tương phản**: Bạn có thể điều chỉnh độ tương phản bằng cách thay đổi giá trị trong hàm `SH1106_Init()`:
```c
SH1106_SendCommand(SH1106_CMD_SET_CONTRAST);
SH1106_SendCommand(0x7F);  // Điều chỉnh từ 0x00 đến 0xFF
```

## Troubleshooting

### Màn hình không hiển thị gì
- Kiểm tra kết nối I2C (SCL, SDA, GND, VCC)
- Đảm bảo địa chỉ I2C đúng (thường là 0x78 hoặc 0x7A)
- Kiểm tra cấu hình I2C trong STM32CubeMX

### Text hiển thị không đúng
- Kiểm tra font chữ được sử dụng
- Đảm bảo tọa độ (x,y) nằm trong phạm vi màn hình (0-127, 0-63)

### I2C bị lỗi
- Kiểm tra tốc độ I2C (không nên vượt quá 400kHz)
- Đảm bảo pull-up resistors trên đường I2C

## Ví dụ nâng cao

```c
// Hiển thị nhiều dòng text
SH1106_WriteStringAt(0, 0, "Line 1", DEFAULT_FONT);
SH1106_WriteStringAt(0, 15, "Line 2", DEFAULT_FONT);
SH1106_WriteStringAt(0, 30, "Line 3", DEFAULT_FONT);
SH1106_WriteStringAt(0, 45, "Line 4", DEFAULT_FONT);
SH1106_UpdateScreen();

// Vẽ các ký tự riêng lẻ
for (int i = 0; i < 10; i++) {
    SH1106_WriteCharAt(i*10, 50, '0' + i, DEFAULT_FONT);
}
SH1106_UpdateScreen();
```

## Liên hệ và hỗ trợ

Nếu bạn gặp vấn đề khi sử dụng thư viện này, hãy kiểm tra:
1. Cấu hình hardware
2. Cấu hình I2C
3. Thứ tự khởi tạo các peripheral

Thư viện này được thiết kế để hoạt động với hầu hết các board STM32 có I2C và tương thích với Keil C.
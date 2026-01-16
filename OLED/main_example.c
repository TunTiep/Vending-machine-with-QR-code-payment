#include "stm32f1xx_hal.h"  // Thay đổi theo board STM32 của bạn
#include "sh1106.h"
#include "fonts.h"

// Khai báo các hàm cần thiết từ STM32 HAL
extern I2C_HandleTypeDef hi2c1;  // Thay đổi theo I2C peripheral bạn sử dụng

// Khai báo các hàm SystemClock và MX_GPIO_Init nếu cần
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/**
 * @brief Hàm main chính
 */
int main(void)
{
    // Khởi tạo HAL
    HAL_Init();

    // Cấu hình System Clock
    SystemClock_Config();

    // Khởi tạo GPIO
    MX_GPIO_Init();

    // Khởi tạo màn hình SH1106
    SH1106_Init();

    // Xóa màn hình
    SH1106_Clear();

    // Hiển thị "Hello World" ở giữa màn hình
    SH1106_WriteStringAt(20, 20, "Hello", DEFAULT_FONT);
    SH1106_WriteStringAt(20, 35, "World!", DEFAULT_FONT);

    // Hoặc hiển thị từng ký tự riêng lẻ
    // SH1106_WriteCharAt(20, 20, 'H', DEFAULT_FONT);
    // SH1106_WriteCharAt(30, 20, 'e', DEFAULT_FONT);
    // SH1106_WriteCharAt(40, 20, 'l', DEFAULT_FONT);
    // SH1106_WriteCharAt(50, 20, 'l', DEFAULT_FONT);
    // SH1106_WriteCharAt(60, 20, 'o', DEFAULT_FONT);
    //
    // SH1106_WriteCharAt(20, 35, 'W', DEFAULT_FONT);
    // SH1106_WriteCharAt(30, 35, 'o', DEFAULT_FONT);
    // SH1106_WriteCharAt(40, 35, 'r', DEFAULT_FONT);
    // SH1106_WriteCharAt(50, 35, 'l', DEFAULT_FONT);
    // SH1106_WriteCharAt(60, 35, 'd', DEFAULT_FONT);
    // SH1106_WriteCharAt(70, 35, '!', DEFAULT_FONT);

    // Cập nhật màn hình để hiển thị nội dung
    SH1106_UpdateScreen();

    while (1)
    {
        // Vòng lặp chính - có thể thêm các chức năng khác ở đây
        HAL_Delay(1000);  // Đợi 1 giây
    }
}

/**
 * @brief Cấu hình System Clock
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief Khởi tạo GPIO
 */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOB_CLK_ENABLE();  // Thay đổi theo chân I2C của bạn

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);  // PB6, PB7 cho I2C1

    /*Configure GPIO pins : PB6 PB7 */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;  // Open drain cho I2C
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
 * @brief Hàm xử lý lỗi
 */
void Error_Handler(void)
{
    /* Trong trường hợp lỗi thực tế, bạn có thể thêm code để xử lý lỗi ở đây */
    while(1)
    {
        // Vòng lặp vô tận trong trường hợp lỗi
    }
}
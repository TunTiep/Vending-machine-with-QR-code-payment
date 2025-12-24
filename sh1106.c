/**
 * SH1106 OLED Display Library for STM32
 * Based on SSD1306 library by Tilen Majerle and Alexander Lutsai
 * Modified for SH1106 controller compatibility
 */

#include "sh1106.h"

extern I2C_HandleTypeDef hi2c1;

/* Write command */
#define SH1106_WRITECOMMAND(command)      sh1106_I2C_Write(SH1106_I2C_ADDR, 0x00, (command))
/* Write data */
#define SH1106_WRITEDATA(data)            sh1106_I2C_Write(SH1106_I2C_ADDR, 0x40, (data))
/* Absolute value */
#define ABS(x)   ((x) > 0 ? (x) : -(x))

/* SH1106 data buffer */
static uint8_t SH1106_Buffer[SH1106_WIDTH * SH1106_HEIGHT / 8];

/* Private SH1106 structure */
typedef struct {
    uint16_t CurrentX;
    uint16_t CurrentY;
    uint8_t Inverted;
    uint8_t Initialized;
} SH1106_t;

/* Private variable */
static SH1106_t SH1106;

/* SH1106 commands */
#define SH1106_NORMALDISPLAY       0xA6
#define SH1106_INVERTDISPLAY       0xA7
#define SH1106_DISPLAYOFF          0xAE
#define SH1106_DISPLAYON           0xAF
#define SH1106_SETCONTRAST         0x81
#define SH1106_DISPLAYALLON_RESUME 0xA4
#define SH1106_DISPLAYALLON        0xA5
#define SH1106_MEMORYMODE          0x20
#define SH1106_COLUMNADDR          0x21
#define SH1106_PAGEADDR            0x22
#define SH1106_COMSCANDEC          0xC8
#define SH1106_COMSCANINC          0xC0
#define SH1106_SEGREMAP            0xA0
#define SH1106_CHARGEPUMP          0x8D
#define SH1106_EXTERNALVCC         0x1
#define SH1106_SWITCHCAPVCC        0x2

uint8_t SH1106_Init(void) {
    /* Init I2C */
    sh1106_I2C_Init();
    
    /* Init LCD */
    SH1106_WRITECOMMAND(SH1106_DISPLAYOFF);                    // 0xAE
    SH1106_WRITECOMMAND(0x02);                                 // Set lower column address
    SH1106_WRITECOMMAND(0x10);                                 // Set higher column address
    SH1106_WRITECOMMAND(0x40);                                 // Set display start line
    SH1106_WRITECOMMAND(SH1106_SETCONTRAST);                   // 0x81
    SH1106_WRITECOMMAND(0x80);                                 // Set contrast value
    SH1106_WRITECOMMAND(SH1106_SEGREMAP | 0x1);               // Set segment re-map
    SH1106_WRITECOMMAND(SH1106_NORMALDISPLAY);                 // 0xA6
    SH1106_WRITECOMMAND(0xA8);                                 // Set multiplex ratio
    SH1106_WRITECOMMAND(0x3F);                                 // 1/64 duty
    SH1106_WRITECOMMAND(SH1106_DISPLAYALLON_RESUME);           // 0xA4
    SH1106_WRITECOMMAND(0xD3);                                 // Set display offset
    SH1106_WRITECOMMAND(0x00);                                 // No offset
    SH1106_WRITECOMMAND(0xD5);                                 // Set display clock divide ratio/oscillator frequency
    SH1106_WRITECOMMAND(0x80);                                 // Set divide ratio
    SH1106_WRITECOMMAND(0xD9);                                 // Set pre-charge period
    SH1106_WRITECOMMAND(0x22);                                 // Pre-charge period
    SH1106_WRITECOMMAND(0xDA);                                 // Set com pins hardware configuration
    SH1106_WRITECOMMAND(0x12);                                 // Com pins configuration
    SH1106_WRITECOMMAND(0xDB);                                 // Set vcomh
    SH1106_WRITECOMMAND(0x20);                                 // Vcomh deselect level
    SH1106_WRITECOMMAND(SH1106_CHARGEPUMP);                    // 0x8D
    SH1106_WRITECOMMAND(0x14);                                 // Enable charge pump
    SH1106_WRITECOMMAND(SH1106_COMSCANDEC);                    // Set com output scan direction
    SH1106_WRITECOMMAND(SH1106_DISPLAYON);                     // 0xAF
    
    /* Clear screen */
    SH1106_Fill(SH1106_COLOR_BLACK);
    
    /* Update screen */
    SH1106_UpdateScreen();
    
    /* Set default values */
    SH1106.CurrentX = 0;
    SH1106.CurrentY = 0;
    
    /* Initialized OK */
    SH1106.Initialized = 1;
    
    /* Return OK */
    return 1;
}

void SH1106_UpdateScreen(void) {
    uint8_t page;
    
    for (page = 0; page < 8; page++) {
        SH1106_WRITECOMMAND(0xB0 + page);                      // Set page address
        SH1106_WRITECOMMAND(0x02);                             // Set lower column address
        SH1106_WRITECOMMAND(0x10);                             // Set higher column address
        
        /* Write data */
        sh1106_I2C_WriteMulti(SH1106_I2C_ADDR, 0x40, &SH1106_Buffer[SH1106_WIDTH * page], SH1106_WIDTH);
    }
}

void SH1106_ToggleInvert(void) {
    uint16_t i;
    
    /* Toggle invert */
    SH1106.Inverted = !SH1106.Inverted;
    
    /* Do memory toggle */
    for (i = 0; i < sizeof(SH1106_Buffer); i++) {
        SH1106_Buffer[i] = ~SH1106_Buffer[i];
    }
}

void SH1106_Fill(SH1106_COLOR_t Color) {
    /* Set memory */
    memset(SH1106_Buffer, (Color == SH1106_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(SH1106_Buffer));
}

void SH1106_DrawPixel(uint16_t x, uint16_t y, SH1106_COLOR_t color) {
    if (
        x >= SH1106_WIDTH ||
        y >= SH1106_HEIGHT
    ) {
        /* Error */
        return;
    }
    
    /* Check if pixels are inverted */
    if (SH1106.Inverted) {
        color = (SH1106_COLOR_t)!color;
    }
    
    /* Set pixel */
    if (color == SH1106_COLOR_WHITE) {
        SH1106_Buffer[x + (y / 8) * SH1106_WIDTH] |= 1 << (y % 8);
    } else {
        SH1106_Buffer[x + (y / 8) * SH1106_WIDTH] &= ~(1 << (y % 8));
    }
}

void SH1106_GotoXY(uint16_t x, uint16_t y) {
    /* Set write pointers */
    SH1106.CurrentX = x;
    SH1106.CurrentY = y;
}

char SH1106_Putc(char ch, FontDef_t* Font, SH1106_COLOR_t color) {
    uint32_t i, b, j;
    
    /* Check available space in LCD */
    if (
        SH1106_WIDTH <= (SH1106.CurrentX + Font->FontWidth) ||
        SH1106_HEIGHT <= (SH1106.CurrentY + Font->FontHeight)
    ) {
        /* Error */
        return 0;
    }
    
    /* Use the font to write */
    for (i = 0; i < Font->FontHeight; i++) {
        b = Font->data[(ch - 32) * Font->FontHeight + i];
        for (j = 0; j < Font->FontWidth; j++) {
            if ((b << j) & 0x8000) {
                SH1106_DrawPixel(SH1106.CurrentX + j, (SH1106.CurrentY + i), (SH1106_COLOR_t) color);
            } else {
                SH1106_DrawPixel(SH1106.CurrentX + j, (SH1106.CurrentY + i), (SH1106_COLOR_t)!color);
            }
        }
    }
    
    /* Increase pointer */
    SH1106.CurrentX += Font->FontWidth;
    
    /* Return character written */
    return ch;
}

char SH1106_Puts(char* str, FontDef_t* Font, SH1106_COLOR_t color) {
    /* Write characters */
    while (*str) {
        /* Write character by character */
        if (SH1106_Putc(*str, Font, color) != *str) {
            /* Return error */
            return *str;
        }
        
        /* Increase string pointer */
        str++;
    }
    
    /* Everything OK, zero should be returned */
    return *str;
}

void SH1106_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SH1106_COLOR_t c) {
    int16_t dx, dy, sx, sy, err, e2, i, tmp; 
    
    /* Check for overflow */
    if (x0 >= SH1106_WIDTH) {
        x0 = SH1106_WIDTH - 1;
    }
    if (x1 >= SH1106_WIDTH) {
        x1 = SH1106_WIDTH - 1;
    }
    if (y0 >= SH1106_HEIGHT) {
        y0 = SH1106_HEIGHT - 1;
    }
    if (y1 >= SH1106_HEIGHT) {
        y1 = SH1106_HEIGHT - 1;
    }
    
    dx = (x0 < x1) ? (x1 - x0) : (x0 - x1);
    dy = (y0 < y1) ? (y1 - y0) : (y0 - y1); 
    sx = (x0 < x1) ? 1 : -1; 
    sy = (y0 < y1) ? 1 : -1; 
    err = ((dx > dy) ? dx : -dy) / 2;

    if (dx == 0) {
        if (y1 < y0) {
            tmp = y1;
            y1 = y0;
            y0 = tmp;
        }
        
        if (x1 < x0) {
            tmp = x1;
            x1 = x0;
            x0 = tmp;
        }
        
        /* Vertical line */
        for (i = y0; i <= y1; i++) {
            SH1106_DrawPixel(x0, i, c);
        }
        
        /* Return from function */
        return;
    }
    
    if (dy == 0) {
        if (y1 < y0) {
            tmp = y1;
            y1 = y0;
            y0 = tmp;
        }
        
        if (x1 < x0) {
            tmp = x1;
            x1 = x0;
            x0 = tmp;
        }
        
        /* Horizontal line */
        for (i = x0; i <= x1; i++) {
            SH1106_DrawPixel(i, y0, c);
        }
        
        /* Return from function */
        return;
    }
    
    while (1) {
        SH1106_DrawPixel(x0, y0, c);
        if (x0 == x1 && y0 == y1) {
            break;
        }
        e2 = err; 
        if (e2 > -dx) {
            err -= dy;
            x0 += sx;
        } 
        if (e2 < dy) {
            err += dx;
            y0 += sy;
        } 
    }
}

void SH1106_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SH1106_COLOR_t c) {
    /* Check input parameters */
    if (
        x >= SH1106_WIDTH ||
        y >= SH1106_HEIGHT
    ) {
        /* Return error */
        return;
    }
    
    /* Check width and height */
    if ((x + w) >= SH1106_WIDTH) {
        w = SH1106_WIDTH - x;
    }
    if ((y + h) >= SH1106_HEIGHT) {
        h = SH1106_HEIGHT - y;
    }
    
    /* Draw 4 lines */
    SH1106_DrawLine(x, y, x + w, y, c);         /* Top line */
    SH1106_DrawLine(x, y + h, x + w, y + h, c); /* Bottom line */
    SH1106_DrawLine(x, y, x, y + h, c);         /* Left line */
    SH1106_DrawLine(x + w, y, x + w, y + h, c); /* Right line */
}

void SH1106_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SH1106_COLOR_t c) {
    uint8_t i;
    
    /* Check input parameters */
    if (
        x >= SH1106_WIDTH ||
        y >= SH1106_HEIGHT
    ) {
        /* Return error */
        return;
    }
    
    /* Check width and height */
    if ((x + w) >= SH1106_WIDTH) {
        w = SH1106_WIDTH - x;
    }
    if ((y + h) >= SH1106_HEIGHT) {
        h = SH1106_HEIGHT - y;
    }
    
    /* Draw lines */
    for (i = 0; i <= h; i++) {
        /* Draw lines */
        SH1106_DrawLine(x, y + i, x + w, y + i, c);
    }
}

void SH1106_Clear(void) {
    SH1106_Fill(SH1106_COLOR_BLACK);
    SH1106_UpdateScreen();
}

void SH1106_InvertDisplay(int i) {
    if (i) {
        SH1106_WRITECOMMAND(SH1106_INVERTDISPLAY);
    } else {
        SH1106_WRITECOMMAND(SH1106_NORMALDISPLAY);
    }
}

/* I2C Implementation */
void sh1106_I2C_Init(void) {
    /* I2C initialization is typically handled in STM32CubeMX generated code */
    /* The hi2c1 handle should be initialized in main.c */
}

void sh1106_I2C_Write(uint8_t address, uint8_t reg, uint8_t data) {
    uint8_t dt[2];
    dt[0] = reg;
    dt[1] = data;
    HAL_I2C_Master_Transmit(&hi2c1, address, dt, 2, sh1106_I2C_TIMEOUT);
}

void sh1106_I2C_WriteMulti(uint8_t address, uint8_t reg, uint8_t *data, uint16_t count) {
    uint8_t dt[129];
    dt[0] = reg;
    uint8_t i;
    for(i = 0; i < count; i++)
        dt[i+1] = data[i];
    HAL_I2C_Master_Transmit(&hi2c1, address, dt, count+1, sh1106_I2C_TIMEOUT);
}

void SH1106_HelloWorld(void) {
    /* Clear the screen */
    SH1106_Clear();
    
    /* Set cursor position */
    SH1106_GotoXY(10, 10);
    
    /* Write "Hello World" */
    SH1106_Puts("Hello World", &Font_11x18, SH1106_COLOR_WHITE);
    
    /* Set cursor for second line */
    SH1106_GotoXY(10, 30);
    
    /* Write additional text */
    SH1106_Puts("SH1106 OLED", &Font_7x10, SH1106_COLOR_WHITE);
    
    /* Set cursor for third line */
    SH1106_GotoXY(10, 45);
    
    /* Write STM32 text */
    SH1106_Puts("STM32 Ready!", &Font_7x10, SH1106_COLOR_WHITE);
    
    /* Update screen to display the text */
    SH1106_UpdateScreen();
}
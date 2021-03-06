#ifndef __LCD12864_H
#define __LCD12864_H
#include "main.h"


#define LCD12864_RES_PIN GPIO_PIN_6
#define LCD12864_RES_PORT GPIOC
#define LCD12864_A0_PIN GPIO_PIN_7
#define LCD12864_A0_PORT GPIOC
#define LCD12864_SCK_PIN GPIO_PIN_8
#define LCD12864_SCK_PORT GPIOC
#define LCD12864_SID_PIN GPIO_PIN_9
#define LCD12864_SID_PORT GPIOC
#define LCD12864_CS_PIN GPIO_PIN_15
#define LCD12864_CS_PORT GPIOB

#define LCD12864_CS_HIGH HAL_GPIO_WritePin(LCD12864_CS_PORT, LCD12864_CS_PIN, GPIO_PIN_SET)
#define LCD12864_CS_LOW HAL_GPIO_WritePin(LCD12864_CS_PORT, LCD12864_CS_PIN, GPIO_PIN_RESET)
#define LCD12864_SCK_HIGH HAL_GPIO_WritePin(LCD12864_SCK_PORT, LCD12864_SCK_PIN, GPIO_PIN_SET)
#define LCD12864_SCK_LOW HAL_GPIO_WritePin(LCD12864_SCK_PORT, LCD12864_SCK_PIN, GPIO_PIN_RESET)
#define LCD12864_SID_HIGH HAL_GPIO_WritePin(LCD12864_SID_PORT, LCD12864_SID_PIN, GPIO_PIN_SET)
#define LCD12864_SID_LOW HAL_GPIO_WritePin(LCD12864_SID_PORT, LCD12864_SID_PIN, GPIO_PIN_RESET)
#define LCD12864_RES_HIGH HAL_GPIO_WritePin(LCD12864_RES_PORT, LCD12864_RES_PIN, GPIO_PIN_SET)
#define LCD12864_RES_LOW HAL_GPIO_WritePin(LCD12864_RES_PORT, LCD12864_RES_PIN, GPIO_PIN_RESET)
#define LCD12864_A0_HIGH HAL_GPIO_WritePin(LCD12864_A0_PORT, LCD12864_A0_PIN, GPIO_PIN_SET)
#define LCD12864_A0_LOW HAL_GPIO_WritePin(LCD12864_A0_PORT, LCD12864_A0_PIN, GPIO_PIN_RESET)

#define WRITE_CMD	0xF8//д????  
#define WRITE_DAT	0xFA//д????

void lcd12864_gpio_init(void);
void Lcd_Init(void);
void LCD_Display_Words(uint8_t x,uint8_t y,uint8_t *str);
void LCD_Clear(void);

#endif

#ifndef __LCD12864_H
#define __LCD12864_H
#include "stm32f1xx.h"

#define RS_HIGH HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET)
#define RS_LOW HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET)

#define RW_HIGH HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET)
#define RW_LOW HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET)

#define CLK_HIGH HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_SET)
#define CLK_LOW HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET)
void send_byte(uint8_t byte);
void send_command(uint8_t command);
void send_data(uint8_t disdata);
void lcd12864_init(void);
void lcd12864_display(uint8_t y,uint8_t x,uint8_t *P);

#endif

#ifndef __Delay_
#define __Delay_
#include "stm32f1xx.h"

void delay_init(void);
void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);

#endif

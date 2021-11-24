#ifndef _HEADFILE_H
#define _HEADFILE_H

#include "stm32f10x.h"
#include "./usart/bsp_usart.h"	
#include "./lcd/bsp_ili9341_lcd.h"
#include "./flash/bsp_spi_flash.h"
#include "bsp_adc.h"
#include "bsp_dac.h"
#include "math.h"
#include "stdlib.h"
#include "table_fft.h"
#include "stm32_dsp.h"
#include "./Key/bsp_key.h"
void measure(void);
int Trigger(void);
void Printf_Charater(void);
void XY_T(void);
void Disp(void);
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);
void XY_F(void);
void fft(void);
#define DIRECT 5


#endif


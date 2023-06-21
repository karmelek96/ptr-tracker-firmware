#ifndef _TRACKERHW_H
#define _TRACKERHW_H

#define CPU_FREQUENCY 16000000UL

#include<stdint.h>
#include<stdbool.h>
#include "stm32g0xx.h"

void SysTick_Handler(void);
void HW_DelayMs(uint32_t _ms);
void HW_trackerHwInit(void);
void HW_writeLED(bool _value);
void HW_write_LORACS(bool _value);
void HW_send_UART(char* _txbuffer, uint8_t _size);
void HW_SPI_WriteBuffer(uint8_t *data, uint32_t size);
void HW_SPI_ReadBuffer(uint8_t *data, uint32_t size);
int16_t HW_readADC(uint8_t _ch);
void HW_StartTimer3();
void HW_StopTimer3();
uint32_t HW_getTimer3();
void HW_resetTimer3();
uint8_t HW_readDIO1();
uint8_t HW_readJMP();

#endif

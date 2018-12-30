#ifndef _SX127X_H_
#define _SX127X_H_

#include <stdint.h>
#include <stdbool.h>

int SX127X_Write(uint8_t ch, uint8_t addr, uint8_t* buf, uint8_t size);
int SX127X_Read(uint8_t ch, uint8_t addr, uint8_t* buf, uint8_t size);

void SX127X_Init(void);
void SX127X_Reset(uint8_t ch);
void SX127X_EnableInt(uint8_t ch);
void SX127X_DioIntHandle(uint8_t pin);

uint8_t SX127X_GetIntDio(uint8_t ch);

#endif

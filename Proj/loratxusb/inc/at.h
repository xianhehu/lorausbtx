#ifndef __AT_H__
#define __AT_H__

#include <stdint.h>
#include <stdbool.h>

void AT_Init(void);
void AT_Run(void);
void USB_To_AT_Data(uint8_t* data_buffer, uint8_t Nb_bytes);

#endif
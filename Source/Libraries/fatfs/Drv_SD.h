#ifndef __DRV_SD_H__
#define __DRV_SD_H__

#include "hal_typedef.h"
#include "bsp_spi.h"


void     SD_Init         (void);
bool     SD_Exist        (void);
uint8_t  SD_ReadBlock    (uint32_t block, uint8_t* buf);
uint8_t  SD_ReadSectors  (uint8_t* buf, uint32_t sector, uint32_t count);
uint8_t  SD_WriteSectors (uint8_t* buf, uint32_t sector, uint32_t count);
uint64_t SD_ReadCapacity (void);
#endif
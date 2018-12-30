#ifndef _VERIFY_H_
#define _VERIFY_H_

#include "types.h"

int VERIFY_ReadId(uint8_t *data, uint8_t size);
int VERIFY_SetPasswd(char* pass, uint8_t len);

extern inline int VERIFY_Check(void);

#endif
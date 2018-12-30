#ifndef __RINGBUF_H__
#define __RINGBUF_H__

#include <stdint.h>

typedef struct {
    uint32_t start;
    uint32_t end;
    uint32_t size;
    uint8_t  *buf;
} ringbuf_t;

void RINGBUF_Init   (ringbuf_t *rb, uint8_t *buf, uint32_t size);
int  RINGBUF_GetLen (ringbuf_t *rb);
int  RINGBUF_Put    (ringbuf_t *rb, uint8_t *buf, uint8_t len);
int  RINGBUF_Peek   (ringbuf_t *rb, uint8_t *buf, uint8_t len);
int  RINGBUF_Poll   (ringbuf_t *rb, uint8_t *buf, uint8_t len);

#endif
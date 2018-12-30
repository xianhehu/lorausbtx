#include "ringbuf.h"

void RINGBUF_Init(ringbuf_t *rb, uint8_t *buf, uint32_t size)
{
    memset(rb, 0, sizeof(ringbuf_t));
    rb->buf  = buf;
    rb->size = size;
}

int  RINGBUF_GetLen(ringbuf_t *rb)
{
    int len = 0;

    if (rb->start<=rb->end) {
        return rb->end-rb->start;
    }

    return rb->end+rb->size-rb->start;
}

int  RINGBUF_Put(ringbuf_t *rb, uint8_t *buf, uint8_t len)
{
    uint32_t remain = rb->size-RINGBUF_GetLen(rb);

    if (remain <= len) {
        return -1;
    }

    for (int i=0; i<len; i++) {
        rb->buf[rb->end++] = buf[i];
        if (rb->end>=rb->size)
          rb->end=0;
    }

    return 0;
}

int  RINGBUF_Peek(ringbuf_t *rb, uint8_t *buf, uint8_t len)
{
    uint32_t remain = RINGBUF_GetLen(rb);

    if (remain<len) {
        return -1;
    }

    for (int i=0; i<len; i++) {
        buf[i] = rb->buf[i+rb->start];
    }

    return 0;
}

int  RINGBUF_Poll(ringbuf_t *rb, uint8_t *buf, uint8_t len)
{
    uint32_t remain = RINGBUF_GetLen(rb);

    if (remain < len) {
        return -1;
    }

    for (int i=0; i<len; i++) {
        buf[i] = rb->buf[rb->start++];

        if (rb->start>=rb->size) {
            rb->start = 0;
        }
    }

    return 0;
}


/**
 * @file    RingBuf.c
 * @brief   Byte ring buffer. See RingBuf.h.
 */
#include "RingBuf.h"

static uint32_t RingBuf_Next(const RingBuf_Type *rb, uint32_t index)
{
    index++;
    return (index >= rb->size) ? 0u : index;
}

Std_ReturnType RingBuf_Init(RingBuf_Type *rb, uint8_t *storage, uint32_t size)
{
    if ((rb == NULL) || (storage == NULL) || (size < 2u))
    {
        return E_NOT_OK;
    }
    rb->storage = storage;
    rb->size    = size;
    rb->head    = 0u;
    rb->tail    = 0u;
    return E_OK;
}

Std_ReturnType RingBuf_Put(RingBuf_Type *rb, uint8_t byte)
{
    uint32_t head = rb->head;
    uint32_t next = RingBuf_Next(rb, head);

    if (next == rb->tail)
    {
        return E_NOT_OK;
    }
    rb->storage[head] = byte;
    rb->head          = next; /* publish after the data is written */
    return E_OK;
}

Std_ReturnType RingBuf_Get(RingBuf_Type *rb, uint8_t *byte)
{
    uint32_t tail = rb->tail;

    if (tail == rb->head)
    {
        return E_NOT_OK;
    }
    *byte    = rb->storage[tail];
    rb->tail = RingBuf_Next(rb, tail);
    return E_OK;
}

uint32_t RingBuf_Write(RingBuf_Type *rb, const uint8_t *data, uint32_t len)
{
    uint32_t n = 0u;

    while ((n < len) && (RingBuf_Put(rb, data[n]) == E_OK))
    {
        n++;
    }
    return n;
}

uint32_t RingBuf_Read(RingBuf_Type *rb, uint8_t *data, uint32_t maxLen)
{
    uint32_t n = 0u;

    while ((n < maxLen) && (RingBuf_Get(rb, &data[n]) == E_OK))
    {
        n++;
    }
    return n;
}

uint32_t RingBuf_Count(const RingBuf_Type *rb)
{
    uint32_t head = rb->head;
    uint32_t tail = rb->tail;

    return (head >= tail) ? (head - tail) : (rb->size - tail + head);
}

uint32_t RingBuf_Free(const RingBuf_Type *rb)
{
    return (rb->size - 1u) - RingBuf_Count(rb);
}

void RingBuf_Clear(RingBuf_Type *rb)
{
    rb->tail = rb->head;
}

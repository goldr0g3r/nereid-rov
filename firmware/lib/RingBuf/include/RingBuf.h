/**
 * @file    RingBuf.h
 * @brief   Fixed-capacity byte ring buffer over caller-provided storage.
 *
 * Safe without locking for exactly one producer and one consumer (e.g. ISR -> task),
 * because head is written only by the producer and tail only by the consumer.
 * Multiple producers or consumers must serialise access (critical section).
 * One storage byte is kept free to distinguish full from empty.
 */
#ifndef RINGBUF_H
#define RINGBUF_H

#include "Std_Types.h"

typedef struct
{
    uint8_t          *storage;
    uint32_t          size; /**< storage size in bytes, capacity is size - 1 */
    volatile uint32_t head; /**< next write index (producer) */
    volatile uint32_t tail; /**< next read index (consumer) */
} RingBuf_Type;

Std_ReturnType RingBuf_Init(RingBuf_Type *rb, uint8_t *storage, uint32_t size);

/** Append one byte. Returns E_NOT_OK when full (byte dropped). */
Std_ReturnType RingBuf_Put(RingBuf_Type *rb, uint8_t byte);

/** Remove one byte. Returns E_NOT_OK when empty. */
Std_ReturnType RingBuf_Get(RingBuf_Type *rb, uint8_t *byte);

/** Append up to @p len bytes, returns the number actually written. */
uint32_t RingBuf_Write(RingBuf_Type *rb, const uint8_t *data, uint32_t len);

/** Remove up to @p maxLen bytes, returns the number actually read. */
uint32_t RingBuf_Read(RingBuf_Type *rb, uint8_t *data, uint32_t maxLen);

uint32_t RingBuf_Count(const RingBuf_Type *rb);
uint32_t RingBuf_Free(const RingBuf_Type *rb);
void     RingBuf_Clear(RingBuf_Type *rb);

#endif /* RINGBUF_H */

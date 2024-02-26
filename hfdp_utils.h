#ifndef HFDP_UTILS__
#define HFDP_UTILS__
#include <stdint.h>
#include "hfdp_cfg.h"

#ifndef NULL
#define NULL 0
#endif

#define HFDP_HEAD_MAGIC      0xA0
#define HFDP_HEAD_MAGIC_MASK (7 << 5)
#define HFDP_HEAD_DF_OFFSET  4
#define HFDP_HEAD_BF_OFFSET  3
#define HFDP_HEAD_SEQ_MASK   0x07

unsigned char hfdp_crc8(unsigned char *message, unsigned char byte_len);

typedef struct _ringbuffer {
    uint32_t in;
    uint32_t out;
    uint32_t msize;  // ringbuffer最大大小，实际能存储的数据长度为msize-1
    uint8_t *buffer;
} ringbuffer_t;

int ringbuffer_init(ringbuffer_t *rb, uint8_t *buf, uint32_t size);
uint32_t ringbuffer_out(ringbuffer_t *rb, uint8_t *data, uint32_t len);
uint32_t ringbuffer_in(ringbuffer_t *rb, uint8_t *data, uint32_t len);
uint8_t ringbuffer_is_full(ringbuffer_t *rb, uint32_t len);
uint8_t ringbuffer_is_empty(ringbuffer_t *rb);

#endif  // HFDP_UTILS__

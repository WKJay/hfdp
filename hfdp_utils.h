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

#if (HFDP_NRT_DATA_LEN <= 0xFF)
#define RING_PTR_TYPE uint8_t
#elif (HFDP_NRT_DATA_LEN <= 0xFFFF)
#define RING_PTR_TYPE uint16_t
#elif (HFDP_NRT_DATA_LEN <= 0x7FFFFFFF)
#define RING_PTR_TYPE uint32_t
#else
#error "HFDP_NRT_DATA_LEN is too large"
#endif

typedef struct _hfdp_ring {
    uint8_t buffer[HFDP_NRT_BUFFER_LEN];  // 本地非实时数据缓存
    RING_PTR_TYPE len;                    // 本地非实时数据长度
    RING_PTR_TYPE head;                   // 本地非实时数据头指针
    RING_PTR_TYPE tail;                   // 本地非实时数据尾指针
} hfdp_ring_t;

unsigned char hfdp_crc8(unsigned char *message, unsigned char byte_len);

int hfdp_ring_init(hfdp_ring_t *ring);
uint8_t hfdp_ring_is_full(hfdp_ring_t *ring, RING_PTR_TYPE len);
uint8_t hfdp_ring_is_empty(hfdp_ring_t *ring);
int hfdp_ring_enqueue(hfdp_ring_t *ring, uint8_t *data, RING_PTR_TYPE len);
int hfdp_ring_dequeue(hfdp_ring_t *ring, uint8_t *data, RING_PTR_TYPE len);
#endif  // HFDP_UTILS__

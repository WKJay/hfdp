#ifndef HFDP_H__
#define HFDP_H__

#include "hfdp_utils.h"
#include "hfdp_cfg.h"

#define HFDP_HEAD_LEN 1

typedef void (*rtd_rd_cb_t)(uint8_t *rtd, uint8_t remote_rtd_len);
typedef void (*rtd_wr_cb_t)(uint8_t *data, uint8_t max_data_len);
typedef int (*hfdp_hw_send_t)(uint8_t *data, uint8_t len);

typedef struct _hfdp_mgr {
    uint8_t *rtd_ptr;                           // 实时数据指针，指向 master 报文中的实时数据
    uint8_t send_buffer[HFDP_SEND_BUFFER_LEN];  // 发送缓存
    uint8_t remote_rtd_len;                     // 远端实时数据长度
    uint8_t local_rtd_len;                      // 本地实时数据长度
    uint8_t remote_nrtd_len;                    // 远端非实时数据长度
    uint8_t local_nrtd_len;                     // 本地非实时数据长度
    uint8_t seq;                                // 报文序列号
    uint8_t remote_bf : 1;                      // 远端缓存满标志
    uint8_t local_bf : 1;                       // 本地缓存满标志
    ringbuffer_t recv_nrtd;                     // 接收到的非实时数据缓存
    ringbuffer_t send_nrtd;                     // 待发送的非实时数据缓存
    rtd_rd_cb_t rtd_rd_cb;                      // 实时数据接收回调
    rtd_wr_cb_t rtd_wr_cb;                      // 实时数据发送回调
    hfdp_hw_send_t hfdp_hw_send;                // 硬件发送接口
} hfdp_mgr_t;

typedef struct _hfdp_cfg {
    rtd_rd_cb_t rtd_rd_cb;        // 实时数据接收回调
    rtd_wr_cb_t rtd_wr_cb;        // 实时数据发送回调
    hfdp_hw_send_t hfdp_hw_send;  // 硬件发送接口
    uint8_t *recv_nrtd_buf;       // 接收非实时数据缓存
    uint8_t *send_nrtd_buf;       // 发送非实时数据缓存
    uint32_t recv_nrtd_len;       // 非实时数据缓存大小
    uint32_t send_nrtd_len;        // 远端实时数据长度
} hfdp_cfg_t;

int hfdp_mgr_init(hfdp_mgr_t *mgr, hfdp_cfg_t *cfg);
int hfdp_mgr_set_remote_dlen(hfdp_mgr_t *mgr, uint8_t rtd_len, uint8_t nrtd_len);
int hfdp_mgr_set_local_dlen(hfdp_mgr_t *mgr, uint8_t rtd_len, uint8_t nrtd_len);

#endif  // HFDP_H__

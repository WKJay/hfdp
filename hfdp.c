/***************************************************************
 * @file           hfdp.c
 * @brief
 * @author         WKJay
 * @Version
 * @Date           2024-02-05
 ***************************************************************/
#include "hfdp.h"
#include "hfdp_utils.h"

int hfdp_mgr_init(hfdp_mgr_t *mgr, hfdp_cfg_t *cfg) {
    if (mgr == NULL) {
        HFDP_LOG("mgr is NULL\r\n");
        return -1;
    }

    mgr->rtd_ptr = NULL;
    mgr->seq = 0;
    mgr->remote_bf = 0;
    mgr->local_bf = 0;

    mgr->rtd_rd_cb = cfg->rtd_rd_cb;
    mgr->rtd_wr_cb = cfg->rtd_wr_cb;
    mgr->hfdp_hw_send = cfg->hfdp_hw_send;

    if (HFDP_SEND_BUFFER_LEN < HFDP_HEAD_LEN + mgr->local_nrtd_len + mgr->local_rtd_len + 1) {
        HFDP_LOG("send buffer is too small\r\n");
        return -1;
    }

    hfdp_ring_init(&mgr->recv_nrtd);
    hfdp_ring_init(&mgr->send_nrtd);

    return 0;
}

int hfdp_mgr_set_remote_dlen(hfdp_mgr_t *mgr, uint8_t rtd_len, uint8_t nrtd_len) {
    if (mgr == NULL) {
        HFDP_LOG("mgr is NULL\r\n");
        return -1;
    }

    mgr->remote_rtd_len = rtd_len;
    mgr->remote_nrtd_len = nrtd_len;

    return 0;
}

int hfdp_mgr_set_local_dlen(hfdp_mgr_t *mgr, uint8_t rtd_len, uint8_t nrtd_len) {
    if (mgr == NULL) {
        HFDP_LOG("mgr is NULL\r\n");
        return -1;
    }

    mgr->local_rtd_len = rtd_len;
    mgr->local_nrtd_len = nrtd_len;

    return 0;
}

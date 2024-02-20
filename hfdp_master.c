/***************************************************************
 * @file           hfdp_master.c
 * @brief
 * @author         WKJay
 * @Version
 * @Date           2024-02-06
 ***************************************************************/
#include "hfdp_master.h"
#include "hfdp_utils.h"

int hfdp_master_init(hfdp_master_t *master, hfdp_cfg_t *cfg) {
    if (master == NULL) {
        HFDP_LOG("master is NULL\r\n");
        return -1;
    }

    master->mgr.remote_rtd_len = 1;
    master->mgr.local_rtd_len = 3;
    master->mgr.remote_nrtd_len = 1;
    master->mgr.local_nrtd_len = 1;

    master->local_seq = HFDP_MASTER_SEQ_START;
    return hfdp_mgr_init(&master->mgr, cfg);
}

int hfdp_master_request(hfdp_master_t *master) {
    uint8_t send_len = 0;
    if (master == NULL) {
        HFDP_LOG("invalid incoming params\r\n");
        return -1;
    }

    // 头
    if (master->local_seq == master->mgr.seq) {
        master->local_seq++;
        if (master->local_seq > HFDP_MASTER_SEQ_END) {
            master->local_seq = HFDP_MASTER_SEQ_START;
        }
    }
    master->mgr.send_buffer[0] = HFDP_HEAD_MAGIC | (master->mgr.local_bf << HFDP_HEAD_BF_OFFSET) | (master->local_seq & HFDP_HEAD_SEQ_MASK);
    send_len = 1;

    // RTD 数据
    if (master->mgr.local_rtd_len) {
        if (master->mgr.rtd_wr_cb) {
            master->mgr.rtd_wr_cb(master->mgr.send_buffer + send_len, master->mgr.local_rtd_len);
        }
        send_len += master->mgr.local_rtd_len;
    }

    // NRTD 数据
    // 如果对方非实时数据缓存满了，则此处 DF 标志位不会被置位
    if ((!master->mgr.remote_bf) && (!hfdp_ring_is_empty(&master->mgr.send_nrtd))) {
        master->mgr.send_buffer[0] |= 1 << HFDP_HEAD_DF_OFFSET;
        hfdp_ring_dequeue(&master->mgr.send_nrtd, master->mgr.send_buffer + send_len, master->mgr.local_nrtd_len);
    }
    // ! 无论有没有NRTD数据，数据帧中都会有对应的数据位存在，并且长度固定
    //   因此实际的有效数据需要应用层去判断和解析
    send_len += master->mgr.local_nrtd_len;

    // crc
    master->mgr.send_buffer[send_len] = hfdp_crc8(master->mgr.send_buffer, send_len);
    send_len++;

    // 发送
    if (master->mgr.hfdp_hw_send) {
        master->mgr.hfdp_hw_send(master->mgr.send_buffer, send_len);
    }

    return 0;
}

int hfdp_master_response_handler(hfdp_master_t *master, uint8_t *data, uint8_t len) {
    uint8_t crc = 0;
    if (master == NULL || data == NULL || len == 0) {
        HFDP_LOG("invalid incoming params\r\n");
        return -1;
    }

    if (len != HFDP_HEAD_LEN + master->mgr.remote_rtd_len + master->mgr.remote_nrtd_len + 1) {
        HFDP_LOG("invalid data length\r\n");
        return -1;
    }

    crc = hfdp_crc8(data, len - 1);
    if (crc != data[len - 1]) {
        HFDP_LOG("crc error\r\n");
        return -1;
    }

    if ((data[0] & HFDP_HEAD_MAGIC_MASK) != HFDP_HEAD_MAGIC) {
        HFDP_LOG("magic error\r\n");
        return -1;
    }

    master->mgr.seq = data[0] & HFDP_HEAD_SEQ_MASK;
    master->mgr.remote_bf = data[0] >> HFDP_HEAD_BF_OFFSET & 0x1;

    if (master->mgr.rtd_rd_cb) {
        master->mgr.rtd_rd_cb(data + HFDP_HEAD_LEN, master->mgr.remote_rtd_len);
    }

    // 存在非实时数据
    if (data[0] >> HFDP_HEAD_DF_OFFSET & 0x1) {
        if (master->mgr.local_bf == 0) {
            hfdp_ring_enqueue(&master->mgr.recv_nrtd, data + HFDP_HEAD_LEN + master->mgr.remote_rtd_len, master->mgr.remote_nrtd_len);
            // 后续是否会满
            if (hfdp_ring_is_full(&master->mgr.recv_nrtd, master->mgr.remote_nrtd_len)) {
                master->mgr.local_bf = 1;
            }
        }
    }

    return 0;
}

int hfdp_master_write_nrtd(hfdp_master_t *master, uint8_t *data, uint32_t len) {
    if (master == NULL || data == NULL || len == 0) {
        HFDP_LOG("invalid incoming params\r\n");
        return -1;
    }

    if (hfdp_ring_is_full(&master->mgr.send_nrtd, len)) {
        HFDP_LOG("send nrtd ring is full\r\n");
        return -1;
    }

    return hfdp_ring_enqueue(&master->mgr.send_nrtd, data, len);
}

int hfdp_master_read_nrtd(hfdp_master_t *master, uint8_t *buf, uint32_t len) {
    int ret = -1;
    if (master == NULL || buf == NULL) {
        HFDP_LOG("invalid incoming params\r\n");
        return -1;
    }
    if (len == 0) return 0;

    if (hfdp_ring_is_empty(&master->mgr.recv_nrtd)) {
        return 0;
    }

    ret = hfdp_ring_dequeue(&master->mgr.recv_nrtd, buf, len);

    if (master->mgr.local_bf) {
        if (!hfdp_ring_is_full(&master->mgr.recv_nrtd, master->mgr.remote_nrtd_len)) {
            master->mgr.local_bf = 0;
        }
    }

    return ret;
}

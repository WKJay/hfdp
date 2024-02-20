/***************************************************************
 * @file           hfdp_slave.c
 * @brief
 * @author         WKJay
 * @Version
 * @Date           2024-02-04
 * 特性：
 * 1.可协商数据格式，从机可以根据主机的要求返回不同的数据格式
 ***************************************************************/
#include "hfdp_slave.h"
#include "hfdp_utils.h"

int hfdp_slave_init(hfdp_slave_t *slave, hfdp_cfg_t *cfg) {
    if (slave == NULL) {
        HFDP_LOG("slave is NULL\r\n");
        return -1;
    }

    slave->mgr.remote_rtd_len = 3;
    slave->mgr.local_rtd_len = 1;
    slave->mgr.remote_nrtd_len = 1;
    slave->mgr.local_nrtd_len = 1;

    return hfdp_mgr_init(&slave->mgr, cfg);
}

/**
 * @brief 从机响应主机请求
 * @param slave 从机句柄
 * @param data 主机请求数据包
 * @param len 主机请求数据长度
 * @return 0 成功 -1 失败
 * @note
 *  - 该函数设计为基于帧的响应，也就是说在调用该函数时必须保证 data 缓冲
 *    区中有完整的一帧数据，否则解析出错
 */
int hfdp_slave_response(hfdp_slave_t *slave, uint8_t *data, uint8_t len) {
    uint8_t crc = 0, send_len = 0;
    if (slave == NULL || data == NULL || len == 0) {
        HFDP_LOG("invalid incoming params\r\n");
        return -1;
    }

    if (len != HFDP_HEAD_LEN + slave->mgr.remote_rtd_len + slave->mgr.remote_nrtd_len + 1) {
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

    slave->mgr.seq = data[0] & HFDP_HEAD_SEQ_MASK;
    slave->mgr.remote_bf = data[0] >> HFDP_HEAD_BF_OFFSET & 0x1;

    if (slave->mgr.rtd_rd_cb) {
        slave->mgr.rtd_rd_cb(data + HFDP_HEAD_LEN, slave->mgr.remote_rtd_len);
    }

    // 存在非实时数据
    if (data[0] >> HFDP_HEAD_DF_OFFSET & 0x1) {
        if (slave->mgr.local_bf == 0) {
            hfdp_ring_enqueue(&slave->mgr.recv_nrtd, data + HFDP_HEAD_LEN + slave->mgr.remote_rtd_len, slave->mgr.remote_nrtd_len);
            // 后续是否会满
            if (hfdp_ring_is_full(&slave->mgr.recv_nrtd, slave->mgr.remote_nrtd_len)) {
                slave->mgr.local_bf = 1;
            }
        }
    }

    // 响应部分
    slave->mgr.send_buffer[0] = HFDP_HEAD_MAGIC | (slave->mgr.local_bf << HFDP_HEAD_BF_OFFSET) | (slave->mgr.seq & HFDP_HEAD_SEQ_MASK);
    send_len = 1;

    // 实时数据部分
    if (slave->mgr.local_rtd_len) {
        if (slave->mgr.rtd_wr_cb) {
            slave->mgr.rtd_wr_cb(slave->mgr.send_buffer + send_len, slave->mgr.local_rtd_len);
        }
        send_len += slave->mgr.local_rtd_len;
    }

    // 非实时数据部分
    // 如果对方非实时数据缓存满了，则此处 DF 标志位不会被置位
    if ((!slave->mgr.remote_bf) && (!hfdp_ring_is_empty(&slave->mgr.send_nrtd))) {
        slave->mgr.send_buffer[0] |= 1 << HFDP_HEAD_DF_OFFSET;
        hfdp_ring_dequeue(&slave->mgr.send_nrtd, slave->mgr.send_buffer + send_len, slave->mgr.local_nrtd_len);
    }
    // ! 无论有没有NRTD数据，数据帧中都会有对应的数据位存在，并且长度固定
    //   因此实际的有效数据需要应用层去判断和解析
    send_len += slave->mgr.local_nrtd_len;

    // crc
    slave->mgr.send_buffer[send_len] = hfdp_crc8(slave->mgr.send_buffer, send_len);
    send_len++;

    // 发送
    if (slave->mgr.hfdp_hw_send) {
        slave->mgr.hfdp_hw_send(slave->mgr.send_buffer, send_len);
    }

    return 0;
}

int hfdp_slave_write_nrtd(hfdp_slave_t *slave, uint8_t *data, uint8_t len) {
    if (slave == NULL || data == NULL || len == 0) {
        return -1;
    }

    if (hfdp_ring_is_full(&slave->mgr.send_nrtd, len)) {
        return -1;
    }

    hfdp_ring_enqueue(&slave->mgr.send_nrtd, data, len);

    return 0;
}

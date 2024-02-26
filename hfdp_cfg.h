#ifndef HFDP_CFG_H__
#define HFDP_CFG_H__

#define HFDP_DEBUG 1  // 是否开启调试信息
#if (HFDP_DEBUG)
#include <stdio.h>
#define HFDP_PRINTF printf
#endif

#define HFDP_SEND_BUFFER_LEN 8   // 发送缓存长度

#define HFDP_DEFAULT_REMOTE_RTD_LEN  3  // 默认远端实时数据长度
#define HFDP_DEFAULT_REMOTE_NRTD_LEN 1  // 默认远端非实时数据长度
#define HFDP_DEFAULT_LOCAL_RTD_LEN   0  // 默认本地实时数据长度
#define HFDP_DEFAULT_LOCAL_NRTD_LEN  1  // 默认本地非实时数据长度

#define HFDP_MASTER_SEQ_START 1  // master 报文序列号起始值
#define HFDP_MASTER_SEQ_END   7  // master 报文序列号结束值

#if (HFDP_DEBUG)
#define HFDP_LOG(fmt, ...)                                \
    do {                                                  \
        HFDP_PRINTF("[HFDP]-%s-%d:", __FILE__, __LINE__); \
        HFDP_PRINTF(fmt, ##__VA_ARGS__);                  \
    } while (0)
#else
#define HFDP_LOG(fmt, ...)
#endif

#endif  // HFDP_CFG_H__

#ifndef HFDP_MASTER__
#define HFDP_MASTER__
#include "hfdp.h"

typedef struct _hfdp_master {
    hfdp_mgr_t mgr;
    uint8_t local_seq;
} hfdp_master_t;

int hfdp_master_init(hfdp_master_t *master, hfdp_cfg_t *cfg);
int hfdp_master_request(hfdp_master_t *master);
int hfdp_master_response_handler(hfdp_master_t *master, uint8_t *data, uint8_t len);
int hfdp_master_write_nrtd(hfdp_master_t *master, uint8_t *data, uint8_t len);
#endif  // HFDP_MASTER__

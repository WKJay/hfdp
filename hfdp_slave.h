#ifndef HFDP_SLAVE_H__
#define HFDP_SLAVE_H__
#include "hfdp.h"

typedef struct _hfdp_slave {
    hfdp_mgr_t mgr;
} hfdp_slave_t;

int hfdp_slave_init(hfdp_slave_t *slave, hfdp_cfg_t *cfg);
int hfdp_slave_response(hfdp_slave_t *slave, uint8_t *data, uint8_t len);
int hfdp_slave_write_nrtd(hfdp_slave_t *slave, uint8_t *data, uint8_t len);
#endif  // HFDP_SLAVE_H__

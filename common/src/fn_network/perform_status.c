#include <stdint.h>
#include "fujinet-network.h"

#if defined(__ATARI__) || defined(__APPLE2__) || defined(__CBM__)
uint8_t network_status_unit(uint8_t unit, uint16_t *bw, uint8_t *c, uint8_t *err);
#endif

uint8_t unit = 0;

uint8_t perform_status(const char *devicespec) {
    uint8_t r = 0;
#if defined(__ATARI__) || defined(__APPLE2__) || defined(__CBM__)
    // use versions that take the unit number to avoid a lookup from the devicespec
    r = network_status_unit(unit, &fn_network_bw, &fn_network_conn, &fn_network_error);
#elif defined(_CMOC_VERSION_) || defined(__PMD85__)
    r = network_status(devicespec, &fn_network_bw, &fn_network_conn, &fn_network_error);
#endif

    return r;
}
#include <stdint.h>
#include <string.h>
#include "fujinet-network.h"
#include "fujinet-bus-apple2.h"

extern uint8_t bad_unit(void);

// this version is used by network_read to avoid a lookup from the devicespec
uint8_t network_status_unit(uint8_t unit, uint16_t *bw, uint8_t *c, uint8_t *err) {
	uint8_t err_status;

	if (sp_network == 0) {
		return bad_unit();
	}

	fn_device_error = 0;
	sp_nw_unit = unit;

	err_status = sp_status_nw(sp_network, 'S'); // network status

	*bw = ((uint16_t)sp_payload[1] << 8) | sp_payload[0];
	*c = sp_payload[2];
	*err = sp_payload[3];

	return fn_error(err_status);
}
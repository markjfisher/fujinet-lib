#include <stdbool.h>
#include <stdint.h>
#include "fujinet-fuji.h"

bool fuji_scan_for_networks(uint8_t *count)
{
	int bytes_read;
	
	if (cbm_open(FUJI_CMD_CHANNEL, FUJI_CBM_DEV, FUJI_CMD_CHANNEL, "\xFD") != 0) {
		return false;
	}

	bytes_read = cbm_read(FUJI_CMD_CHANNEL, count, 1);
	cbm_close(FUJI_CMD_CHANNEL);
	return bytes_read > 0;
}

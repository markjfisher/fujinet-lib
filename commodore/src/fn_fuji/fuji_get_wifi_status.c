#include <stdbool.h>
#include <stdint.h>
#include "fujinet-fuji.h"

bool fuji_get_wifi_status(uint8_t *status)
{
	int bytes_read;

	if (cbm_open(FUJI_CMD_CHANNEL, FUJI_CBM_DEV, FUJI_CMD_CHANNEL, "\xFA") != 0) {
		return false;
	}

	bytes_read = cbm_read(FUJI_CMD_CHANNEL, status, 1);
	cbm_close(FUJI_CMD_CHANNEL);
	return bytes_read > 0;
}

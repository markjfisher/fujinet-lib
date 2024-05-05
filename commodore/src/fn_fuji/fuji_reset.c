#include <stdbool.h>
#include <stdint.h>

#include "fujinet-fuji.h"

bool fuji_reset()
{
	if (cbm_open(FUJI_CMD_CHANNEL, FUJI_CBM_DEV, FUJI_CMD_CHANNEL, "\xFF") != 0) {
		return false;
	}
	cbm_close(FUJI_CMD_CHANNEL);
	return true;
}

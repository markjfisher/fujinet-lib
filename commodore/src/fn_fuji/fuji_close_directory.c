#include <stdbool.h>
#include <stdint.h>
#include "fujinet-fuji.h"

bool fuji_close_directory()
{
	if (cbm_open(FUJI_CMD_CHANNEL, FUJI_CBM_DEV, FUJI_CMD_CHANNEL, "\xF5") != 0) {
		return false;
	}
	cbm_close(FUJI_CMD_CHANNEL);
	return true;
}

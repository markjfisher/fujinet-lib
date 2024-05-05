#include <stdbool.h>
#include <stdint.h>
#include "fujinet-fuji.h"

bool fuji_get_scan_result(uint8_t n, SSIDInfo *ssid_info)
{
	int bytes_read;
	uint8_t pl[3]; // CC65 can't do direct assignment: uint8_t pl[3] = {0xFC, n, 0x00};
	pl[0] = 0xFC;
	pl[1] = n;
	pl[2] = 0x00;

	if (cbm_open(FUJI_CMD_CHANNEL, FUJI_CBM_DEV, FUJI_CMD_CHANNEL, (char *) pl) != 0) {
		return false;
	}

	bytes_read = cbm_read(FUJI_CMD_CHANNEL, ssid_info, sizeof(SSIDInfo));
	cbm_close(FUJI_CMD_CHANNEL);
	return bytes_read > 0;
}

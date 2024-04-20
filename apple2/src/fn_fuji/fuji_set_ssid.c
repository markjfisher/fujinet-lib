#include <stdint.h>
#include <string.h>
#include "fujinet-fuji.h"
#include "fujinet-bus-apple2.h"

// This version does NOT send an ENCODING byte, this is detected on the FujiNet by the length being exactly 97 AND that the first byte of data is NOT 0x01.
// That will allow old code to still work with the FujiNet, and not need recompiling to send the additional encoding byte.
bool fuji_set_ssid(NetConfig *nc)
{
	uint8_t nc_len = sizeof(NetConfig);

    sp_error = sp_get_fuji_id();
	if (sp_error <= 0) {
		return false;
	}

	sp_payload[0] = nc_len; // low byte of 97
	sp_payload[1] = 0;      // high byte of 97
	memcpy(&sp_payload[2], nc, nc_len);

	sp_error = sp_control(sp_fuji_id, 0xFB);
	return sp_error == 0;

}

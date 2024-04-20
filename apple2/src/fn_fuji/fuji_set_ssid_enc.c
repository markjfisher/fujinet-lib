#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "fujinet-fuji.h"
#include "fujinet-bus-apple2.h"

bool needs_encoding(char c) {
    // Characters that are safe and do not need encoding
    // Alphanumeric characters and a few special characters are considered safe.
	// Note: SPACE -> %20, and NOT '+'
    if ((c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') ||
        c == '-' || c == '_' || c == '.' || c == '~') {
        return false;
    }
    return true;
}

void encode_char(char c, char* output) {
	// Avoid using sprintf in library, it draws in a HUGE amount of cc65 library
    const char hexChars[] = "0123456789ABCDEF";
    output[0] = '%';
    output[1] = hexChars[((unsigned char)c >> 4) & 0xF]; // Extract and convert the high nibble
    output[2] = hexChars[(unsigned char)c & 0xF]; 		 // Extract and convert the low nibble
}

// return status of encoding string, false indicates there was an error
bool url_encode(const char* input, char** encoded) {
	char* output;

    // Allocate memory for the worst-case scenario where each character needs encoding.
    *encoded = (char*)malloc(3 * strlen(input) + 1);
    output = *encoded;

	if (!output) {
		// malloc failed, exit and let the caller know
		return false;
	}

    while (*input) {
        if (needs_encoding(*input)) {
            encode_char(*input, output);
            output += 3;
        } else {
            *output++ = *input;
        }
        input++;
    }
    *output = '\0'; // Null-terminate the encoded string
	return true;
}

// return status of encoding strings, false means there was an error
bool url_encode_netconfig(NetConfig *nc, char** encoded_ssid, char** encoded_password) {
    bool encoded_status;
	encoded_status = url_encode(nc->ssid, encoded_ssid);
	if (!encoded_status) return false;

    return url_encode(nc->password, encoded_password);
}

bool fuji_set_ssid_enc(NetConfig *nc, enum SSIDInfoEncoding e)
{
	char *encoded_ssid = NULL;
	char *encoded_password = NULL;
	int ssid_len = 0;
	int pass_len = 0;
	int data_len = 0;
	bool encoded_status;

    sp_error = sp_get_fuji_id();
	if (sp_error <= 0) {
		return false;
	}

	// we need to act on the encoding type, if it's 'none' there's nothing to do, otherwise use the appropriate encoding specified
	switch (e) {
		case url_encoded:
			encoded_status = url_encode_netconfig(nc, &encoded_ssid, &encoded_password);
			if (!encoded_status) return false;
			break;
		default:
			// no encoding, so do the 'standard' version so we skip all the encoding etc.
			// It would be better for the caller to use that version directly so this whole file's code is not included in the application
			return fuji_set_ssid(nc);
			break;
	}

	if (encoded_ssid == NULL) {
		// indicate setting ssid failed, as we could not allocate memory for the ssid
		return false;
	}
	if (encoded_password == NULL) {
		// indicate setting ssid failed, as we could not allocate memory for the password.
		// we must free the ssid though, as it must have succeeded
		free(encoded_ssid);
		return false;
	}

	ssid_len = strlen(encoded_ssid) + 1;      // add 1 for NUL byte at end of string
	pass_len = strlen(encoded_password) + 1;
	data_len = ssid_len + pass_len + 1;       // add 1 for encoding byte

	// send the length
	sp_payload[0] = data_len & 0xFF;
	sp_payload[1] = (data_len >> 8) & 0xFF;
	// send the encoding used - NOTE: THIS REQUIRES FIRMWARE THAT EXPECTS AN ENCODED BYTE TO BE SENT
	sp_payload[2] = e;

	// send the encoded strings, note the strings include the NUL terminator, and that is copied into the payload too.
	memcpy(&sp_payload[3], encoded_ssid, ssid_len);
	memcpy(&sp_payload[3 + ssid_len], encoded_password, pass_len);

	sp_error = sp_control(sp_fuji_id, 0xFB);

	// free up the strings
	free(encoded_ssid);
	free(encoded_password);

	// return the status
	return sp_error == 0;

}

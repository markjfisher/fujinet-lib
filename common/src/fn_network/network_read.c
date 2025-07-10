#ifdef _CMOC_VERSION_
#include <cmoc.h>
#include <coco.h>
#include <fujinet-network-coco.h>
#include <fujinet-fuji-coco.h>
#include <dw.h>
#else
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#endif /* _CMOC_VERSION_ */
#include "fujinet-network.h"

#ifdef __ATARI__
#include "fujinet-network-atari.h"
#include "fujinet-bus-atari.h"
#include <atari.h>
#endif

#ifdef __APPLE2__
#include "fujinet-network-apple2.h"
#include "fujinet-bus-apple2.h"
#include "sp.h"
#endif

#ifdef __CBM__
#include "fujinet-network-cbm.h"
#endif

#ifdef __PMD85__
#include "fujinet-network-pmd85.h"
#include "fujinet-fuji-pmd85.h"
#include "dw.h"
#endif

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define MAX_READ_SIZE 512

// #include <stdio.h>

extern uint8_t unit;
extern uint8_t perform_status(const char *devicespec);

int16_t network_read(const char *devicespec, uint8_t *buf, uint16_t len)
{
    uint8_t r = 0;
    uint16_t fetch_size = 0;
    uint16_t amount_left = len;
    uint16_t total_read = 0;

#if defined(_CMOC_VERSION_) || defined(__PMD85__)
    struct _r
    {
        uint8_t opcode;
        uint8_t unit;
        uint8_t cmd;
        uint16_t len;
    } read_r;
#endif

#if defined(__CBM__)
    const char *after;
#endif

#if defined(__ATARI__) || defined(_CMOC_VERSION_) || defined(__CBM__) || defined(__PMD85__)
    unit = 0;
#endif

    if (len == 0 || buf == NULL) {

#if defined(__ATARI__)
        return -fn_error(132); // invalid command
#elif defined(__APPLE2__)
        return -fn_error(SP_ERR_BAD_CMD);
#elif defined(__CBM__)
        return -FN_ERR_BAD_CMD;
#elif defined(_CMOC_VERSION_) || defined(__PMD85__)
        return -fn_error(132); // invalid command
#endif

    }

#ifdef __APPLE2__
    // check we have the SP network value
    if (sp_network == 0) {
        return -fn_error(SP_ERR_BAD_UNIT);
    }
#endif

    fn_bytes_read = 0;
    fn_device_error = 0;

#if defined(__ATARI__) || defined(_CMOC_VERSION_) || defined(__PMD85__)
    unit = network_unit(devicespec);
#elif defined(__CBM__)
    unit = getDeviceNumber(devicespec, &after);
#endif

    while (1) {
        // exit condition
        if (amount_left == 0) {
            // printf("amount_left == 0, breaking\n");
            // the FN is still pinging the interrupt until we get an EOF, not just when the amount left is 0, we need to fetch the EOF status if it is there
            perform_status(devicespec);
            break;
        }

#if defined(__ATARI__)
        // Check if we have enabled VPRCED checking, and there is a trip before calling status, otherwise tight loop.
        // Another change eventually here would be to also support a callback function the user provides
        // so they can give feedback in their application while there's data being read
        if (network_read_interrupt_enabled && !network_read_trip) {
            // No data available, reloop
            // printf("network_read_trip == false, continuing\n");
            continue;
        }
#endif
        r = perform_status(devicespec);

        // check if the status failed. The buffer may be partially filled, up to client if they want to use any of it. The count is in fn_bytes_read
        if (r != 0) {
            fn_bytes_read = total_read;
#if defined(__ATARI__)
            network_read_trip = false;
#endif
            // r is the FN_ERR code
            return -r;
        }

        // EOF hit, exit reading
        if (fn_network_error == 136) {
            // printf("fn_network_error == 136, breaking\n");
            break;
        }

        // is there another error? The buffer may be partially filled, up to client if they want to use any of it. The count is in fn_bytes_read
        if (fn_network_error != 1) {
            fn_bytes_read = total_read;
#if defined(__ATARI__)
            network_read_trip = false;
#endif
            return -FN_ERR_IO_ERROR;
        }

        // we are waiting for bytes to become available while still connected.
        // Causes tight loop if there's a long delay reading from network into FN, so we may see lots of status requests
        if (fn_network_bw == 0 && fn_network_conn == 1) {
            // printf("fn_network_bw == 0 && fn_network_conn == 1, continuing\n");
            continue;
        }

        fetch_size = MIN(amount_left, fn_network_bw);
#ifdef __APPLE2__
        // need to validate this is only required for apple
        fetch_size = MIN(fetch_size, MAX_READ_SIZE);
#endif

#if defined(__ATARI__)
        sio_read(unit, buf, fetch_size);

#elif defined(__APPLE2__)
        sp_read_nw(sp_network, fetch_size);
        memcpy(buf, sp_payload, fetch_size);
#elif defined(__CBM__)
        cbm_read(unit + CBM_DATA_CHANNEL_0, buf, fetch_size);
#endif
#if defined(_CMOC_VERSION_) || defined(__PMD85__)
        read_r.opcode = OP_NET;
        read_r.unit = unit;
        read_r.cmd = 'R';
  #if defined(_CMOC_VERSION_)
        read_r.len = fetch_size;
  #else
        read_r.len = (fetch_size << 8) | (fetch_size >> 8);
  #endif
        bus_ready();
        dwwrite((uint8_t *)&read_r, sizeof(read_r));
        network_get_response(unit, buf, fetch_size);
#endif // _CMOC_VERSION_ || __PMD85__
        buf += fetch_size;
        amount_left -= fetch_size;
        total_read += fetch_size;

#if defined(__ATARI__)
        // printf("set trip:0, rearming\n");
        network_read_trip = false;
        // rearm the interrupt
        PIA.pactl |= 1;
#endif
    }

#if defined(__ATARI__)
    // printf("setting trip to false at end\n");
    network_read_trip = false;
#endif

    // do this here at the end, not in the loop so sio_read for atari can continue to set fn_bytes_read for short reads.
    fn_bytes_read = total_read;
    return total_read;
}

#include <cmoc.h>
#include <coco.h>
#include "fujinet-fuji.h"

bool fuji_set_sio_external_clock(uint16_t rate)
{
    // Not needed on coco, at all.
    return true;
}

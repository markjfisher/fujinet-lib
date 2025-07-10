/**
 * read interrupt handling
 *
 * Allows hosts that support having an interrupt to indicate there are network bytes ready to
 * enable or disable the functionality. It is defaulted to enabled where supported, but applications
 * that already use whatever the platform's mechanism is, can disable it by changing network_read_interrupt_enabled
 */

#ifdef _CMOC_VERSION_
#include "stdbool-coco.h"
#else
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#endif


// Deliberately making this directly available, as we're on 8 bit machines, so don't need full getters/setters. This isn't java.
bool network_read_interrupt_enabled = true;

#ifdef __ATARI__
#include <atari.h>

extern void network_read_ih(void);

void   *old_vprced        = NULL;       /* capture the old vector so we can restore it when disabled */
bool    is_enabled        = false;      /* have we already enabled the interrupt - stop us from losing old vector */
uint8_t old_pactl_bit0    = 0;          /* store the old bit0 of pactl we force to 1 */
bool    network_read_trip = false;      /* the flag set by the interrupt handler */
uint8_t network_has_trip  = 0;

void network_read_interrupt_disable(void) {
    if (old_vprced == NULL) return;

    PIA.pactl  &= 0xFE;             /* Disable interrupts. */
    OS.vprced   = old_vprced;       /* Bring back old PROCEED vector */
    PIA.pactl  |= old_pactl_bit0;   /* Bring back original PACTL state. */
    old_vprced  = NULL;
    is_enabled  = false;
    network_read_trip = false;
    network_has_trip  = 0;

}

void network_read_interrupt_enable(void) {
    if (!network_read_interrupt_enabled) return;
    if (is_enabled) return;

    network_read_trip = false;
    network_has_trip  = 0;
    is_enabled        = true;
    old_vprced        = OS.vprced;       /* Store old VPRCED vector for exit  */
    old_pactl_bit0    = PIA.pactl & 1;   /* Keep track of old interrupt state */
    PIA.pactl        &= 0xFE;            /* Turn off interrupts momentarily.  */
    OS.vprced         = network_read_ih; /* Set interrupt handler.            */
    PIA.pactl        |= 1;               /* Tell PIA we are ready.            */
}

#endif

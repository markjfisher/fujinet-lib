        .export     _network_close

        .import     _bus
        .import     _bus_status
        .import     _fn_device_error
        .import     _network_unit
        .import     _network_read_interrupt_disable
        .import     _network_open_devices
        .import     _copy_network_cmd_data
        .import     popax

        .include    "device.inc"
        .include    "fujinet-network.inc"
        .include    "zp.inc"
        .include    "macros.inc"

; uint8_t network_close(const char* devicespec)
.proc _network_close
        ldy     #$00
        sty     _fn_device_error

        ; get the network unit for this device, A/X already set correctly
        jsr     _network_unit
        sta     tmp8                    ; save the UNIT

        setax   #t_network_close
        jsr     _copy_network_cmd_data

        ; add our specifics, and call SIO
        mva     tmp8, IO_DCB::dunit
        ; mva     #$00, IO_DCB::dbuflo
        ; sta     IO_DCB::dbufhi
        jsr     _bus

        lda     tmp8
        jsr     _bus_status

        pha                     ; save the error code
        cmp     #FN_ERR_OK
        ; is it was not OK, then skip decreasing the open count, as the device was unable to close.
        ; this is EXTREMELY unlikely.
        bne     :+

        ; no error, so decrement the open devices count
        dec     _network_open_devices
        bne     :+              ; did we reach 0?

        ; yes, attempt to disable the interrupt. This is idempotent.
        jsr     _network_read_interrupt_disable

:
        ; restore X, and A to their correct values from the bus status
        ldx     #$00
        pla     ; ensures Z flag has the correct value based on the error status in case we did a DEC above
        rts

.endproc

.rodata
t_network_close:
        .byte 'C', 0, 0, 0, 0, 0

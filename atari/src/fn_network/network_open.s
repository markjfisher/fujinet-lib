        .export     _network_open
        .export     _network_open_devices

        .import     _bus
        .import     _fn_device_error
        .import     _bus_status
        .import     _network_unit
        .import     _copy_network_cmd_data
        .import     _network_read_interrupt_enable
        .import     fn_open_mode_table
        .import     fn_open_trans_table
        .import     popa
        .import     popax

        .include    "device.inc"
        .include    "fujinet-network.inc"
        .include    "zp.inc"
        .include    "macros.inc"

; uint8_t network_open(const char* devicespec, uint8_t mode, uint8_t trans);
.proc _network_open
        pha                             ; save trans

        ; try and enable interrupts, this will not do anything if the interrupt is already setup, or we don't allow interrupts for reading.
        jsr     _network_read_interrupt_enable

        ldy     #$00
        sty     _fn_device_error

        setax   #t_network_open
        jsr     _copy_network_cmd_data

        jsr     popa                    ; mode
        sta     IO_DCB::daux1

        jsr     popax                   ; devicespec
        sta     IO_DCB::dbuflo
        stx     IO_DCB::dbufhi

        ; get the network unit for this device
        jsr     _network_unit
        sta     IO_DCB::dunit

        ; save mode in modes table with offset of unit
        ; this allows other users of this unit to see the mode set
        tax
        mva     IO_DCB::daux1, {fn_open_mode_table-1, x}

        pla                             ; restore trans
        sta     IO_DCB::daux2

        ; store the translation mode for this device
        sta     fn_open_trans_table-1, x

        jsr     _bus
        ; restore the unit for the status call
        lda     IO_DCB::dunit
        jsr     _bus_status

        ; we have an FN_ERR_ status in A, if it's OK we increment the open_devices
        tay                             ; save the error code
        cmp     #FN_ERR_OK
        bne     :+
        inc     _network_open_devices
:       
        tya     ; ensures Z flag has the correct value based on the error status in case we did an INC above
        rts

.endproc

.data

_network_open_devices:
        .byte 0

.rodata
t_network_open:
        .byte 'O', $80, 0, 1, $ff, $ff

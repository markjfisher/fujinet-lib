        .export _network_read_ih

        .import _network_read_trip
        .import _network_has_trip

_network_read_ih:
        ; lda     _network_has_trip
        ; beq     :+

        inc     _network_has_trip
        ; bne     out
        ; beq     out

; :
        lda     #$01            ; set trip value to 1 (true) to mark that we have available data
        sta     _network_read_trip
        ; sta     _network_has_trip

out:       
        pla
        rti

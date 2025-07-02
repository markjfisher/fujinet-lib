        .export _network_read_ih

        .import _network_read_trip

_network_read_ih:
        lda     #$01            ; set trip value to 1 (true) to mark that we have available data
        sta     _network_read_trip
        pla
        rti

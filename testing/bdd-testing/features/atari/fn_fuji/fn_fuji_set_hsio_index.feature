Feature: IO library test - fn_fuji_set_hsio_index

  This tests FN-FUJI fn_fuji_set_hsio_index

  Scenario: execute _fuji_set_hsio_index
    Given atari-fn-fuji application test setup
      And I add common atari-io files
      And I add atari src file "fn_fuji/fuji_set_hsio_index.s"
      And I add file for compiling "features/atari/fn_fuji/test-apps/test_bb.s"
      And I add file for compiling "features/atari/fn_fuji/stubs/bus-simple.s"
      And I create and load atari application
      And I write memory at $80 with $ff
      And I write memory at t_b1 with $33
      And I write memory at t_b2 with $66
      And I write word at t_fn with address _fuji_set_hsio_index
     When I execute the procedure at _init for no more than 120 instructions

    # check the DCB values were set correctly
    Then I expect to see DDEVIC equal $70
     And I expect to see DUNIT equal $01
     And I expect to see DCOMND equal $e3
     And I expect to see DSTATS equal 0
     And I expect to see DBUFLO equal 0
     And I expect to see DBUFHI equal 0
     And I expect to see DTIMLO equal $0f
     And I expect to see DBYTLO equal 0
     And I expect to see DBYTHI equal 0
     And I expect to see DAUX1 equal $66
     And I expect to see DAUX2 equal $33

    # check BUS was called
    Then I expect to see $80 equal $01


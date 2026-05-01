Feature: Smart card changer command api
The Web API is tested by sending commands and check the trajectory state

Scenario: Move from retracted to slot b
    Given the smart card changer is in retracted position
    When the smart card changer slot b is connected
    And wait for 20 seconds
    Then the smart chard changer is in connected position
    And  the smart chard changer is retracted


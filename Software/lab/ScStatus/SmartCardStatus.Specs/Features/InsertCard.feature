Feature: Insert Card
Example of a tested smart card reader application and automatic inserting and removing a smart card.

Scenario: Insert smart card in slot b and access it from the smart card status app
    Given the smart card status is not inserted
    And the smart card changer is in retracted position
    When the smart card changer slot b is connected
    And wait for 20 seconds
    Then the smart chard changer is in connected position
    And the smart card status is inserted
    And the smart chard changer is retracted

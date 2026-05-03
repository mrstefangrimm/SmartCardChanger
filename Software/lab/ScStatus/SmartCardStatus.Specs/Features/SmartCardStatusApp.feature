Feature: Smart Card Status App
Example of a test for the Windows application the check the smart card reader status

Scenario: Smart card is inserted manually within 20 seconds
    Given the smart card status is not inserted
    When wait for 20 seconds
    Then the smart card status is inserted

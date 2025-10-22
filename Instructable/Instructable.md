# Smart Card Changer

## Introduction
The Smart Card Changer enables remote access to up to five smart cards, such as access or bank cards, using a single smart card reader. Through a Bluetooth connection, you can conveniently insert or eject cards via your smartphone. The smart card changer is especially useful when the smart card reader is connected to a PC located in a different room.

The smart card changer has three components:

- The Mechanical Structure
- The Control Module
- The Overload Protection Module

The mechanical structure is constructed using basswood sheets and wooden cylinders. It uses two micro servo motors for a linear and a rotational motion.
The control module uses an Adafruit BLE feather board, which controls the two servo motors and is used for the Bluetooth connection with a smartphone.
The overload protection module monitors the electrical current drawn by the servo motors and automatically interrupts the circuit in the event of an overload.

Tooling
- A small lathe to cut the wooden cylinders to length
- Fretsaw or carpet knife to cut the basswood sheets
- Soldering station

The material costs are approximately $120.

## Supplies

- HID  Omnikey USB smart cards reader/writer, version 3021
- basswood sheets, 4 mm



## Step 1: Platform

![SCC-Assemble1-Base](.\SCC-Assemble1-Base.jpg)

![SCC-Assembled1-Base](.\SCC-Assembled1-Base.jpg)

The platform is the structural base of the carriage and the rotator, which are assembled in separate steps.

On the left side in the image are the rails for the carriage. The the right side are two cylinders on which the rotation platform is mounted.



Dimensions and Positions:

- 2 cylinders with the height of 50 mm. They are located 15 mm from the longitudinal side and 150 mm from the rear side
- 2 cylinders with the height of 40 mm. They are located 10mm from the longitudinal side and 28 mm from the rear side
- 4 cylinders with the height of 20 mm. They are located 10 mm from the longitudinal and 10 mm from the front sides
- A basswood piece 350 mm x 140 mm. The slot for the servo motor has a size of 25 mm x 12 mm and the distance to the rear side is 63 mm
- 4 basswood pieces 170 mm x 20 mm
- 2 basswood pieces 170 mm x 10 mm
- 4 felt mats with 20 mm diameter



## Step 2: Carriage and Servo arm

![SCC-Assemble2-Carriage](.\SCC-Assemble2-Carriage.jpg)

![SCC-Assembled2-Carriage](.\SCC-Assembled2-Carriage.jpg)

The carriage is the part that slides in the rails of the mechanical structure. In the center of the image is the servo arm extension and the right the servo arm. The servo arm is mounted onto the servo.



Dimensions and Positions:

- 2 cylinders with the height of 46 mm. They are located 10 mm and 55 mm from the front side
- 1 cylinder with the height of 9.7 mm. It is located 10 mm from the front side. The diameter of the hole in the center is 5.2 mm
- 3 cylinders with the the height of 3 mm. The diameter of the hole in the center is 5 mm
- 2 bolts with a 5 mm diameter and a length of 24 mm
- 1 basswood piece 119 mm x 95 mm. The hole for the spacer is locate 10 mm from the front side and has a diameter of 5 mm
- 1 basswood piece 175 mm x 15 mm. The holes are located 10 mm from the front sides with a diameter of 5.2 mm
- 1 basswood piece 70 mm x 15 mm. One hole is located 10 mm from the front side with a diameter of 5.2 mm. One hole is located 20 mm from the front side and has a diameter of 6 mm





## Step 3: Card Reader Platform

![SCC-Assemble3-ReaderPlatform](.\SCC-Assemble3-ReaderPlatform.jpg)

![SCC-Assembled3-ReaderPlatform](.\SCC-Assembled3-ReaderPlatform.jpg)

![SCC-Assembled3-ReaderPlatformWithReader](.\SCC-Assembled3-ReaderPlatformWithReader.jpg)

The reader platform is mounted onto the carriage. It holds the Omnikey Smart Card Reader. The black, soft material to fixate the reader is anti-static foam (the one that is used when you buy integrated circuits).



Dimensions and Positions:

- 1 basswood piece 120 mm x 90 mm
- 4 basswood pieces 30 mm x 15 mm. 2 pieces are located at the front side with a in-between distance of 64 mm.  2 pieces are located 38 mm from the front side in a 45 degree angle
-  2 basswood pieces 30 mm x 20 mm
- 4 pieces of anti-static foam



## Step 4: Rotator Platform

![SCC-Assemble4-RotationPlatform](.\SCC-Assemble4-RotationPlatform.jpg)

![SCC-Assembled4-RotatationPlatform](.\SCC-Assembled4-RotationPlatform.jpg)

The rotator platform is mounted onto the mechanical structure. It has a ramp on both sides to mechanically adjust the rotator.



Dimensions and Positions:

- 4 cylinders with the height of 8 mm. The cylinders are located 10 mm from the edge.
- 2 basswood pieces 58 mm x 20 mm
- 1 basswood piece 140 mm x 56 mm. The slot for the servo motor has a size of 25 mm x 12 mm and the distance to both the rear and the front side is 15.5 mm



## Step 5: Rotator

![SCC-Assemble5-Rotatator](.\SCC-Assemble5-Rotatator.jpg)

![SCC-Assembled5-Rotatator](.\SCC-Assembled5-Rotatator.jpg)

![SCC-Assembled5-Rotatator-MechanicalAdjustment](.\SCC-Assembled5-Rotatator-MechanicalAdjustment.jpg)

The rotator is mounted onto the rotator platform. The angle between the card slots is 30 degrees. The card slots are cut out of cardboard which is glued onto the rotator. The cards are fixated with hexagon screws.

The mechanical adjustment is part of this step. Use the 4 adjustment screws until each card slot has the same height as the card reader mouth.



Dimensions and Positions:

- 1 large basswood piece 300 mm x 215 mm. The rotation center is located 65 mm from the rear side. The radius for the card slots is 150 mm (215 - 65). The card fixation screws are located 35 mm from the front side for each card slot.
- 1 cardboard with the cut out card slots. The slots have a dimension of 54 mm x 30 mm



## Step 6: Rotator Lids

![SCC-Assemble6-RotatatorLid](.\SCC-Assemble6-RotatatorLid.jpg)

![SCC-Assemble6-RotatatorLid-Bottom](.\SCC-Assemble6-RotatatorLid-Bottom.jpg)

![SCC-Assembled6-RotatatorLid](.\SCC-Assembled6-RotatatorLid.jpg)

With the rotator lid, the cards are fixated to the rotator. Cardboard pieces are glued to the lids for a good grip.



Dimensions and Positions:

- 5 basswood pieces 60 mm x 79 mm. The angle of the side edges is 15 degrees towards the rotation center
- 5 cardboard pieces 46 mm x 15 mm
- 10 cardboard pieces 46 mm x 5 mm



## Step 7: Control Module

![SCC-Assemble7-ControlModule-Parts](.\SCC-Assemble7-ControlModule-Parts.jpg)

![SCC-Assemble7-ControlModule-Schema](.\SCC-Assemble7-ControlModule-Schema.jpg)

![SCC-Assembled7-ControlModule](.\SCC-Assembled7-ControlModule.jpg)

![SCC-Assembled7-ControlModuleWithFeather](.\SCC-Assembled7-ControlModuleWithFeather.jpg)


The smart card changer is already operational with the control module. In the next step, the control module is tested and the smart card changer is calibrated

Parts

 - 3 LEDs in different colors
 - 3 resitors 100 ohm
 - 2 male headers with 3 pins to connect the servo motors
 - 1 male header with 12 pins
 - 1 male header with 17 pins
 - 1 female header for 8 pins the overload protection module
 - 1 connector for 5V DC

## Step 8: Test and Calibrate


## Step 9: Overload Protection Module

![SCC-Assemble8-OverloadProtectionModule-Parts](.\SCC-Assemble8-OverloadProtectionModule-Parts.jpg)

![SCC-Assemble8-OverloadProtectionModule-Schema](.\SCC-Assemble8-OverloadProtectionModule-Schema.jpg)

![SCC-Assembled8-OverloadProtectionModule](.\SCC-Assembled8-OverloadProtectionModule.jpg)


The overload protection module is optional - smart card changer can be operated without it. Without the protection module, the smart cards could be damaged though.

Parts:

 - 1 step-up converter. This is required because the transistor requires 5 VDC and the Adafruit feather provides 3.3 VDC
 - 1 transitor IRF520 (or similar)
 - 2 ACS712 current measurment modules
 - 1 female header for 8 pins the overload protection module


## Step 10: Operate

That is the Smart Card Changer. In this last step we install the software and connect to it from a smart phone.




[![MSBuild and MSTest](https://github.com/mrstefangrimm/SmartCardChanger/actions/workflows/msbuild.yml/badge.svg)](https://github.com/mrstefangrimm/SmartCardChanger/actions/workflows/msbuild.yml)

# Smart Card Changer

The Smart Card Changer gives remote access to up to five smart cards, such as access or bank cards, using a single smart card reader. This repository contains the software, the hardware and the mechanical design files.

![Assembled Smart Card Changet](https://github.com/mrstefangrimm/SmartCardChanger/blob/main/Instructable/SCC-AssembledAndConnected.jpg)

## Getting Started

To use the software, you will have to build a device. How to build one is described in an [instructable](https://www.instructables.com/Smart-Card-Changer/).

## Examples


## Tests
Unit tests cover only basic functionality. The unit tests are part of the WinSccApp Visual Studio solution.


## Software Toolchain

**Built Applications**

| Application | License |
| ----------- | ------- |
| SccApp     | LGPL    |
| CalibrationApp     | LGPL    |
| WinSccApp  | LGPL    |

**Used Libraries**

| Library                           | Licence           |
| :-------------------------------- | ----------------- |
| prfServo                          | LGPL              |
| Adafruit MCP23017 Arduino Library | BSD               |
| Adafruit BluefruitLE nRF51        | BSD               |
| Arduino.h                         | LGPL              |
| TemplateStateMachine              | Apache License    |
| .NET 8                            | Microsoft license |

**Used Software Development Tools**

| Tool                         | License                                                      |
| ---------------------------- | ------------------------------------------------------------ |
| Arduino IDE                  | GPL                                                          |
| Adafruit AVR Boards [Learn more](https://learn.adafruit.com/adafruit-feather-32u4-bluefruit-le/setup) | Adafruit license |
| avrdude                      | GPL                                                          |
| gcc                          | GPL                                                          |
| Visual Studio Community 2022 | Microsoft license                                            |

## License

This is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

## Project History

The idea for the project came in mid 2025 and was first published on instructables.com in October 2025.

## Change History
See [github releases](https://github.com/mrstefangrimm/SmartCardChanger/releases).

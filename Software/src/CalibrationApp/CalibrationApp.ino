#include <Servo.h>
#include <EEPROM.h>
#include <float.h>

const uint8_t RTN_PIN = 12;
const uint8_t LNG_PIN = 11;
const uint8_t OVERLOAD_PIN = 9;
const uint8_t LED_GREEN = 5;
const uint8_t LED_YELLOW = 10;
const uint8_t LED_RED = 13;

Servo servoLng;
Servo servoRtn;


struct CalibrationData_V1 {
  float rtnParams[4] = { 0 };
  float rtnBacklashes[4] = { 0 };
  float lngParams[4] = { 0 };
  float lngBacklashes[4] = { 0 };
};

uint16_t currentLngPos = 900;
uint16_t currentRtnPos = 1400;

void setup() {
  Serial.begin(9600);

  servoLng.attach(LNG_PIN);
  servoLng.write(currentLngPos);

  servoRtn.attach(RTN_PIN);
  servoRtn.write(currentRtnPos);

  pinMode(OVERLOAD_PIN, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  while (!Serial) {
    ;
  }
}

enum State {
  Init,
  Calibrate,
  Write,
  Overload,
  Test
};
State state = State::Init;

enum CalibrationAxis {
  RTN,
  LNG
};
CalibrationAxis stateCalibrationAxis = CalibrationAxis::RTN;

void stateInit();
void stateCalibrate();
void stateWrite();
void stateOverload();
void stateTest();

void loop() {
  //Serial.println(state);
  switch (state) {
    case State::Init: stateInit(); break;
    case State::Calibrate: stateCalibrate(); break;
    case State::Write: stateWrite(); break;
    case State::Overload: stateOverload(); break;
    case State::Test: stateTest(); break;
  }
  delay(100);
}


bool validatate = true;
bool hasCafe = false;
bool eraseMemory = false;

void stateInit() {

  digitalWrite(OVERLOAD_PIN, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, HIGH);
  digitalWrite(LED_RED, LOW);

  if (validatate) {
    char cafe[5] = { 0 };
    cafe[0] = EEPROM.read(0);
    cafe[1] = EEPROM.read(1);
    cafe[2] = EEPROM.read(2);
    cafe[3] = EEPROM.read(3);
    Serial.println(cafe);
    hasCafe = strncmp("cafe", cafe, 4) == 0;

    if (hasCafe) {
      state = State::Calibrate;
      Serial.print(F("Position: "));
    } else {
      Serial.println(F("Erase memory? [y/n]"));
    }

    validatate = false;
  } else if (!hasCafe) {

    if (Serial.available()) {
      char yesNo = Serial.read();
      if (yesNo == 'y') {
        EEPROM.write(0, 'c');
        EEPROM.write(1, 'a');
        EEPROM.write(2, 'f');
        EEPROM.write(3, 'e');
        EEPROM.write(4, 0);

        state = State::Calibrate;
        Serial.print(F("Position: "));
      } else {
        Serial.println(F("Sorry, the answer has to be yes."));
      }
    }
  }
}

void stateCalibrate() {
  digitalWrite(OVERLOAD_PIN, HIGH);
  if (Serial.available()) {
    //char ch = Serial.read();
    String inputStr = Serial.readString();
    delay(20);
    Serial.print(F("*"));
    Serial.print(inputStr.c_str());
    Serial.println(F("*"));
    if (inputStr.c_str()[0] == 'p') {  // Power off
      Serial.println(F("Power off for 5 seconds"));
      digitalWrite(OVERLOAD_PIN, LOW);
      delay(5000);
      digitalWrite(OVERLOAD_PIN, HIGH);
      Serial.println(F("Power on"));
    } else if (inputStr.c_str()[0] == 'r') {
      Serial.println(F("rotation servo"));
      stateCalibrationAxis = CalibrationAxis::RTN;
    } else if (inputStr.c_str()[0] == 'l') {
      Serial.println(F("longitidinal servo"));
      stateCalibrationAxis = CalibrationAxis::LNG;
    } else if (inputStr.c_str()[0] == 'w') {
      state = State::Write;

      digitalWrite(OVERLOAD_PIN, LOW);
      Serial.println(F("Write EEPROM"));
      Serial.println(F("------------"));

      return;
    } else if (inputStr.c_str()[0] == 't') {
      state = State::Test;
      Serial.println();
      return;
    } else if (inputStr.c_str()[0] == 'o') {
      state = State::Overload;
      Serial.println();
      return;
    } else if (inputStr.c_str()[0] != "") {
      auto inputInt = inputStr.toInt();
      if (inputInt < 800 && inputInt > 2000) return;

      Serial.print(F("->"));




      if (stateCalibrationAxis == CalibrationAxis::LNG) {

        if (currentLngPos > inputInt) {
          Serial.print(inputInt + 30);
          servoLng.write(inputInt + 30);
          delay(500);
          Serial.print(F("->"));
          Serial.println(inputInt);
          servoLng.write(inputInt);
        } else {
          Serial.print(inputInt - 30);
          servoLng.write(inputInt - 30);
          delay(500);
          Serial.print(F("->"));
          Serial.println(inputInt);
          servoLng.write(inputInt);
        }

        currentLngPos = inputInt;

      } else {

        if (currentRtnPos > inputInt) {
          Serial.print(inputInt + 30);
          servoRtn.write(inputInt + 30);
          delay(500);
          Serial.print(F("->"));
          Serial.println(inputInt);
          servoRtn.write(inputInt);
        } else {
          Serial.print(inputInt - 30);
          servoRtn.write(inputInt - 30);
          delay(500);
          Serial.print(F("->"));
          Serial.println(inputInt);
          servoRtn.write(inputInt);
        }

        currentRtnPos = inputInt;
      }
    }

    Serial.print(F("Position: "));
  }
}

bool versionState = false;
CalibrationData_V1 writeState;
CalibrationData_V1 calibrationData;
void stateWrite() {

  if (calibrationData.lngParams[0] == 0 && writeState.lngParams[0] == 0) {
    writeState.lngParams[0] = 1;
    Serial.print(F("Lng param ord 0: "));
  } else if (calibrationData.lngParams[0] != 0 && writeState.lngParams[1] == 0) {
    writeState.lngParams[1] = 1;
    Serial.print(F("Lng param ord 1: "));
  } else if (calibrationData.lngParams[1] != 0 && writeState.lngParams[2] == 0) {
    writeState.lngParams[2] = 1;
    Serial.print(F("Lng param ord 2: "));
  } else if (calibrationData.lngParams[2] != 0 && writeState.lngParams[3] == 0) {
    writeState.lngParams[3] = 1;
    Serial.print(F("Lng param ord 3: "));
  } else if (calibrationData.lngParams[3] != 0 && writeState.rtnParams[0] == 0) {
    writeState.rtnParams[0] = 1;
    Serial.print(F("Rtn param ord 0: "));
  } else if (calibrationData.rtnParams[0] != 0 && writeState.rtnParams[1] == 0) {
    writeState.rtnParams[1] = 1;
    Serial.print(F("Rtn param ord 1: "));
  } else if (calibrationData.rtnParams[1] != 0 && writeState.rtnParams[2] == 0) {
    writeState.rtnParams[2] = 1;
    Serial.print(F("Rtn param ord 2: "));
  } else if (calibrationData.rtnParams[2] != 0 && writeState.rtnParams[3] == 0) {
    writeState.rtnParams[3] = 1;
    Serial.print(F("Rtn param ord 3: "));
  } else if (calibrationData.rtnParams[3] != 0 && writeState.rtnBacklashes[0] == 0) {
    writeState.rtnBacklashes[0] = 1;
    Serial.print(F("Rtn backl ord 0: "));
  } else if (calibrationData.rtnBacklashes[0] != 0 && writeState.rtnBacklashes[1] == 0) {
    writeState.rtnBacklashes[1] = 1;
    Serial.print(F("Rtn backl ord 1: "));
  } else if (calibrationData.rtnBacklashes[1] != 0 && writeState.rtnBacklashes[2] == 0) {
    writeState.rtnBacklashes[2] = 1;
    Serial.print(F("Rtn backl ord 2: "));
  } else if (calibrationData.rtnBacklashes[2] != 0.0 && versionState == false) {
    versionState = true;
    Serial.println(F("-------------------"));
    Serial.println(F("1"));
    Serial.println(calibrationData.lngParams[0], 6);
    Serial.println(calibrationData.lngParams[1], 6);
    Serial.println(calibrationData.lngParams[2], 6);
    Serial.println(calibrationData.lngParams[3], 6);
    Serial.println(calibrationData.lngBacklashes[0], 6);
    Serial.println(calibrationData.lngBacklashes[1], 6);
    Serial.println(calibrationData.lngBacklashes[2], 6);
    Serial.println(calibrationData.lngBacklashes[3], 6);

    Serial.println(calibrationData.rtnParams[0], 6);
    Serial.println(calibrationData.rtnParams[1], 6);
    Serial.println(calibrationData.rtnParams[2], 6);
    Serial.println(calibrationData.rtnParams[3], 6);
    Serial.println(calibrationData.rtnBacklashes[0], 6);
    Serial.println(calibrationData.rtnBacklashes[1], 6);
    Serial.println(calibrationData.rtnBacklashes[2], 6);
    Serial.println(calibrationData.rtnBacklashes[3], 6);
    Serial.println(F("-------------------"));
    Serial.println();
    Serial.print(F("Save? [y/n]"));
  }

  if (Serial.available()) {
    if (calibrationData.lngParams[0] == 0) {
      auto inputStr = Serial.readString();
      auto inputFloat = inputStr.toFloat();
      if (inputFloat == 0.0) inputFloat = FLT_MIN;
      Serial.println(inputFloat, 6);
      calibrationData.lngParams[0] = inputFloat;
    } else if (calibrationData.lngParams[1] == 0) {
      auto inputStr = Serial.readString();
      auto inputFloat = inputStr.toFloat();
      if (inputFloat == 0.0) inputFloat = FLT_MIN;
      Serial.println(inputFloat, 6);
      calibrationData.lngParams[1] = inputFloat;
    } else if (calibrationData.lngParams[2] == 0) {
      auto inputStr = Serial.readString();
      auto inputFloat = inputStr.toFloat();
      if (inputFloat == 0.0) inputFloat = FLT_MIN;
      Serial.println(inputFloat, 6);
      calibrationData.lngParams[2] = inputFloat;
    } else if (calibrationData.lngParams[3] == 0) {
      auto inputStr = Serial.readString();
      auto inputFloat = inputStr.toFloat();
      if (inputFloat == 0.0) inputFloat = FLT_MIN;
      Serial.println(inputFloat, 6);
      calibrationData.lngParams[3] = inputFloat;
    } else if (calibrationData.rtnParams[0] == 0) {
      auto inputStr = Serial.readString();
      auto inputFloat = inputStr.toFloat();
      if (inputFloat == 0.0) inputFloat = FLT_MIN;
      Serial.println(inputFloat, 6);
      calibrationData.rtnParams[0] = inputFloat;
    } else if (calibrationData.rtnParams[1] == 0) {
      auto inputStr = Serial.readString();
      auto inputFloat = inputStr.toFloat();
      if (inputFloat == 0.0) inputFloat = FLT_MIN;
      Serial.println(inputFloat, 6);
      calibrationData.rtnParams[1] = inputFloat;
    } else if (calibrationData.rtnParams[2] == 0) {
      auto inputStr = Serial.readString();
      auto inputFloat = inputStr.toFloat();
      if (inputFloat == 0.0) inputFloat = FLT_MIN;
      Serial.println(inputFloat, 6);
      calibrationData.rtnParams[2] = inputFloat;
    } else if (calibrationData.rtnParams[3] == 0) {
      auto inputStr = Serial.readString();
      auto inputFloat = inputStr.toFloat();
      if (inputFloat == 0.0) inputFloat = FLT_MIN;
      Serial.println(inputFloat, 6);
      calibrationData.rtnParams[3] = inputFloat;
    } else if (calibrationData.rtnBacklashes[0] == 0) {
      auto inputStr = Serial.readString();
      auto inputFloat = inputStr.toFloat();
      if (inputFloat == 0.0) inputFloat = FLT_MIN;
      Serial.println(inputFloat, 6);
      calibrationData.rtnBacklashes[0] = inputFloat;
    } else if (calibrationData.rtnBacklashes[1] == 0) {
      auto inputStr = Serial.readString();
      auto inputFloat = inputStr.toFloat();
      if (inputFloat == 0.0) inputFloat = FLT_MIN;
      Serial.println(inputFloat, 6);
      calibrationData.rtnBacklashes[1] = inputFloat;
    } else if (calibrationData.rtnBacklashes[2] == 0) {
      auto inputStr = Serial.readString();
      auto inputFloat = inputStr.toFloat();
      if (inputFloat == 0.0) inputFloat = FLT_MIN;
      Serial.println(inputFloat, 6);
      calibrationData.rtnBacklashes[2] = inputFloat;
    } else if (versionState) {
      auto yesNo = Serial.read();
      if (yesNo == 'y') {
        EEPROM.update(4, 1);
        EEPROM.put(5, calibrationData);
        Serial.println(F("Saved!"));
        state = State::Test;
      }
    }
  }
}


enum OverloadState {
  OInit,
  AtPosition,
  MovingLng,
  MovingRtn,
  Save
};
OverloadState overloadState = OverloadState::OInit;
float avarageRtnNoise = 0;
uint16_t overloadCounter = 0;

uint16_t atPositionLng[100];
uint16_t atPositionRtn[100];
uint16_t movingLngLng[120];
uint16_t movingLngRtn[120];
uint16_t movingRtnLng[120];
uint16_t movingRtnRtn[120];

void stateOverload() {
  //Serial.println("hello overload");
  /*
  Calc Mean At Position Lng
22:54:54.086 -> 764, 799, 799, 794, 799, 797, 798, 798, 800, 800, 802, 801, 799, 798, 800, 797, 800, 799, 800, 800, 797, 800, 799, 798, 800, 797, 801, 798, 796, 797, 798, 800, 798, 797, 801, 798, 797, 796, 795, 799, 798, 801, 797, 800, 798, 797, 798, 799, 798, 798, 801, 800, 798, 797, 799, 799, 800, 799, 798, 799, 797, 800, 798, 798, 802, 796, 799, 798, 796, 799, 799, 799, 802, 797, 794, 801, 797, 798, 801, 800, 797, 798, 799, 798, 796, 799, 798, 802, 796, 799, 798, 800, 798, 798, 801, 798, 798, 799, 800, 800, 
22:54:54.150 -> Mean 798.23
22:54:54.150 -> Calc Mean At Position Rtn
22:54:54.150 -> 756, 757, 758, 758, 759, 759, 761, 760, 761, 759, 761, 763, 763, 761, 759, 760, 762, 759, 759, 759, 758, 761, 762, 761, 762, 761, 760, 762, 763, 765, 764, 764, 763, 763, 764, 762, 760, 762, 761, 763, 765, 761, 762, 763, 762, 764, 763, 765, 765, 762, 765, 765, 766, 764, 764, 764, 764, 763, 762, 763, 763, 763, 764, 765, 764, 764, 763, 764, 766, 765, 763, 763, 762, 764, 760, 762, 764, 764, 763, 765, 763, 764, 763, 763, 764, 765, 765, 763, 764, 764, 764, 762, 765, 764, 765, 763, 761, 763, 763, 763, 
22:54:54.182 -> Mean 762.49
22:54:54.182 -> Calc Mean Moving Lng Lng
22:54:54.182 -> 800, 795, 754, 734, 793, 799, 794, 761, 731, 789, 771, 798, 772, 794, 770, 783, 791, 791, 795, 769, 782, 782, 798, 787, 776, 793, 776, 795, 765, 792, 785, 784, 767, 751, 797, 769, 796, 788, 799, 764, 768, 751, 788, 791, 788, 773, 795, 800, 781, 764, 756, 782, 792, 781, 751, 748, 796, 797, 798, 794, 785, 773, 797, 797, 798, 795, 790, 781, 785, 790, 781, 776, 750, 750, 795, 792, 779, 774, 777, 774, 792, 788, 776, 789, 766, 790, 792, 782, 775, 796, 795, 755, 745, 778, 791, 792, 794, 768, 798, 782, 783, 760, 796, 776, 765, 764, 792, 787, 787, 796, 788, 787, 761, 790, 791, 792, 783, 769, 797, 797, 
22:54:54.214 -> Mean 781.40
22:54:54.214 -> Calc Mean Moving Lng Rtn
22:54:54.214 -> 765, 776, 742, 736, 763, 773, 774, 750, 727, 751, 753, 763, 757, 766, 756, 756, 761, 765, 769, 765, 758, 767, 769, 763, 757, 768, 762, 768, 755, 765, 767, 762, 752, 737, 761, 748, 762, 761, 767, 750, 746, 738, 761, 765, 758, 758, 770, 772, 769, 749, 747, 754, 769, 752, 742, 729, 761, 768, 771, 772, 766, 747, 766, 774, 774, 773, 768, 754, 761, 760, 761, 754, 736, 731, 761, 766, 757, 759, 758, 759, 763, 766, 762, 760, 752, 763, 768, 759, 761, 769, 773, 744, 736, 749, 765, 765, 761, 756, 766, 756, 758, 750, 764, 759, 747, 744, 765, 760, 768, 769, 768, 758, 747, 763, 767, 767, 762, 759, 768, 775, 
22:54:54.278 -> Mean 759.29
22:54:54.278 -> Calc Mean Moving Rtn Lng
22:54:54.278 -> 797, 799, 798, 756, 718, 708, 694, 708, 719, 723, 723, 722, 726, 732, 736, 738, 746, 741, 754, 757, 793, 781, 796, 791, 794, 786, 794, 779, 769, 772, 785, 786, 758, 798, 794, 777, 730, 757, 709, 689, 728, 714, 741, 754, 722, 762, 753, 768, 764, 774, 776, 796, 788, 798, 792, 793, 781, 781, 786, 773, 786, 779, 794, 760, 776, 758, 768, 755, 768, 753, 780, 774, 771, 775, 740, 777, 776, 762, 783, 753, 769, 756, 788, 769, 767, 770, 745, 740, 758, 774, 786, 780, 768, 790, 799, 797, 782, 771, 713, 696, 702, 708, 744, 719, 724, 757, 750, 748, 744, 765, 752, 777, 787, 792, 779, 794, 786, 784, 792, 767, 
22:54:54.310 -> Mean 762.12
22:54:54.310 -> Calc Mean Moving Rtn Rtn
22:54:54.310 -> 765, 775, 768, 735, 708, 702, 683, 680, 692, 700, 706, 702, 706, 720, 717, 721, 719, 723, 726, 736, 757, 762, 767, 769, 770, 763, 771, 762, 757, 757, 768, 765, 743, 766, 771, 766, 725, 730, 703, 683, 705, 700, 710, 711, 717, 726, 730, 740, 740, 747, 758, 767, 767, 771, 769, 771, 766, 769, 762, 765, 754, 752, 758, 750, 752, 741, 745, 731, 749, 735, 746, 744, 738, 744, 729, 743, 740, 734, 747, 741, 743, 741, 755, 747, 750, 751, 738, 724, 738, 739, 754, 751, 753, 762, 772, 777, 762, 764, 713, 693, 687, 689, 696, 696, 699, 718, 731, 731, 727, 742, 726, 744, 754, 762, 763, 769, 763, 770, 763, 747, 
22:54:54.389 -> Mean 740.06
  */

  if (overloadState == OverloadState::OInit) {
    digitalWrite(OVERLOAD_PIN, HIGH);
    auto lngSensor = analogRead(A0);
    auto rtnSensor = analogRead(A1);
    Serial.print(F("Lng "));
    Serial.print(lngSensor);
    Serial.print(F(" Rtn "));
    Serial.println(rtnSensor);

    overloadCounter++;
    if (overloadCounter > 20) {
      overloadCounter = 0;

      digitalWrite(OVERLOAD_PIN, HIGH);
      Serial.println(F("AtPosition"));

      overloadState = OverloadState::AtPosition;
    }
  } else if (overloadState == OverloadState::AtPosition) {

    auto lngSensor = analogRead(A0);
    auto rtnSensor = analogRead(A1);
    Serial.print(F("Lng "));
    Serial.print(lngSensor);
    Serial.print(F(" Rtn "));
    Serial.println(rtnSensor);

    atPositionLng[overloadCounter] = lngSensor;
    atPositionRtn[overloadCounter] = rtnSensor;

    overloadCounter++;
    if (overloadCounter > 100) {
      overloadCounter = 0;

      Serial.println(F("MovingLng"));

      overloadState = OverloadState::MovingLng;
    }
  } else if (overloadState == OverloadState::MovingLng) {

    for (int a = 30; a < 90; a++) {
      servoLng.write(a);
      auto lngSensor = analogRead(A0);
      auto rtnSensor = analogRead(A1);
      Serial.print(F("Lng "));
      Serial.print(lngSensor);
      Serial.print(F(" Rtn "));
      Serial.println(rtnSensor);

      movingLngLng[overloadCounter] = lngSensor;
      movingLngRtn[overloadCounter] = rtnSensor;

      overloadCounter++;

      delay(10);
    }
    for (int a = 90; a > 30; a--) {
      servoLng.write(a);
      auto lngSensor = analogRead(A0);
      auto rtnSensor = analogRead(A1);
      Serial.print(F("Lng "));
      Serial.print(lngSensor);
      Serial.print(F(" Rtn "));
      Serial.println(rtnSensor);

      movingLngLng[overloadCounter] = lngSensor;
      movingLngRtn[overloadCounter] = rtnSensor;

      overloadCounter++;

      delay(10);
    }


    overloadCounter = 0;
    Serial.println(F("MovingRtn"));

    overloadState = OverloadState::MovingRtn;

  } else if (overloadState == OverloadState::MovingRtn) {

    for (int a = 90; a < 120; a++) {
      servoRtn.write(a);
      auto lngSensor = analogRead(A0);
      auto rtnSensor = analogRead(A1);
      Serial.print(F("Lng "));
      Serial.print(lngSensor);
      Serial.print(F(" Rtn "));
      Serial.println(rtnSensor);

      movingRtnLng[overloadCounter] = lngSensor;
      movingRtnRtn[overloadCounter] = rtnSensor;

      overloadCounter++;

      delay(10);
    }
    for (int a = 120; a > 60; a--) {
      servoRtn.write(a);
      auto lngSensor = analogRead(A0);
      auto rtnSensor = analogRead(A1);
      Serial.print(F("Lng "));
      Serial.print(lngSensor);
      Serial.print(F(" Rtn "));
      Serial.println(rtnSensor);

      movingRtnLng[overloadCounter] = lngSensor;
      movingRtnRtn[overloadCounter] = rtnSensor;

      overloadCounter++;

      delay(10);
    }
    for (int a = 60; a < 90; a++) {
      servoRtn.write(a);
      auto lngSensor = analogRead(A0);
      auto rtnSensor = analogRead(A1);
      Serial.print(F("Lng "));
      Serial.print(lngSensor);
      Serial.print(F(" Rtn "));
      Serial.println(rtnSensor);

      movingRtnLng[overloadCounter] = lngSensor;
      movingRtnRtn[overloadCounter] = rtnSensor;

      overloadCounter++;

      delay(10);
    }


    overloadCounter = 0;
    Serial.println(F("Save"));

    overloadState = OverloadState::Save;
  } else if (overloadState == OverloadState::Save) {

    Serial.println(F("Calc Mean At Position Lng"));
    double meanAtPosLng = 0;
    for (int i = 0; i < 100; i++) {
      Serial.print(atPositionLng[i]);
      Serial.print(F(", "));
      meanAtPosLng += atPositionLng[i] / 100.0;
    }
    Serial.println();
    Serial.print(F("Mean "));
    Serial.println(meanAtPosLng);

    Serial.println(F("Calc Mean At Position Rtn"));
    double meanAtPosRtn = 0;
    for (int i = 0; i < 100; i++) {
      Serial.print(atPositionRtn[i]);
      Serial.print(F(", "));
      meanAtPosRtn += atPositionRtn[i] / 100.0;
    }
    Serial.println();
    Serial.print(F("Mean "));
    Serial.println(meanAtPosRtn);

    Serial.println(F("Calc Mean Moving Lng Lng"));
    double meanMovingLngLng = 0;
    for (int i = 0; i < 120; i++) {
      Serial.print(movingLngLng[i]);
      Serial.print(F(", "));
      meanMovingLngLng += movingLngLng[i] / 120.0;
    }
    Serial.println();
    Serial.print(F("Mean "));
    Serial.println(meanMovingLngLng);

    Serial.println(F("Calc Mean Moving Lng Rtn"));
    double meanMovingLngRtn = 0;
    for (int i = 0; i < 120; i++) {
      Serial.print(movingLngRtn[i]);
      Serial.print(F(", "));
      meanMovingLngRtn += movingLngRtn[i] / 120.0;
    }
    Serial.println();
    Serial.print(F("Mean "));
    Serial.println(meanMovingLngRtn);

    Serial.println(F("Calc Mean Moving Rtn Lng"));
    double meanMovingRtnLng = 0;
    for (int i = 0; i < 120; i++) {
      Serial.print(movingRtnLng[i]);
      Serial.print(F(", "));
      meanMovingRtnLng += movingRtnLng[i] / 120.0;
    }
    Serial.println();
    Serial.print(F("Mean "));
    Serial.println(meanMovingRtnLng);

    Serial.println(F("Calc Mean Moving Rtn Rtn"));
    double meanMovingRtnRtn = 0;
    for (int i = 0; i < 120; i++) {
      Serial.print(movingRtnRtn[i]);
      Serial.print(F(", "));
      meanMovingRtnRtn += movingRtnRtn[i] / 120.0;
    }
    Serial.println();
    Serial.print(F("Mean "));
    Serial.println(meanMovingRtnRtn);

    state = State::Init;
  }
}

void stateTest() {
  auto version = EEPROM.read(4);
  Serial.print(F("Version "));
  Serial.println(version == 1);
  CalibrationData_V1 calibrationData;
  EEPROM.get(5, calibrationData);
  Serial.println(F("Lng params"));
  Serial.println(calibrationData.lngParams[0]);
  Serial.println(calibrationData.lngParams[1]);
  Serial.println(calibrationData.lngParams[2]);
  Serial.println(calibrationData.lngParams[3]);
  Serial.println(F("Lng backlashes"));
  Serial.println(calibrationData.lngBacklashes[0]);
  Serial.println(calibrationData.lngBacklashes[1]);
  Serial.println(calibrationData.lngBacklashes[2]);
  Serial.println(calibrationData.lngBacklashes[3]);
  Serial.println(F("Rtn params"));
  Serial.println(calibrationData.rtnParams[0]);
  Serial.println(calibrationData.rtnParams[1]);
  Serial.println(calibrationData.rtnParams[2]);
  Serial.println(calibrationData.rtnParams[3]);
  Serial.println(F("Lng backlashes"));
  Serial.println(calibrationData.rtnBacklashes[0]);
  Serial.println(calibrationData.rtnBacklashes[1]);
  Serial.println(calibrationData.rtnBacklashes[2]);
  Serial.println(calibrationData.rtnBacklashes[3]);

  state = State::Init;
}

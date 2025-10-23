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
  float rtnCoefficients[4] = { 0 };
  float rtnBacklashCoefficients[4] = { 0 };
  float lngCoefficients[4] = { 0 };
  float lngBacklashCoefficients[4] = { 0 };

  uint16_t rtnOverflowLowerThreshold = 0;
  uint16_t rtnOverflowUpperThreshold = 0;
  uint16_t lngOverflowLowerThreshold = 0;
  uint16_t lngOverflowUpperThreshold = 0;
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
    Serial.print(inputStr.c_str());
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
      Serial.println(F("longitudinal servo"));
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

  if (calibrationData.lngCoefficients[0] == 0 && writeState.lngCoefficients[0] == 0) {
    writeState.lngCoefficients[0] = 1;
    Serial.print(F("Lng param ord 0: "));
  } else if (calibrationData.lngCoefficients[0] != 0 && writeState.lngCoefficients[1] == 0) {
    writeState.lngCoefficients[1] = 1;
    Serial.print(F("Lng param ord 1: "));
  } else if (calibrationData.lngCoefficients[1] != 0 && writeState.lngCoefficients[2] == 0) {
    writeState.lngCoefficients[2] = 1;
    Serial.print(F("Lng param ord 2: "));
  } else if (calibrationData.lngCoefficients[2] != 0 && writeState.lngCoefficients[3] == 0) {
    writeState.lngCoefficients[3] = 1;
    Serial.print(F("Lng param ord 3: "));
  } else if (calibrationData.lngCoefficients[3] != 0 && writeState.rtnCoefficients[0] == 0) {
    writeState.rtnCoefficients[0] = 1;
    Serial.print(F("Rtn param ord 0: "));
  } else if (calibrationData.rtnCoefficients[0] != 0 && writeState.rtnCoefficients[1] == 0) {
    writeState.rtnCoefficients[1] = 1;
    Serial.print(F("Rtn param ord 1: "));
  } else if (calibrationData.rtnCoefficients[1] != 0 && writeState.rtnCoefficients[2] == 0) {
    writeState.rtnCoefficients[2] = 1;
    Serial.print(F("Rtn param ord 2: "));
  } else if (calibrationData.rtnCoefficients[2] != 0 && writeState.rtnCoefficients[3] == 0) {
    writeState.rtnCoefficients[3] = 1;
    Serial.print(F("Rtn param ord 3: "));
  } else if (calibrationData.rtnCoefficients[3] != 0 && writeState.rtnBacklashCoefficients[0] == 0) {
    writeState.rtnBacklashCoefficients[0] = 1;
    Serial.print(F("Rtn backl ord 0: "));
  } else if (calibrationData.rtnBacklashCoefficients[0] != 0 && writeState.rtnBacklashCoefficients[1] == 0) {
    writeState.rtnBacklashCoefficients[1] = 1;
    Serial.print(F("Rtn backl ord 1: "));
  } else if (calibrationData.rtnBacklashCoefficients[1] != 0 && writeState.rtnBacklashCoefficients[2] == 0) {
    writeState.rtnBacklashCoefficients[2] = 1;
    Serial.print(F("Rtn backl ord 2: "));
  } else if (calibrationData.rtnBacklashCoefficients[2] != 0.0 && versionState == false) {
    versionState = true;
    Serial.println(F("-------------------"));
    Serial.println(F("1"));
    Serial.println(calibrationData.lngCoefficients[0], 6);
    Serial.println(calibrationData.lngCoefficients[1], 6);
    Serial.println(calibrationData.lngCoefficients[2], 6);
    Serial.println(calibrationData.lngCoefficients[3], 6);
    Serial.println(calibrationData.lngBacklashCoefficients[0], 6);
    Serial.println(calibrationData.lngBacklashCoefficients[1], 6);
    Serial.println(calibrationData.lngBacklashCoefficients[2], 6);
    Serial.println(calibrationData.lngBacklashCoefficients[3], 6);

    Serial.println(calibrationData.rtnCoefficients[0], 6);
    Serial.println(calibrationData.rtnCoefficients[1], 6);
    Serial.println(calibrationData.rtnCoefficients[2], 6);
    Serial.println(calibrationData.rtnCoefficients[3], 6);
    Serial.println(calibrationData.rtnBacklashCoefficients[0], 6);
    Serial.println(calibrationData.rtnBacklashCoefficients[1], 6);
    Serial.println(calibrationData.rtnBacklashCoefficients[2], 6);
    Serial.println(calibrationData.rtnBacklashCoefficients[3], 6);
    Serial.println(F("-------------------"));
    Serial.println();
    Serial.print(F("Save? [y/n]"));
  }

  if (Serial.available()) {
    if (calibrationData.lngCoefficients[0] == 0) {
      auto inputStr = Serial.readString();
      auto inputFloat = inputStr.toFloat();
      if (inputFloat == 0.0) inputFloat = FLT_MIN;
      Serial.println(inputFloat, 6);
      calibrationData.lngCoefficients[0] = inputFloat;
    } else if (calibrationData.lngCoefficients[1] == 0) {
      auto inputStr = Serial.readString();
      auto inputFloat = inputStr.toFloat();
      if (inputFloat == 0.0) inputFloat = FLT_MIN;
      Serial.println(inputFloat, 6);
      calibrationData.lngCoefficients[1] = inputFloat;
    } else if (calibrationData.lngCoefficients[2] == 0) {
      auto inputStr = Serial.readString();
      auto inputFloat = inputStr.toFloat();
      if (inputFloat == 0.0) inputFloat = FLT_MIN;
      Serial.println(inputFloat, 6);
      calibrationData.lngCoefficients[2] = inputFloat;
    } else if (calibrationData.lngCoefficients[3] == 0) {
      auto inputStr = Serial.readString();
      auto inputFloat = inputStr.toFloat();
      if (inputFloat == 0.0) inputFloat = FLT_MIN;
      Serial.println(inputFloat, 6);
      calibrationData.lngCoefficients[3] = inputFloat;
    } else if (calibrationData.rtnCoefficients[0] == 0) {
      auto inputStr = Serial.readString();
      auto inputFloat = inputStr.toFloat();
      if (inputFloat == 0.0) inputFloat = FLT_MIN;
      Serial.println(inputFloat, 6);
      calibrationData.rtnCoefficients[0] = inputFloat;
    } else if (calibrationData.rtnCoefficients[1] == 0) {
      auto inputStr = Serial.readString();
      auto inputFloat = inputStr.toFloat();
      if (inputFloat == 0.0) inputFloat = FLT_MIN;
      Serial.println(inputFloat, 6);
      calibrationData.rtnCoefficients[1] = inputFloat;
    } else if (calibrationData.rtnCoefficients[2] == 0) {
      auto inputStr = Serial.readString();
      auto inputFloat = inputStr.toFloat();
      if (inputFloat == 0.0) inputFloat = FLT_MIN;
      Serial.println(inputFloat, 6);
      calibrationData.rtnCoefficients[2] = inputFloat;
    } else if (calibrationData.rtnCoefficients[3] == 0) {
      auto inputStr = Serial.readString();
      auto inputFloat = inputStr.toFloat();
      if (inputFloat == 0.0) inputFloat = FLT_MIN;
      Serial.println(inputFloat, 6);
      calibrationData.rtnCoefficients[3] = inputFloat;
    } else if (calibrationData.rtnBacklashCoefficients[0] == 0) {
      auto inputStr = Serial.readString();
      auto inputFloat = inputStr.toFloat();
      if (inputFloat == 0.0) inputFloat = FLT_MIN;
      Serial.println(inputFloat, 6);
      calibrationData.rtnBacklashCoefficients[0] = inputFloat;
    } else if (calibrationData.rtnBacklashCoefficients[1] == 0) {
      auto inputStr = Serial.readString();
      auto inputFloat = inputStr.toFloat();
      if (inputFloat == 0.0) inputFloat = FLT_MIN;
      Serial.println(inputFloat, 6);
      calibrationData.rtnBacklashCoefficients[1] = inputFloat;
    } else if (calibrationData.rtnBacklashCoefficients[2] == 0) {
      auto inputStr = Serial.readString();
      auto inputFloat = inputStr.toFloat();
      if (inputFloat == 0.0) inputFloat = FLT_MIN;
      Serial.println(inputFloat, 6);
      calibrationData.rtnBacklashCoefficients[2] = inputFloat;
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
  Serial.println(calibrationData.lngCoefficients[0]);
  Serial.println(calibrationData.lngCoefficients[1]);
  Serial.println(calibrationData.lngCoefficients[2]);
  Serial.println(calibrationData.lngCoefficients[3]);
  Serial.println(F("Lng backlashes"));
  Serial.println(calibrationData.lngBacklashCoefficients[0]);
  Serial.println(calibrationData.lngBacklashCoefficients[1]);
  Serial.println(calibrationData.lngBacklashCoefficients[2]);
  Serial.println(calibrationData.lngBacklashCoefficients[3]);
  Serial.println(F("Rtn params"));
  Serial.println(calibrationData.rtnCoefficients[0]);
  Serial.println(calibrationData.rtnCoefficients[1]);
  Serial.println(calibrationData.rtnCoefficients[2]);
  Serial.println(calibrationData.rtnCoefficients[3]);
  Serial.println(F("Lng backlashes"));
  Serial.println(calibrationData.rtnBacklashCoefficients[0]);
  Serial.println(calibrationData.rtnBacklashCoefficients[1]);
  Serial.println(calibrationData.rtnBacklashCoefficients[2]);
  Serial.println(calibrationData.rtnBacklashCoefficients[3]);

  state = State::Init;
}

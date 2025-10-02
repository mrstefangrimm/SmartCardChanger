#ifndef prfServoImpl_h
#define prfServoImpl_h

#include <stdlib.h>
#include "prfServo2.h"
#include "../Shared/Messages.h"
#include "../Shared/BitCounter.h"
#include "../Shared/ConfigurationData.h"
#include "../Shared/ResourceStrings.h"

namespace nspls {

const uint8_t RTN_PIN = 12;
const uint8_t LNG_PIN = 11;
PRSDEFI(RSAO, "prfServoImpl");


template<class TLog, class TServo, class TEeprom>
class prfServoImpl : public prfServo2ImplBase<uint16_t> {
public:
  explicit prfServoImpl(TEeprom& eeprom)
    : _eeprom(eeprom) {
  }

  void begin() override {
    pinMode(RTN_PIN, OUTPUT);
    pinMode(LNG_PIN, OUTPUT);
  }

  void attachRtn() {
    _servoRtn.attach(RTN_PIN);
    _servoRtn.write(90);  // go to mid position of physical rotation axis
  }

  void attachLng() {
    _servoLng.attach(LNG_PIN);
    _servoLng.write(30);  // go to "zero" position of the physical rotation axis
  }

  void get(float** coefficients, float** backlashCoefficients) const final {

    char version = _eeprom.read(4);
    _log->print(F("Calibration Data Version "));
    _log->println(version == 1);
    if (version == 1) {
      CalibrationData_V1 calibrationData;

      _eeprom.get(5, calibrationData);

      coefficients[0][0] = calibrationData.rtnCoefficients[0];
      coefficients[0][1] = calibrationData.rtnCoefficients[1];
      coefficients[0][2] = calibrationData.rtnCoefficients[2];
      coefficients[0][3] = calibrationData.rtnCoefficients[3];

      backlashCoefficients[0][0] = calibrationData.rtnBacklashCoefficients[0];
      backlashCoefficients[0][1] = calibrationData.rtnBacklashCoefficients[1];
      backlashCoefficients[0][2] = calibrationData.rtnBacklashCoefficients[2];

      coefficients[1][0] = calibrationData.lngCoefficients[0];
      coefficients[1][1] = calibrationData.lngCoefficients[1];
      coefficients[1][2] = calibrationData.lngCoefficients[2];
      coefficients[1][3] = calibrationData.lngCoefficients[3];
      backlashCoefficients[1][0] = calibrationData.lngBacklashCoefficients[0];

      _log->println(F("Lng coefficients"));
      _log->println(calibrationData.lngCoefficients[0], 6);
      _log->println(calibrationData.lngCoefficients[1], 6);
      _log->println(calibrationData.lngCoefficients[2], 6);
      _log->println(calibrationData.lngCoefficients[3], 6);
      _log->println(F("Lng backlash coefficients"));
      _log->println(calibrationData.lngBacklashCoefficients[0], 6);
      _log->println(calibrationData.lngBacklashCoefficients[1], 6);
      _log->println(calibrationData.lngBacklashCoefficients[2], 6);
      _log->println(calibrationData.lngBacklashCoefficients[3], 6);
      _log->println(F("Rtn coefficients"));
      _log->println(calibrationData.rtnCoefficients[0], 6);
      _log->println(calibrationData.rtnCoefficients[1], 6);
      _log->println(calibrationData.rtnCoefficients[2], 6);
      _log->println(calibrationData.rtnCoefficients[3], 6);
      _log->println(F("Lng backlash coefficients"));
      _log->println(calibrationData.rtnBacklashCoefficients[0], 6);
      _log->println(calibrationData.rtnBacklashCoefficients[1], 6);
      _log->println(calibrationData.rtnBacklashCoefficients[2], 6);
      _log->println(calibrationData.rtnBacklashCoefficients[3], 6);
      _log->println(F("-------------------"));
    } else {

      rs_println(RSAO, RSSTARTUP, RSUSEHARDCODEDVALUES);

      // https://arachnoid.com/polysolve/

      // RTN, 19.Aug.2025
      // 30 1105
      // 60 1270
      // 90 1435
      // 120 1602
      // 150 1765
      coefficients[0][0] = 9.4440000000000009e+002f;
      coefficients[0][1] = 5.2730158730158729e+000f;
      coefficients[0][2] = 3.1746031746031746e-003f;
      coefficients[0][3] = -1.2345679012345678e-005f;

      // 30 1085 -25
      // 60 1245 -25
      // 90 1410 -25
      // 120 1585 -17
      // 150 1745 -20
      backlashCoefficients[0][0] = -2.6799999999999997e+001f;
      backlashCoefficients[0][1] = 3.1428571428571431e-002f;
      backlashCoefficients[0][2] = 1.5873015873015873e-004f;

      // LNG, 16.Aug.2025
      // 0 950
      // 10 1090
      // 20 1180
      // 40 1330
      // 60 1505
      // 70 1630
      coefficients[1][0] = 9.5199345532256200e+002f;
      coefficients[1][1] = 1.5448059970614935e+001f;
      coefficients[1][2] = -2.3899425671160265e-001f;
      coefficients[1][3] = 2.2417189795642862e-003f;
      backlashCoefficients[1][0] = 0;
    }
  }

  void write(uint8_t num, uint16_t servoVal) {
    //_logger.print(num);
    //_logger.print(F(" "));
    //_logger.println(servoVal);
    if (num == 0) {
      _servoRtn.write(servoVal);
    } else if (num == 1) {
      _servoLng.write(servoVal);
    }
  }

private:
  typename TLog::Type* _log = TLog::create();
  TEeprom& _eeprom;

  TServo _servoRtn;
  TServo _servoLng;
};

}

#endif

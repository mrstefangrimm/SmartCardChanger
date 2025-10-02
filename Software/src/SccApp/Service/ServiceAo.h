#ifndef Service_h
#define Service_h

#include "../Shared/Messages.h"
#include "../Shared/ConfigurationData.h"

namespace nssvc {

#define DBG(x) x

PRSDEFI(RSEEPROMBEFORE, "EEPROM Before: ");
PRSDEFI(RSEEPROMAFTER, "EEPROM After:  ");

namespace event {
struct Timeout {
  MessageData_t msg;
};
}

const uint8_t RTN_PIN = 12;
const uint8_t LNG_PIN = 11;

template<class TLog, class TServo, class TEeprom>
class ServiceAo {

public:
  explicit ServiceAo(Messages& messages, TEeprom& eeprom)
    : _messages(messages), _eeprom(eeprom) {
    //_statemachine.begin();
  }

  void load() {
    _inputMsg = _messages.toServiceQueue.pop();
  }

  void run() {
    if (_inputMsg != 0) {
      ServiceCmdData msg(_inputMsg);
      CalibrationData_V1 calibrationData;

      switch ((ServiceCmdInfo)msg.info) {

      default: return;

      case ServiceCmdInfo::Enter:
        //_servoRtn.attach(RTN_PIN);
        //_servoLng.attach(LNG_PIN);
        break;

      case ServiceCmdInfo::Quit:
        //_servoRtn.detach();
        //_servoLng.detach();

        _messages.toPulsWidthTransmitterQueue.push(TargetPosData(true).raw);
        break;

      case ServiceCmdInfo::Print:
        _eeprom.get(5, calibrationData);
        _log->println(F("Rtn coefficents"));
        _log->println(calibrationData.rtnCoefficients[0], 3);
        _log->println(calibrationData.rtnCoefficients[1], 3);
        _log->println(calibrationData.rtnCoefficients[2], 3);
        _log->println(calibrationData.rtnCoefficients[3], 3);
        _log->println(F("Rtn backlash coefficents"));
        _log->println(calibrationData.rtnBacklashCoefficients[0], 3);
        _log->println(calibrationData.rtnBacklashCoefficients[1], 3);
        _log->println(calibrationData.rtnBacklashCoefficients[2], 3);
        _log->println(calibrationData.rtnBacklashCoefficients[3], 3);
        _log->println(F("Lng coefficents"));
        _log->println(calibrationData.lngCoefficients[0], 3);
        _log->println(calibrationData.lngCoefficients[1], 3);
        _log->println(calibrationData.lngCoefficients[2], 3);
        _log->println(calibrationData.lngCoefficients[3], 3);
        _log->println(F("Lng backlash coefficents"));
        _log->println(calibrationData.lngBacklashCoefficients[0], 3);
        _log->println(calibrationData.lngBacklashCoefficients[1], 3);
        _log->println(calibrationData.lngBacklashCoefficients[2], 3);
        _log->println(calibrationData.lngBacklashCoefficients[3], 3);
        _log->println(F("Rtn lower/upper thresholds"));
        _log->println(calibrationData.rtnOverflowLowerThreshold);
        _log->println(calibrationData.rtnOverflowUpperThreshold);
        _log->println(F("Lng lower/upper thresholds"));
        _log->println(calibrationData.lngOverflowLowerThreshold);
        _log->println(calibrationData.lngOverflowUpperThreshold);
        break;

      case ServiceCmdInfo::AdjustRtnServoOffset:
        _eeprom.get(5, calibrationData);

        DBG({
          rs_print(RSEEPROMBEFORE);
          _log->println(calibrationData.rtnCoefficients[0]);
          });
        calibrationData.rtnCoefficients[0] += (int16_t)msg.data;
        DBG({
          rs_print(RSEEPROMAFTER);
          _log->println(calibrationData.rtnCoefficients[0]);
          });
        _eeprom.put(5, calibrationData);

      case ServiceCmdInfo::AdjustLngServoOffset:
        _eeprom.get(5, calibrationData);

        DBG({
          rs_print(RSEEPROMBEFORE);
          _log->println(calibrationData.lngCoefficients[0]);
          });
        calibrationData.lngCoefficients[0] += (int16_t)msg.data;
        DBG({
          rs_print(RSEEPROMAFTER);
          _log->println(calibrationData.lngCoefficients[0]);
          });
        _eeprom.put(5, calibrationData);

      case ServiceCmdInfo::AdjustRtnOverloadLower:
        _eeprom.get(5, calibrationData);

        DBG({
          rs_print(RSEEPROMBEFORE);
          _log->println(calibrationData.rtnOverflowLowerThreshold);
          });
        calibrationData.rtnOverflowLowerThreshold += (int16_t)msg.data;
        DBG({
          rs_print(RSEEPROMAFTER);
          _log->println(calibrationData.rtnOverflowLowerThreshold);
          });
        _eeprom.put(5, calibrationData);

      case ServiceCmdInfo::AdjustRtnOverloadUpper:
        _eeprom.get(5, calibrationData);

        DBG({
          rs_print(RSEEPROMBEFORE);
          _log->println(calibrationData.rtnOverflowUpperThreshold);
          });
        calibrationData.rtnOverflowUpperThreshold += (int16_t)msg.data;
        DBG({
          rs_print(RSEEPROMAFTER);
          _log->println(calibrationData.rtnOverflowUpperThreshold);
          });
        _eeprom.put(5, calibrationData);

      case ServiceCmdInfo::AdjustLngOverloadLower:
        _eeprom.get(5, calibrationData);

        DBG({
          rs_print(RSEEPROMBEFORE);
          _log->println(calibrationData.lngOverflowLowerThreshold);
          });
        calibrationData.lngOverflowLowerThreshold += (int16_t)msg.data;
        DBG({
          rs_print(RSEEPROMAFTER);
          _log->println(calibrationData.lngOverflowLowerThreshold);
          });
        _eeprom.put(5, calibrationData);

      case ServiceCmdInfo::AdjustLngOverloadUpper:
        _eeprom.get(5, calibrationData);

        DBG({
          rs_print(RSEEPROMBEFORE);
          _log->println(calibrationData.lngOverflowUpperThreshold);
          });
        calibrationData.lngOverflowUpperThreshold += (int16_t)msg.data;
        DBG({
          rs_print(RSEEPROMAFTER);
          _log->println(calibrationData.lngOverflowUpperThreshold);
          });
        _eeprom.put(5, calibrationData);
      }
    }

    //_statemachine.dispatch(event::Timeout{ _inputMsg });
    _inputMsg = 0;
  }

private:
  typename TLog::Type* _log = TLog::create();
  Messages& _messages;
  MessageData_t _inputMsg = 0;

  TEeprom& _eeprom;
  //TServo _servoRtn;
  //TServo _servoLng;
};
}

#endif
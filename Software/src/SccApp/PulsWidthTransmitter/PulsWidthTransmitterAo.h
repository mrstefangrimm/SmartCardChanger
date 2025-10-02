#ifndef PulsWidthTransmitterAo_h
#define PulsWidthTransmitterAo_h

#include "prfServoImpl.h"
#include "../Shared/Messages.h"
#include "../Shared/BitCounter.h"

namespace nspls {

template<class TLog, class TServo, class TEeprom>
class PulsWidthTransmitterAo {
public:
  PulsWidthTransmitterAo(Messages& messages, TEeprom& eeprom)
    : _messages(messages), _eeprom(eeprom) {
    _impl = new prfServoImpl<TLog, TServo, TEeprom>(eeprom);
    _servos = new prfServo2<uint8_t, float, uint16_t>(_impl, 0b00001111, 0b00000010); //lng order:3, rtn order:3, lng backl order:0, rtn backl order:2
  }

  void load() {
    _inputMsg = _messages.toPulsWidthTransmitterQueue.pop();
  }

  void run() {
    switch (_state) {
    case State::Init: stateInit(); break;
    case State::Ready: stateReady(); break;
    };
    _inputMsg = 0;
  }

private:
  void stateInit() {
    if (_stateInitCounter == 1) {

      // Check magic number on EEPROM

      char cafe[4];
      cafe[0] = _eeprom.read(0);
      cafe[1] = _eeprom.read(1);
      cafe[2] = _eeprom.read(2);
      cafe[3] = _eeprom.read(3);

      if (strncmp("cafe", cafe, 4) != 0) {
        _messages.toSystemMonitorQueue.push(SystemStatusData(SystemStatusInfo::NeedsCalibration).raw);
        // TODO: _state = State::Error;
      }

      _servos->begin();

      _log->println(F("attachLng"));
      _impl->attachLng();
      //_servos->write(1, 30);
      // _targetLng = 30;
      // _currentLng = 30;
    } else if (_stateInitCounter == 20) {
      _log->println(F("attachRtn"));
      _impl->attachRtn();
      _state = State::Ready;
    }
    _stateInitCounter++;
  }

  void stateReady() {
    //_log->println(_impl->calibrationData.lngParams[0]);

   //_log->println(F("stateReady"));
    if (_inputMsg != 0) {
      TargetPosData target(_inputMsg);

      if (target.reset)
      {
        _stateInitCounter = 0;
        _state = State::Init;
        return;
      }

      _log->print(F("target Servo "));
      _log->print((int)target.servoNo);
      _log->print(F(" target Pos "));
      _log->println((int)target.position / 100.0f);

      if (target.servoNo == 0) {
        _targetRtn = target.position / 100.0f;
        _stepSizeRtn = target.stepSize / 10.0f;
      } else if (target.servoNo == 1) {
        _targetLng = target.position / 100.0f;
        _stepSizeLng = target.stepSize / 10.0f;
      }
    }

    if (abs(_targetRtn - _currentRtn) > _stepSizeRtn) {
      _currentRtn += (_targetRtn > _currentRtn) ? _stepSizeRtn : -_stepSizeRtn;
      _servos->write(0, _currentRtn);
      //_log->println(F("rtn moving"));
    } else if (abs(_targetRtn - _currentRtn) > _epsilon) {
      // current rtn in range [stepSize, epsilon]. Move servo to the exact target.
      _currentRtn = _targetRtn;
      _servos->write(0, _currentRtn);
    }

    if (abs(_targetLng - _currentLng) > _stepSizeLng) {
      _currentLng += (_targetLng > _currentLng) ? _stepSizeLng : -_stepSizeLng;
      _servos->write(1, _currentLng);
      //_log->print(F("lng moving "));
      //_log->println(_currentLng);
    } else if (abs(_targetLng - _currentLng) > _epsilon) {
      // current lng in range [stepSize, epsilon]. Move servo to the exact target.
      _currentLng = _targetLng;
      _servos->write(1, _currentLng);
    }
  }

private:
  bool _stateInit = true;
  uint8_t _stateInitCounter = 0;

  enum class State {
    Init,
    Ready
  };
  State _state = State::Init;

  const float _epsilon = 0.1f;  // 0.1 deg, 0.1 mm

  prfServoImpl<TLog, TServo, TEeprom>* _impl;
  prfServo2<uint8_t, float, uint16_t>* _servos;

  typename TLog::Type* _log = TLog::create();
  Messages& _messages;
  TEeprom& _eeprom;
  MessageData_t _inputMsg = 0;

  float _targetRtn = 90;
  float _targetLng = 0;
  float _currentRtn = 90;
  float _currentLng = 0;
  float _stepSizeRtn = 1;
  float _stepSizeLng = 1;
};

}

#endif

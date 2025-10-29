#ifndef OverloadMonitorAo_h
#define OverloadMonitorAo_h

#include "../Shared/Messages.h"
#include "../Shared/BitCounter.h"
#include "../Shared/ResourceStrings.h"
#include "tsm.h"

namespace nsom {
using namespace tsmlib;

#define DBG(x)

PRSDEFI(RSAO, "OverloadMonitor");
PRSDEFI(RSMONITORING, "Monitoring");
PRSDEFI(RSOVERLOAD, "Overload");
PRSDEFI(RSISOVERLOAD, "IsOverload");

const uint8_t OVERLOAD_PIN = 9;

using StatePolicy = State<MemoryAddressComparator, true>;
template<class TLog> struct Startup;
template<class TLog> struct Monitoring;
template<class TLog> struct Overload;
template<class TLog> struct IsStartupDone;
template<class TLog, class TEeprom> struct IsOverload;

namespace event {
struct Timeout {
  MessageData_t msg;
  Messages& messages;
};
}

template<class TLog, class TEeprom>
class OverloadMonitorAo {
public:
  explicit OverloadMonitorAo(Messages& messages, TEeprom& eeprom)
    : _messages(messages) {
    pinMode(OVERLOAD_PIN, OUTPUT);
    _statemachine.begin();

    char version = eeprom.read(4);
    _log->print(F("Calibration Data Version "));
    _log->println(version == 1);
    if (version == 1) {
      CalibrationData_V1 calibrationData;

      eeprom.get(5, calibrationData);

      lowerThresholdRtn = calibrationData.rtnOverflowLowerThreshold;
      upperThresholdRtn = calibrationData.rtnOverflowUpperThreshold;
      lowerThresholdLng = calibrationData.lngOverflowLowerThreshold;
      upperThresholdLng = calibrationData.lngOverflowUpperThreshold;

    } else {
      rs_println(RSAO, RSSTARTUP, RSUSEHARDCODEDVALUES);
    }
  }

  static uint16_t lowerThresholdRtn;
  static uint16_t upperThresholdRtn;
  static uint16_t lowerThresholdLng;
  static uint16_t upperThresholdLng;

  void load() {
    _inputMsg = _messages.toOverloadMonitorQueue.pop();
  }

  void run() {
    _statemachine.dispatch(event::Timeout{ _inputMsg, _messages });
    _inputMsg = 0;
  }

private:
  typename TLog::Type* _log = TLog::create();
  Messages& _messages;
  MessageData_t _inputMsg = 0;

  using ToMonitoringFromStartup = Transition<event::Timeout, Monitoring<TLog>, Startup<TLog>, IsStartupDone<TLog>, NoAction>;
  using ToOverloadFromMonitoring = Transition<event::Timeout, Overload<TLog>, Monitoring<TLog>, IsOverload<TLog, TEeprom>, NoAction>;

  using Transitions =
    Typelist<ToMonitoringFromStartup,
    Typelist<ToOverloadFromMonitoring,
    NullType>>;

  using InitTransition = InitialTransition<Startup<TLog>, NoAction>;
  using Sm = Statemachine<Transitions, InitTransition>;
  Sm _statemachine;
};

template<class TLog, class TEeprom>
uint16_t OverloadMonitorAo<TLog, TEeprom>::lowerThresholdRtn = 700;
template<class TLog, class TEeprom>
uint16_t OverloadMonitorAo<TLog, TEeprom>::upperThresholdRtn = 1000;
template<class TLog, class TEeprom>
uint16_t OverloadMonitorAo<TLog, TEeprom>::lowerThresholdLng = 700;
template<class TLog, class TEeprom>
uint16_t OverloadMonitorAo<TLog, TEeprom>::upperThresholdLng = 1000;

template<class TLog>
struct Startup : public BasicState<Startup<TLog>, StatePolicy, true>, public SingletonCreator<Startup<TLog>> {

  template<class Event>
  void entry(const Event&) {
    //DBG(TLog::create()->println(rs_cat(RSAO, RSSTARTUP, RSENTRY)));
    DBG(rs_println(RSAO, RSSTARTUP, RSENTRY));
    pinMode(OVERLOAD_PIN, OUTPUT);
    digitalWrite(OVERLOAD_PIN, HIGH);

    // TODO EEPROM.get
    rs_println(RSAO, RSSTARTUP, RSUSEHARDCODEDVALUES);
  }

  BitCounter<6> counter;
};

template<class TLog>
struct Monitoring : public BasicState<Monitoring<TLog>, StatePolicy, true>, public SingletonCreator<Monitoring<TLog>> {
  template<class Event>
  void entry(const Event&) {
    //DBG(TLog::create()->println(rs_cat(RSAO, RSMONITORING, RSENTRY)));
    DBG(rs_println(RSAO, RSMONITORING, RSENTRY));
  }

  uint16_t currentRtn = 0;
  uint16_t currentLng = 0;
};

template<class TLog>
struct Overload : public BasicState<Overload<TLog>, StatePolicy, true>, public SingletonCreator<Overload<TLog>> {
  template<class Event>
  void entry(const Event&) {
  }
  template<class Event>
  void entry(const event::Timeout& ev) {
    //DBG(TLog::create()->println(rs_cat(RSAO, RSOVERLOAD, RSENTRY)));
    DBG(rs_println(RSAO, RSOVERLOAD, RSENTRY));
    digitalWrite(OVERLOAD_PIN, LOW);
    ev.messages.toSystemMonitorQueue.push(SystemStatusData(SystemStatusInfo::Error).raw);
  }
};

template<class TLog>
struct IsStartupDone {
  bool eval(Startup<TLog>& startupState, const event::Timeout&) {
    return startupState.counter.increment();
  }
};

template<class TLog, class TEeprom>
struct IsOverload {
  bool eval(Monitoring<TLog>& monitoringState, const event::Timeout& ev) {

    typename TLog::Type* log = TLog::create();
    SystemStatusData status(ev.msg);

    if (status.error) return true;

    auto lngSensor = analogRead(A0);
    auto rtnSensor = analogRead(A1);

    DBG(
    if (_counter.increment()) {
      log->print(F("Lng:"));
      log->print(lngSensor);
      log->print(F(" Rtn:"));
      log->println(rtnSensor);
    });

    monitoringState.currentLng = lngSensor;
    monitoringState.currentRtn = rtnSensor;

    if (!(OverloadMonitorAo<TLog, TEeprom>::lowerThresholdLng < lngSensor && lngSensor < OverloadMonitorAo<TLog, TEeprom>::upperThresholdLng)) {
      //log->print(rs_cat(RSISOVERLOAD, RSMONITORING, RSOVERLOAD));
      rs_print(RSISOVERLOAD, RSMONITORING, RSOVERLOAD);
      log->print(F(" Lng:"));
      log->print(lngSensor);
      log->print(F(" Rtn:"));
      log->println(rtnSensor);

      return true;
    }

    if (!(OverloadMonitorAo<TLog, TEeprom>::lowerThresholdRtn < rtnSensor && rtnSensor < OverloadMonitorAo<TLog, TEeprom>::upperThresholdRtn)) {
      //log->print(rs_cat(RSISOVERLOAD, RSMONITORING, RSOVERLOAD));
      rs_print(RSISOVERLOAD, RSMONITORING, RSOVERLOAD);
      log->print(F(" Lng:"));
      log->print(lngSensor);
      log->print(F(" Rtn:"));
      log->println(rtnSensor);

      return true;
    }

    return false;
  }

  static BitCounter<6> _counter;
};

template<class TLog, class TEeprom>
BitCounter<6> IsOverload<TLog, TEeprom>::_counter;

}
#endif

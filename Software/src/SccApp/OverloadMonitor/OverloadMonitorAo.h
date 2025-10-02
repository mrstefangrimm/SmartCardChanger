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
template<class TLog> struct IsOverload;

namespace event {
struct Timeout {
  MessageData_t msg;
  Messages& messages;
};
}

template<class TLog>
class OverloadMonitorAo {
public:
  explicit OverloadMonitorAo(Messages& messages)
    : _messages(messages) {
    pinMode(OVERLOAD_PIN, OUTPUT);
    _statemachine.begin();
  }

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
  using ToOverloadFromMonitoring = Transition<event::Timeout, Overload<TLog>, Monitoring<TLog>, IsOverload<TLog>, NoAction>;

  using Transitions =
    Typelist<ToMonitoringFromStartup,
             Typelist<ToOverloadFromMonitoring,
                      NullType>>;

  using InitTransition = InitialTransition<Startup<TLog>, NoAction>;
  using Sm = Statemachine<Transitions, InitTransition>;
  Sm _statemachine;
};

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

  uint16_t lowerThresholdRtn = 680;
  uint16_t upperThresholdRtn = 810;
  uint16_t lowerThresholdLng = 760;
  uint16_t upperThresholdLng = 840;

  uint16_t currentRtn;
  uint16_t currentLng;
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

template<class TLog>
struct IsOverload {
  bool eval(Monitoring<TLog>& monitoringState, const event::Timeout& ev) {

    typename TLog::Type* log = TLog::create();
    SystemStatusData status(ev.msg);

    if (status.error) return true;

    auto lngSensor = analogRead(A0);
    auto rtnSensor = analogRead(A1);

    DBG({
        if (abs(lngSensor - (int)monitoringState.currentLng) > 10 || abs(rtnSensor - (int)monitoringState.currentRtn) > 10) {
          log->print(F("Lng:"));
          log->print(lngSensor);
          log->print(F(" Rtn:"));
          log->println(rtnSensor);
      } });

    monitoringState.currentLng = lngSensor;
    monitoringState.currentRtn = rtnSensor;

    if (!(monitoringState.lowerThresholdLng < lngSensor && lngSensor < monitoringState.upperThresholdLng)) {
      //log->print(rs_cat(RSISOVERLOAD, RSMONITORING, RSOVERLOAD));
      rs_print(RSISOVERLOAD, RSMONITORING, RSOVERLOAD);
      log->print(F(" Lng:"));
      log->print(lngSensor);
      log->print(F(" Rtn:"));
      log->println(rtnSensor);

      return true;
    }

    if (!(monitoringState.lowerThresholdRtn < rtnSensor && rtnSensor < monitoringState.upperThresholdRtn)) {
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
};

}
#endif

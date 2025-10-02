#ifndef SystemMonitorAo_h
#define SystemMonitorAo_h

#include "../Shared/Messages.h"
#include "../Shared/BitCounter.h"
#include "../Shared/ResourceStrings.h"
#include "tsm.h"

namespace nssm {
using namespace tsmlib;

#define DBG(x) 

PRSDEFI(RSAO, "SystemMonitor");
PRSDEFI(RSGREEN, "Green");
PRSDEFI(RSYELLOW, "Yellow");
PRSDEFI(RSRED, "Red");
PRSDEFI(RSBLINKING, "Blining");

const uint8_t LED_GREEN = 5;
const uint8_t LED_YELLOW = 10;
const uint8_t LED_RED = 13;

using StatePolicy = State<MemoryAddressComparator, true>;
template<class TLog> struct PermanentGreen;
template<class TLog> struct BlinkGreen;
template<class TLog> struct PermanentYellow;
template<class TLog> struct BlinkYellow;
template<class TLog> struct PermanentRed;

namespace event {
struct Moving {};
struct AtPosition {};
struct Error {};
struct NeedsCalibration {};
struct Service {};
struct Timeout {};
}

template<class TLog>
class SystemMonitorAo {
public:
  explicit SystemMonitorAo(Messages& messages)
    : _messages(messages) {
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    _statemachine.begin();
  }

  void load() {
    _inputMsg = _messages.toSystemMonitorQueue.pop();
  }

  void run() {
    if (_inputMsg != 0) {
      SystemStatusData msg(_inputMsg);
      if (msg.moving) {
        _statemachine.dispatch(event::Moving{});
      } else if (msg.atPosition) {
        _statemachine.dispatch(event::AtPosition{});
      } else if (msg.error) {
        _statemachine.dispatch(event::Error{});
      } else if (msg.needsCalibration) {
        _statemachine.dispatch(event::NeedsCalibration{});
      } else if (msg.service) {
        _statemachine.dispatch(event::Service{});
      }
    } else {
      _statemachine.dispatch(event::Timeout{});
    }
    _inputMsg = 0;
  }

private:
  typename TLog::Type* _log = TLog::create();
  Messages& _messages;
  MessageData_t _inputMsg = 0;

  // From PermanentGreen
  using ToBlinkGreenFromPermanentGreen = Transition<event::Moving, BlinkGreen<TLog>, PermanentGreen<TLog>, NoGuard, NoAction>;
  using ToPermanentGreenFromPermanentGreen = SelfTransition<event::Timeout, PermanentGreen<TLog>, NoGuard, NoAction, false>;
  using ToPermanentRedFromPermanentGreen = Transition<event::Error, PermanentRed<TLog>, PermanentGreen<TLog>, NoGuard, NoAction>;
  using ToBlinkYellowFromPermanentGreen = Transition<event::NeedsCalibration, BlinkYellow<TLog>, PermanentGreen<TLog>, NoGuard, NoAction>;
  using ToPermanentYellowFromPermanentGreen = Transition<event::Service, PermanentYellow<TLog>, PermanentGreen<TLog>, NoGuard, NoAction>;

  // From BlinkGreen
  using ToPermanentGreenGreenFromBlinkGreen = Transition<event::AtPosition, PermanentGreen<TLog>, BlinkGreen<TLog>, NoGuard, NoAction>;
  using ToBlinkGreenFromBlinkGreen = SelfTransition<event::Moving, BlinkGreen<TLog>, NoGuard, NoAction, false>;
  using ToPermanentRedFromBlinkGreen = Transition<event::Error, PermanentRed<TLog>, BlinkGreen<TLog>, NoGuard, NoAction>;

  // From PermanentYello
  using ToPermanentGreenFromPermanentYellow = Transition<event::Service, PermanentGreen<TLog>, PermanentYellow<TLog>, NoGuard, NoAction>;
  using ToPermanentYellowFromPermanentYellow = SelfTransition<event::Timeout, PermanentYellow<TLog>, NoGuard, NoAction, false>;

  // From BlinkYello
  using ToPermanentYellowFromBlinkYellow = Transition<event::Service, PermanentYellow<TLog>, BlinkYellow<TLog>, NoGuard, NoAction>;
  using ToBlinkYellowFromBlinkYellow = SelfTransition<event::Timeout, BlinkGreen<TLog>, NoGuard, NoAction, false>;

  using Transitions =
    Typelist< ToBlinkGreenFromPermanentGreen,
              Typelist< ToPermanentGreenFromPermanentGreen,
                        Typelist< ToPermanentRedFromPermanentGreen,
                                  Typelist< ToBlinkYellowFromPermanentGreen,
                                            Typelist< ToPermanentYellowFromPermanentGreen,
                                                      Typelist< ToPermanentGreenGreenFromBlinkGreen,
                                                                Typelist< ToBlinkGreenFromBlinkGreen,
                                                                          Typelist< ToPermanentRedFromBlinkGreen,
                                                                                    Typelist< ToPermanentGreenFromPermanentYellow,
                                                                                              Typelist< ToPermanentYellowFromPermanentYellow,
                                                                                                        Typelist< ToPermanentYellowFromBlinkYellow,
                                                                                                                  Typelist< ToBlinkYellowFromBlinkYellow,
                                                                                                                            NullType>>>>>>>>>>>>;

  using InitTransition = InitialTransition<PermanentGreen<TLog>, NoAction>;
  using Sm = Statemachine<Transitions, InitTransition>;
  Sm _statemachine;
};

template<class TLog>
struct PermanentGreen : public BasicState<PermanentGreen<TLog>, StatePolicy, true, false, true>, public SingletonCreator<PermanentGreen<TLog>> {
  template<class Event>
  void entry(const Event&) {
    DBG(rs_println(RSAO, RSGREEN, RSENTRY));

    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
  }

  template<class Event>
  void doit(const Event&) {
    //TLog::create()->println("SystemMonitor::PermanentGreen::doit");

    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
  }
};

template<class TLog>
struct PermanentYellow : public BasicState<PermanentYellow<TLog>, StatePolicy, true>, public SingletonCreator<PermanentYellow<TLog>> {
  template<class Event>
  void entry(const Event&) {
    DBG(rs_println(RSAO, RSYELLOW, RSENTRY));

    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_RED, LOW);
  }

  template<class Event>
  void doit(const Event&) {
    //TLog::create()->println(F("SystemMonitor::PermanentYellow::doit"));

    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_RED, LOW);
  }
};

template<class TLog>
struct PermanentRed : public BasicState<PermanentRed<TLog>, StatePolicy, true>, public SingletonCreator<PermanentRed<TLog>> {
  template<class Event>
  void entry(const Event&) {
    DBG(rs_println(RSAO, RSRED, RSENTRY));

    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, HIGH);
  }
};

template<class TLog>
struct BlinkGreen : public BasicState<BlinkGreen<TLog>, StatePolicy, true, false, true>, public SingletonCreator<BlinkGreen<TLog>> {
  template<class Event>
  void entry(const Event&) {
    DBG(rs_println(RSAO, RSGREEN, RSBLINKING, RSENTRY));

    digitalWrite(LED_GREEN, _ledState);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
  }

  template<class Event>
  void doit(const Event&) {
    //TLog::create()->println(F("SystemMonitor::BlinkGreen::doit"));

    if (_blinkCounter.increment()) {
      _ledState = _ledState == LOW ? HIGH : LOW;
    }
    digitalWrite(LED_GREEN, _ledState);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
  }

  BitCounter<3> _blinkCounter;
  uint8_t _ledState = LOW;
};

template<class TLog>
struct BlinkYellow : public BasicState<BlinkYellow<TLog>, StatePolicy, true, false, true>, public SingletonCreator<BlinkYellow<TLog>> {

  template<class Event>
  void entry(const Event&) {
    DBG(rs_println(RSAO, RSYELLOW, RSBLINKING, RSENTRY));

    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, _ledState);
    digitalWrite(LED_RED, LOW);
  }

  template<class Event>
  void doit(const Event&) {
    //TLog::create()->println(F("SystemMonitor::BlinkYellow::doit"));

    if (_blinkCounter.increment()) {
      _ledState = _ledState == LOW ? HIGH : LOW;
    }
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, _ledState);
    digitalWrite(LED_RED, LOW);
  }

  BitCounter<3> _blinkCounter;
  uint8_t _ledState = LOW;
};

}

#endif

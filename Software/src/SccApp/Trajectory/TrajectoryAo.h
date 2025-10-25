#ifndef TrajectoryAo_h
#define TrajectoryAo_h

#include "../Shared/Messages.h"
#include "../Shared/BitCounter.h"
#include "../Shared/ResourceStrings.h"

namespace nstj {
using namespace tsmlib;

#define DBG(x) 

PRSDEFI(RSAO, "Trajectory");
PRSDEFI(RSCONNECTING, "Connecting");
PRSDEFI(RSCONNECTED, "Connected");
PRSDEFI(RSRETRACTING, "Retracting");
PRSDEFI(RSRETRACTED, "Retracted");

using StatePolicy = State<MemoryAddressComparator, true>;
template<class TLog> struct Startup;
template<class TLog> struct Retracted;
template<class TLog> struct Connecting;
template<class TLog> struct Retracting;
template<class TLog> struct Connected;
template<class TLog> struct IsConnected;
template<class TLog> struct IsRetracted;

namespace event {
struct Connect {
  Messages& messages;
  float targetPos;
};
struct Retract {
  Messages& messages;
};
struct Panic {
  Messages& messages;
};
struct Overload {
  Messages& messages;
};
struct Timeout {
  Messages& messages;
};
}

const uint8_t RTN = 0;
const uint8_t LNG = 1;
static const float retractedPos = 0;
static const float disconnectedPos = 30;
static const float connectedPos = 72;

template<class TLog>
class TrajectoryAo {
public:
  TrajectoryAo(Messages& messages)
    : _messages(messages) {
    _statemachine.begin();
  }

  void load() {
    _inputMsg = _messages.toTrajectoryQueue.pop();
  }

  void run() {
    if (_inputMsg != 0) {
      TrajectoryData msg(_inputMsg);
      if (msg.connectA) {
        _statemachine.dispatch(event::Connect{ _messages, cardAPos });
      } else if (msg.connectB) {
        _statemachine.dispatch(event::Connect{ _messages, cardBPos });
      } else if (msg.connectC) {
        _statemachine.dispatch(event::Connect{ _messages, cardCPos });
      } else if (msg.connectD) {
        _statemachine.dispatch(event::Connect{ _messages, cardDPos });
      } else if (msg.connectE) {
        _statemachine.dispatch(event::Connect{ _messages, cardEPos });
      } else if (msg.retract) {
        _statemachine.dispatch(event::Retract{ _messages });
      } else if (msg.panic) {
        _statemachine.dispatch(event::Panic{ _messages });
      } else if (msg.overload) {
        _statemachine.dispatch(event::Overload{ _messages });
      }
    } else {
      _statemachine.dispatch(event::Timeout{ _messages });
    }
    _inputMsg = 0;
  }

private:
  const float cardAPos = 30;
  const float cardBPos = 60;
  const float cardCPos = 90;
  const float cardDPos = 120;
  const float cardEPos = 150;

  Messages& _messages;
  MessageData_t _inputMsg = 0;
  BitCounter<6> _counter;

  // From Startup
  using ToRetractedFromStartup = Transition<event::Timeout, Retracted<TLog>, Startup<TLog>, NoGuard, NoAction>;

  // From Retracted
  using ToConnectingFromRetracted = Transition<event::Connect, Connecting<TLog>, Retracted<TLog>, NoGuard, NoAction>;

  // From Connecting
  using ToConnectedFromConnecting = ChoiceTransition<event::Timeout, Connected<TLog>, Connecting<TLog>, Connecting<TLog>, IsConnected<TLog>, NoAction>;
  using ToRetractedFromConnecting = Transition<event::Panic, Retracted<TLog>, Connecting<TLog>, NoGuard, NoAction>;

  // From Connected
  using ToRetractingFromConnected = Transition<event::Retract, Retracting<TLog>, Connected<TLog>, NoGuard, NoAction>;
  using ToRetractedFromConnected = Transition<event::Panic, Retracted<TLog>, Connected<TLog>, NoGuard, NoAction>;

  // From Retracting
  using ToRetractedFromRetracting = ChoiceTransition<event::Timeout, Retracted<TLog>, Retracting<TLog>, Retracting<TLog>, IsRetracted<TLog>, NoAction>;
  using ToRetractedFromRetractingPanic = Transition<event::Panic, Retracted<TLog>, Connecting<TLog>, NoGuard, NoAction>;

  using Transitions =
    Typelist< ToRetractedFromStartup,
              Typelist< ToConnectingFromRetracted,
                        Typelist< ToConnectedFromConnecting,
                                  Typelist< ToRetractedFromConnecting,
                                            Typelist< ToRetractingFromConnected,
                                                      Typelist< ToRetractedFromConnected,
                                                                Typelist< ToRetractedFromRetracting,
                                                                          Typelist< ToRetractedFromRetractingPanic,
                                                                                    NullType>>>>>>>>;

  using InitTransition = InitialTransition<Startup<TLog>, NoAction>;
  using Sm = Statemachine<Transitions, InitTransition>;
  Sm _statemachine;
};

template<class TLog>
struct Startup : public BasicState<Startup<TLog>, StatePolicy, true>, public SingletonCreator<Startup<TLog>> {
  template<class Event>
  void entry(const Event&) {
    DBG(rs_println(RSAO, RSSTARTUP, RSENTRY));
  }
};

template<class TLog>
struct Retracted : public BasicState<Retracted<TLog>, StatePolicy, true>, public SingletonCreator<Retracted<TLog>> {
  template<class Event>
  void entry(const Event&) {
    rtassert(false);
  }
  template<class Event>
  void entry(const event::Panic& ev) {
    DBG(rs_println(RSAO, RSRETRACTED, RSENTRY));
    ev.messages.toSystemMonitorQueue.push(SystemStatusData(SystemStatusInfo::AtPosition).raw);
    ev.messages.toOverloadMonitorQueue.push(SystemStatusData(SystemStatusInfo::AtPosition).raw);
  }
  template<class Event>
  void entry(const event::Timeout& ev) {
    DBG(rs_println(RSAO, RSRETRACTED, RSENTRY));
    ev.messages.toSystemMonitorQueue.push(SystemStatusData(SystemStatusInfo::AtPosition).raw);
    ev.messages.toOverloadMonitorQueue.push(SystemStatusData(SystemStatusInfo::AtPosition).raw);
  }
};

template<class TLog>
struct Connecting : public BasicState<Connecting<TLog>, StatePolicy, true, true, true>, public SingletonCreator<Connecting<TLog>> {
  template<class Event>
  void entry(const Event&) {
    rtassert(false);
  }
  template<class Event>
  void entry(const event::Connect& ev) {
    DBG(rs_println(RSAO, RSCONNECTING, RSENTRY));
    _counter = BitCounter<6>();
    _state = StateConnecting::Init;
    _targetPos = ev.targetPos;

    ev.messages.toSystemMonitorQueue.push(SystemStatusData(SystemStatusInfo::Moving).raw);
    ev.messages.toOverloadMonitorQueue.push(SystemStatusData(SystemStatusInfo::Moving).raw);
  }

  template<class Event>
  void doit(const event::Panic& ev) {
  }
  template<class Event>
  void doit(const event::Connect& ev) {
    // intentionally left blank.
  }
  template<class Event>
  void doit(const event::Timeout& ev) {

    if (_state == StateConnecting::Init) {
      ev.messages.toPulsWidthTransmitterQueue.push(TargetPosData(RTN, _targetPos, 2).raw);
      _state = StateConnecting::Revolving;
    } else if (_state == StateConnecting::Revolving) {
      if (_counter.increment()) {
        _counter = BitCounter<6>(50);
        ev.messages.toPulsWidthTransmitterQueue.push(TargetPosData(LNG, disconnectedPos, 2).raw);
        _state = StateConnecting::ToDisconnected;
      }
    } else if (_state == StateConnecting::ToDisconnected) {
      if (_counter.increment()) {
        _counter = BitCounter<6>(20);
        ev.messages.toPulsWidthTransmitterQueue.push(TargetPosData(LNG, connectedPos, 10).raw);
        _state = StateConnecting::ToConnected;
      }
    } else if (_state == StateConnecting::ToConnected) {
      if (_counter.increment()) {
        _state = StateConnecting::AtFinalPosition;
      }
    }
  }

  template<class Event>
  void exit(const Event& ev) {
    DBG(rs_println(RSAO, RSRETRACTED, RSEXIT));
  }
  template<class Event>
  void exit(const event::Panic& ev) {
    DBG(rs_println(RSAO, RSRETRACTED, RSEXIT));
    ev.messages.toPulsWidthTransmitterQueue.push(TargetPosData(LNG, retractedPos, 15).raw);
  }

  bool connected() {
    return _state == StateConnecting::AtFinalPosition;
  }

private:
  float _targetPos = 90;
  BitCounter<6> _counter;

  enum class StateConnecting {
    Init,
    Revolving,
    ToDisconnected,
    ToConnected,
    AtFinalPosition
  };
  StateConnecting _state = StateConnecting::Init;
};

template<class TLog>
struct Connected : public BasicState<Connected<TLog>, StatePolicy, true, true, false>, public SingletonCreator<Connected<TLog>> {
  template<class Event>
  void entry(const Event&) {
    rtassert(false);
  }
  template<class Event>
  void entry(const event::Timeout& ev) {
    DBG(TLog::create()->println(F("Trajectory::Connected::entry")));
    DBG(rs_println(RSAO, RSCONNECTED, RSENTRY));
    ev.messages.toSystemMonitorQueue.push(SystemStatusData(SystemStatusInfo::AtPosition).raw);
    ev.messages.toOverloadMonitorQueue.push(SystemStatusData(SystemStatusInfo::AtPosition).raw);
  }

  template<class Event>
  void exit(const Event& ev) {
    DBG(TLog::create()->println(F("Trajectory::Connected::exit")));
  }
  template<class Event>
  void exit(const event::Panic& ev) {
    DBG(rs_println(RSAO, RSRETRACTED, RSEXIT, RSPANIC));
    ev.messages.toPulsWidthTransmitterQueue.push(TargetPosData(LNG, retractedPos, 15).raw);
  }
};

template<class TLog>
struct IsConnected {
  bool eval(Connecting<TLog>& state, const event::Timeout&) {
    return state.connected();
  }
};

template<class TLog>
struct Retracting : public BasicState<Retracting<TLog>, StatePolicy, true, true, true>, public SingletonCreator<Retracting<TLog>> {
  template<class Event>
  void entry(const Event&) {
    rtassert(false);
  }
  template<class Event>
  void entry(const event::Retract& ev) {
    DBG(rs_println(RSAO, RSRETRACTING, RSENTRY));
    _counter = BitCounter<6>();
    _state = StateRetracting::Init;

    ev.messages.toSystemMonitorQueue.push(SystemStatusData(SystemStatusInfo::Moving).raw);
    ev.messages.toOverloadMonitorQueue.push(SystemStatusData(SystemStatusInfo::Moving).raw);
  }

  template<class Event>
  void doit(const event::Panic& ev) {
  }
  template<class Event>
  void doit(const event::Retract& ev) {
    // intentionally left blank.
  }
  template<class Event>
  void doit(const event::Timeout& ev) {

    if (_state == StateRetracting::Init) {
      _counter = BitCounter<6>(20);
      ev.messages.toPulsWidthTransmitterQueue.push(TargetPosData(LNG, disconnectedPos, 10).raw);
      _state = StateRetracting::ToDisconnected;
    }
    if (_state == StateRetracting::ToDisconnected) {
      if (_counter.increment()) {
        _counter = BitCounter<6>(30);
        ev.messages.toPulsWidthTransmitterQueue.push(TargetPosData(LNG, retractedPos, 2).raw);
        _state = StateRetracting::ToRetracted;
      }
    }
    if (_state == StateRetracting::ToRetracted) {
      if (_counter.increment()) {
        _state = StateRetracting::AtFinalPosition;
      }
    }
  }

  template<class Event>
  void exit(const Event& ev) {
    DBG(rs_println(RSAO, RSRETRACTING, RSEXIT));
  }
  template<class Event>
  void exit(const event::Panic& ev) {
    DBG(rs_println(RSAO, RSRETRACTING, RSEXIT, RSPANIC));
    ev.messages.toPulsWidthTransmitterQueue.push(TargetPosData(LNG, retractedPos, 15).raw);
  }

  bool retracted() {
    return _state == StateRetracting::AtFinalPosition;
  }

private:
  BitCounter<6> _counter;

  enum class StateRetracting {
    Init,
    ToDisconnected,
    ToRetracted,
    AtFinalPosition
  };
  StateRetracting _state = StateRetracting::Init;
};

template<class TLog>
struct IsRetracted {
  bool eval(Retracting<TLog>& state, const event::Timeout&) {
    return state.retracted();
  }
};

}

#endif

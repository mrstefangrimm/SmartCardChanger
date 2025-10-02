#ifndef BleTerminalAo_h
#define BleTerminalAo_h

#include "../Shared/Messages.h"
#include "../Shared/BitCounter.h"
#include "../Shared/ResourceStrings.h"
#include "BluefruitConfig.h"

namespace nsble {

#define DBG(x)

PRSDEFI(RSAO, "BleTerminal");
PRSDEFI(RSSTATEINIT, "Init");
PRSDEFI(RSSTATEDISCONNECT, "Disconnect");
PRSDEFI(RSSTATEDISCONNECTED, "Disconnected");
PRSDEFI(RSSTATECONNECT, "Connect");
PRSDEFI(RSSTATECONNECTED, "Connected");
PRSDEFI(RSBLEMODULE, "BleModule");
PRSDEFI(RSERROR, "Error");
PRSDEFI(RSFACTORYRESET, "FactoryReset");

#define FACTORYRESET_ENABLE 1
#define MINIMUM_FIRMWARE_VERSION "0.6.6"
#define MODE_LED_BEHAVIOUR "MODE"

template<class TLog, class TBle>
class BleTerminalAo {
public:
  BleTerminalAo(Messages& messages, TBle& ble)
    : _messages(messages), _ble(ble) {
  }

  void load() {
  }

  void run() {
    if (_counter.increment()) {
      switch (_state) {
        case State::Init: stateInit(); break;
        case State::Disconnected: stateDisconnected(); break;
        case State::Connected: stateConnected(); break;
      }
      _inputMsg = 0;
    }
  }

private:
  void stateInit() {
    DBG(rs_println(RSAO, RSSTATEINIT));

    if (!_ble.begin(VERBOSE_MODE)) {
      rs_println(RSAO, RSSTATEINIT, RSBLEMODULE, RSERROR);
      _state = State::Error;
      return;
    }

    if (FACTORYRESET_ENABLE) {
      DBG(rs_println(RSAO, RSSTATEINIT, RSFACTORYRESET));
      if (!_ble.factoryReset()) {
        rs_println(RSAO, RSSTATEINIT, RSFACTORYRESET, RSERROR);
        _state = State::Error;
        return;
      }
    }

    _ble.echo(false);
    DBG(_ble.info());

    _ble.verbose(false);  // debug info is a little annoying after this point!

    _state = State::Disconnected;
  }

  void stateDisconnected() {
    if (_ble.isConnected()) {
      rs_println(RSAO, RSSTATEDISCONNECTED, RSSTATECONNECT, RSBLEMODULE);
      // LED Activity command is only supported from 0.6.6
      if (_ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION)) {
        // Change Mode LED Activity
        _ble.sendCommandCheckOK(F("AT+HWModeLED=" MODE_LED_BEHAVIOUR));
      }
      _state = State::Connected;
    }
  }

  void stateConnected() {
    if (!_ble.isConnected()) {
      rs_println(RSAO, RSSTATECONNECTED, RSSTATEDISCONNECT, RSBLEMODULE);
      _state = State::Disconnected;
      return;
    }

    _ble.println(F("AT+BLEUARTRX"));
    _ble.readline();
    if (strcmp(_ble.buffer, "OK") != 0) {
      DBG(
        auto log = TLog::create();
        log->print(F("[Recv] "));
        log->println(_ble.buffer););
      _ble.waitForOK();

      char lastEnteredChar = _ble.buffer[strlen(_ble.buffer) - 1];

      if (lastEnteredChar == 'a') {
        rs_println(RSSCC, RSSCCMOVETO, RSA);
        _messages.toTrajectoryQueue.push(TrajectoryData(TrajectoryInfo::ConnectA).raw);
      } else if (lastEnteredChar == 'b') {
        rs_println(RSSCC, RSSCCMOVETO, RSB);
        _messages.toTrajectoryQueue.push(TrajectoryData(TrajectoryInfo::ConnectB).raw);
      } else if (lastEnteredChar == 'c') {
        rs_println(RSSCC, RSSCCMOVETO, RSC);
        _messages.toTrajectoryQueue.push(TrajectoryData(TrajectoryInfo::ConnectC).raw);
      } else if (lastEnteredChar == 'd') {
        rs_println(RSSCC, RSSCCMOVETO, RSD);
        _messages.toTrajectoryQueue.push(TrajectoryData(TrajectoryInfo::ConnectD).raw);
      } else if (lastEnteredChar == 'e') {
        rs_println(RSSCC, RSSCCMOVETO, RSE);
        _messages.toTrajectoryQueue.push(TrajectoryData(TrajectoryInfo::ConnectE).raw);
      } else if (lastEnteredChar == 'r') {
        rs_println(RSSCC, RSSCCMOVETORETRACTED);
        _messages.toTrajectoryQueue.push(TrajectoryData(TrajectoryInfo::Retract).raw);
      } else if (lastEnteredChar == 'p') {
        rs_println(RSSCC, RSPANIC);
        _messages.toTrajectoryQueue.push(TrajectoryData(TrajectoryInfo::Panic).raw);
      } else if (lastEnteredChar == 'P') {
        rs_println(RSSCC, RSBIGPANIC);
        _messages.toSystemMonitorQueue.push(SystemStatusData(SystemStatusInfo::Error).raw);
        _messages.toOverloadMonitorQueue.push(SystemStatusData(SystemStatusInfo::Error).raw);
      }
    }
  }

  enum class State {
    Init,
    Disconnected,
    Connected,
    Error
  };
  State _state = State::Init;

  TBle& _ble;
  Messages& _messages;
  MessageData_t _inputMsg = 0;
  BitCounter<1> _counter;
};

}

#endif

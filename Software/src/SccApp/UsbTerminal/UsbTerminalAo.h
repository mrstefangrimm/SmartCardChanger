#ifndef UsbTerminalAo_h
#define UsbTerminalAo_h

#include "../Shared/Messages.h"
#include "../Shared/BitCounter.h"
#include "../Shared/ResourceStrings.h"

namespace nsusb {

#define DBG(x) 

//PRSDEFM(RSADJRTN, "Adjust Rtn");
//PRSDEFM(RSADJLNG, "Adjust Lng");
//PRSDEFM(RSSVOOFFSET, "Servo Offset");
//PRSDEFM(RSOVERLLOWTH, "Overload Lower Threshold");
//PRSDEFM(RSOVERLUPPTH, "Overload Upper Threshold");

template<class TLog, class TSerial>
class UsbTerminalAo {
public:
  UsbTerminalAo(Messages& messages, TSerial& serial)
    : _messages(messages), _serial(serial) {
  }

  void load() {
  }

  void run() {
    if (_counter.increment()) {
      switch (_state) {
      case State::Init: stateInit(); break;
      case State::Ready: stateReady(); break;
      case State::Service: stateService(); break;
      }
    }
  }

private:
  void stateInit() {
    if (_serial) {
      _serial.begin(9600);
      DBG(_log->println(F("Serial is ready.")));

      stateReadyPrintHelp();

      _state = State::Ready;
    }
  }

  void stateReady() {
    if (_serial.available() > 0) {
      char cmd = _serial.read();
      DBG(_log->println(cmd));
      if (cmd == 'a') {
        rs_println(RSSCC, RSSCCMOVETO, RSA);
        _messages.toTrajectoryQueue.push(TrajectoryData(TrajectoryInfo::ConnectA).raw);
      } else if (cmd == 'b') {
        rs_println(RSSCC, RSSCCMOVETO, RSB);
        _messages.toTrajectoryQueue.push(TrajectoryData(TrajectoryInfo::ConnectB).raw);
      } else if (cmd == 'c') {
        rs_println(RSSCC, RSSCCMOVETO, RSC);
        _messages.toTrajectoryQueue.push(TrajectoryData(TrajectoryInfo::ConnectC).raw);
      } else if (cmd == 'd') {
        rs_println(RSSCC, RSSCCMOVETO, RSD);
        _messages.toTrajectoryQueue.push(TrajectoryData(TrajectoryInfo::ConnectD).raw);
      } else if (cmd == 'e') {
        rs_println(RSSCC, RSSCCMOVETO, RSE);
        _messages.toTrajectoryQueue.push(TrajectoryData(TrajectoryInfo::ConnectE).raw);
      } else if (cmd == 'r') {
        rs_println(RSSCC, RSSCCMOVETORETRACTED);
        _messages.toTrajectoryQueue.push(TrajectoryData(TrajectoryInfo::Retract).raw);
      } else if (cmd == 'p') {
        rs_println(RSSCC, RSPANIC);
        _messages.toTrajectoryQueue.push(TrajectoryData(TrajectoryInfo::Panic).raw);
      } else if (cmd == 'P') {
        rs_println(RSSCC, RSBIGPANIC);
        _messages.toSystemMonitorQueue.push(SystemStatusData(SystemStatusInfo::Error).raw);
        _messages.toOverloadMonitorQueue.push(SystemStatusData(SystemStatusInfo::Error).raw);
      } else if (cmd == '?') {
        stateReadyPrintHelp();
      } else if (cmd == 's') {
        rs_println(RSSCC, RSSERVICE);
        _messages.toSystemMonitorQueue.push(SystemStatusData(SystemStatusInfo::Service).raw);
        _messages.toServiceQueue.push(ServiceCmdData(ServiceCmdInfo::Enter).raw);
        _state = State::Service;
        stateServicePrintHelp();
      }
    }
  }

  void stateReadyPrintHelp() {
    _serial.print(F("\033[0H\033[0J"));

    _serial.println();
    _serial.println(F("a: Connect Card A"));
    _serial.println(F("b: Connect Card B"));
    _serial.println(F("c: Connect Card C"));
    _serial.println(F("d: Connect Card D"));
    _serial.println(F("e: Connect Card E"));
    _serial.println(F("d: Retract"));
    _serial.println(F("p: Panic - Immediately retract"));
    _serial.println(F("P: Big Panic - Immediately power off servos"));
    _serial.println(F("?: Help"));
    _serial.println(F("s: Service"));
  }

  char cmdBuf[20] = { 0 };

  void stateService() {
    if (_serial.available() > 0) {
      char cmd = _serial.read();

      if (cmdBuf[0] == 0 && cmd == '?') {
        stateServicePrintHelp();
      }
      if (cmdBuf[0] == 0 && cmd == 'q') {
        _messages.toServiceQueue.push(ServiceCmdData(ServiceCmdInfo::Quit).raw);
        _messages.toSystemMonitorQueue.push(SystemStatusData(SystemStatusInfo::Service).raw);
        _state = State::Ready;
      } else if (cmdBuf[0] == 0 && cmd == 'p') {
        _messages.toServiceQueue.push(ServiceCmdData(ServiceCmdInfo::Print).raw);
      } else if (cmd == 'a') {
        memset(cmdBuf, 0, 20);
        cmdBuf[0] = 'a';
        _serial.print((char)cmd);
      } else if (cmdBuf[0] == 'a') {

        if (cmd == 13) {
          if (cmdBuf[1] = 'r' && cmdBuf[2] == 's' && cmdBuf[3] == 'o') {
            char numBuf[10] = { 0 };
            memcpy(numBuf, &cmdBuf[5], 10);
            int16_t rtnAdjustment = atoi(numBuf);
            _messages.toServiceQueue.push(ServiceCmdData(ServiceCmdInfo::AdjustRtnServoOffset, rtnAdjustment).raw);
          } else if (cmdBuf[1] = 'l' && cmdBuf[2] == 's' && cmdBuf[3] == 'o') {
            char numBuf[10] = { 0 };
            memcpy(numBuf, &cmdBuf[5], 10);
            int16_t rtnAdjustment = atoi(numBuf);
            _messages.toServiceQueue.push(ServiceCmdData(ServiceCmdInfo::AdjustLngServoOffset, rtnAdjustment).raw);
          } else if (cmdBuf[1] = 'r' && cmdBuf[2] == 'o' && cmdBuf[3] == 'l') {
            char numBuf[10] = { 0 };
            memcpy(numBuf, &cmdBuf[5], 10);
            int16_t rtnAdjustment = atoi(numBuf);
            _messages.toServiceQueue.push(ServiceCmdData(ServiceCmdInfo::AdjustRtnOverloadLower, rtnAdjustment).raw);
          } else if (cmdBuf[1] = 'r' && cmdBuf[2] == 'o' && cmdBuf[3] == 'u') {
            char numBuf[10] = { 0 };
            memcpy(numBuf, &cmdBuf[5], 10);
            int16_t rtnAdjustment = atoi(numBuf);
            _messages.toServiceQueue.push(ServiceCmdData(ServiceCmdInfo::AdjustRtnOverloadUpper, rtnAdjustment).raw);
          } else if (cmdBuf[1] = 'l' && cmdBuf[2] == 'o' && cmdBuf[3] == 'l') {
            char numBuf[10] = { 0 };
            memcpy(numBuf, &cmdBuf[5], 10);
            int16_t rtnAdjustment = atoi(numBuf);
            _messages.toServiceQueue.push(ServiceCmdData(ServiceCmdInfo::AdjustLngOverloadLower, rtnAdjustment).raw);
          } else if (cmdBuf[1] = 'l' && cmdBuf[2] == 'o' && cmdBuf[3] == 'u') {
            char numBuf[10] = { 0 };
            memcpy(numBuf, &cmdBuf[5], 10);
            int16_t rtnAdjustment = atoi(numBuf);
            _messages.toServiceQueue.push(ServiceCmdData(ServiceCmdInfo::AdjustLngOverloadUpper, rtnAdjustment).raw);
          }

          memset(cmdBuf, 0, 20);
          stateServicePrintHelp();
        }

        for (int i = 1; i < sizeof(cmdBuf); i++) {
          if (cmdBuf[i] == 0) {
            cmdBuf[i] = cmd;
            break;
          }
        }
        _serial.print((char)cmd);




        /*ServiceCmdInfo svcCmdInfo = cmd == 'a'
          ? ServiceCmdInfo::AdjustRtn
          : cmd == 'r'
          ? ServiceCmdInfo::CalibrateRtn
          : ServiceCmdInfo::CalibrateLng;

        auto svcCmdArgStr = _serial.readString();
        auto svcCmdArg = svcCmdArgStr.toInt();
        DBG(
          {
            _serial.print(F("Svc Cmd Arg '"));
            _serial.print(svcCmdArg);
            _serial.println(F("'"));
          }
        );
        _messages.toServiceQueue.push(ServiceCmdData(svcCmdInfo, (int16_t)svcCmdArg).raw);*/
      }
    }
  }

  void stateServicePrintHelp() {
    _serial.print(F("\033[0H\033[0J"));

    //PRSDEFM(RSADJRTN, "Adjust Rtn");
    //PRSDEFM(RSADJLNG, "Adjust Lng");
    //PRSDEFM(RSSVOOFFSET, "Servo Offset");
    //PRSDEFM(RSOVERLLOWTH, "Overload Lower Threshold");
    //PRSDEFM(RSOVERLUPPTH, "Overload Upper Threshold");

    _serial.println();
    _serial.println(F("arso: Adjust RtnServo Offset"));
_serial.println(F("also: Adjust LngServo Offsetg"));
_serial.println(F("arol: Adjust RtnOverload Lower Threshold"));
_serial.println(F("arou: Adjust RtnOverload Lower Threshold"));
_serial.println(F("alol: Adjust LngOverload Lower Threshold"));
_serial.println(F("alou: Adjust LngOverload Lower Threshold"));

    //_serial.print(F("arso: ")); rs_println(RSADJRTN, RSSVOOFFSET);
    //_serial.print(F("also: ")); rs_println(RSADJLNG, RSSVOOFFSET);
    //_serial.print(F("arol: ")); rs_println(RSADJRTN, RSOVERLLOWTH);
    //_serial.print(F("arou: ")); rs_println(RSADJRTN, RSOVERLUPPTH);
    //_serial.print(F("alol: ")); rs_println(RSADJLNG, RSOVERLLOWTH);
    //_serial.print(F("alou: ")); rs_println(RSADJLNG, RSOVERLUPPTH);
    _serial.println(F("p: Print Calibration Data"));
    _serial.println(F("q: quit"));
    _serial.println(F("?: Help"));
  }

  enum class State {
    Init,
    Ready,
    Service
  };
  State _state = State::Init;

  typename TLog::Type* _log = TLog::create();
  Messages& _messages;
  TSerial& _serial;
  BitCounter<2> _counter;
};

}

#endif

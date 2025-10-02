#include <iostream>
#include <cassert>
#include <chrono>
#include <thread>
#include <conio.h>

#define F
#define PROGMEM
const uint8_t OUTPUT = 0;
const uint8_t LOW = 0;
const uint8_t HIGH = 0;
static const uint8_t A0 = 18;
static const uint8_t A1 = 19;
enum class SerialFakeMode { BIN, HEX };
void pinMode(uint8_t pin, uint8_t mode) {}
void delay(uint32_t ms) {}
void digitalWrite(uint8_t pin, uint8_t mode) {}
int analogRead(uint8_t pin) { return 780; }

using namespace std;

#include "SystemMonitor/SystemMonitorAo.h"
#include "PulsWidthTransmitter/PulsWidthTransmitterAo.h"
#include "Trajectory/TrajectoryAo.h"
#include "UsbTerminal/UsbTerminalAo.h"
#include "BleTerminal/BleTerminalAo.h"
#include "OverloadMonitor/OverloadMonitorAo.h"
#include "Service/ServiceAo.h"
#include "Shared/Messages.h"
#include "Shared/Singleton.h"
#include "Shared/prsserial.h"
#include "Shared/ConfigurationData.h"


void rtassert(bool condition) { assert(condition); }

struct DummyString {
  DummyString(char* ch) {

  }
  const char* c_str() const { return ""; }
  int toInt() const { return 1400; }
};

struct DummySerial {
  void begin(unsigned long baud) {}
  int available() { return _kbhit(); }
  int read() { return _getch(); }
  DummyString readString() {
    _cgets_s(_buf, 50, &_pos);
    return DummyString(_buf);
  }
  void print(const char* ch) {
    std::cout << ch;
  }
  void print(char ch) {
    std::cout << ch;
  }
  void print(char ch, uint8_t) {
    char str[2] = { ch, '\0' };
    std::cout << str;
  }
  void print(int val) {
    std::cout << val;
  }
  void print(float val) {
    std::cout << val << std::endl;
  }
  void println() {
    std::cout << std::endl;
  }
  void println(const char* ch) {
    std::cout << ch << std::endl;
  }
  void println(const char* ch, SerialFakeMode) {
    std::cout << ch << std::endl;
  }
  void println(int val) {
    std::cout << val << std::endl;
  }
  void println(float val, uint8_t precision) {
    std::cout << val << std::endl;
  }
  operator bool() { return true; }

private:
  char _buf[50];
  size_t _pos;
};

struct DummyBle {
  bool begin(bool v) { return false; }
  bool isConnected() { return false; }
  size_t println(const char[]) { return 0; }
  uint16_t readline() { return 0; }
  bool waitForOK() { return false; }
  bool factoryReset() { return false; }
  bool echo(bool enable) { return false; }
  void info() {}
  void verbose(bool enable) {}
  bool isVersionAtLeast(const char* versionString) { return false; }
  bool sendCommandCheckOK(const char cmd[]) { return false; }

  char buffer[65] = {};
};

class DummyEEProm {
public:
  DummyEEProm() {
    _header[0] = 'c';
    _header[1] = 'a';
    _header[2] = 'f';
    _header[3] = 'e';
    _header[4] = 1;

    // RTN, 19.Aug.2025
    // 30 1105
    // 60 1270
    // 90 1435
    // 120 1602
    // 150 1765
    _configData.rtnCoefficients[0] = 9.4440000000000009e+002f;
    _configData.rtnCoefficients[1] = 5.2730158730158729e+000f;
    _configData.rtnCoefficients[2] = 3.1746031746031746e-003f;
    _configData.rtnCoefficients[3] = -1.2345679012345678e-005f;

    // 30 1085 -25
    // 60 1245 -25
    // 90 1410 -25
    // 120 1585 -17
    // 150 1745 -20
    _configData.rtnBacklashCoefficients[0] = -2.6799999999999997e+001f;
    _configData.rtnBacklashCoefficients[1] = 3.1428571428571431e-002f;
    _configData.rtnBacklashCoefficients[2] = 1.5873015873015873e-004f;

    // LNG, 16.Aug.2025
    // 0 950
    // 10 1090
    // 20 1180
    // 40 1330
    // 60 1505
    // 70 1630
    _configData.lngCoefficients[0] = 9.5199345532256200e+002f;
    _configData.lngCoefficients[1] = 1.5448059970614935e+001f;
    _configData.lngCoefficients[2] = -2.3899425671160265e-001f;
    _configData.lngCoefficients[3] = 2.2417189795642862e-003f;
    _configData.lngBacklashCoefficients[0] = 0;
  }
  uint8_t read(int idx) {
    return idx < 5 ? _header[idx] : 0;
  }
  void write(int idx, uint8_t val) {
    if (idx < 5) {
      _header[idx] = val;
    }
  }
  void update(int idx, uint8_t val) {
    if (idx < 5) {
      _header[idx] = val;
    }      
  }
  CalibrationData_V1& get(int idx, CalibrationData_V1& config) {
    memcpy(&config, &_configData, sizeof(CalibrationData_V1));
    return config;
  }
  const CalibrationData_V1& put(int idx, const CalibrationData_V1& config) {
    memcpy(&_configData, &config, sizeof(CalibrationData_V1));
    return _configData;
  }
private:
  char _header[5];
  CalibrationData_V1 _configData;
};

class FakeServo {
public:
  void attach(uint8_t pin) {
    _pin = pin;
    std::cout << "FakeServo:attach([" << (short)pin << "])" << std::endl;
  }
  void write(uint16_t angle) const {
    std::cout << "FakeServo[" << _pin << "]:write([" << angle << "])" << std::endl;
  }
  void detach() {
    std::cout << "FakeServo:detach()" << std::endl;
  }
private:
  int _pin;
};

DummySerial logger;
using LogType = Singleton<DummySerial>;
template<> DummySerial& LogType::instance = logger;

DummySerial serial;
DummyBle ble;
DummyEEProm eeprom;

Messages messages;

nssm::SystemMonitorAo<LogType> systemMonitor(messages);
nspls::PulsWidthTransmitterAo<LogType, FakeServo, DummyEEProm> transmitter(messages, eeprom);
nstj::TrajectoryAo<LogType> trajectory(messages);
nsusb::UsbTerminalAo<LogType, DummySerial> usbTerminal(messages, serial);
nsble::BleTerminalAo<LogType, DummyBle> bleTerminal(messages, ble);
nsom::OverloadMonitorAo<LogType> overloadMonitor(messages);
nssvc::ServiceAo<LogType, FakeServo, DummyEEProm> service(messages, eeprom);

void setup() {
}

void loop() {
  usbTerminal.load();
  bleTerminal.load();
  trajectory.load();
  transmitter.load();
  systemMonitor.load();
  overloadMonitor.load();
  service.load();

  usbTerminal.run();
  bleTerminal.run();
  trajectory.run();
  transmitter.run();
  systemMonitor.run();
  overloadMonitor.run();
  service.run();
}

void loop_thread() {
  while (true) {
    loop();
    this_thread::sleep_for(chrono::milliseconds(100));
  }
}

void main() {
  setup();
  thread st(&loop_thread);
  st.join();
}

/*
void move(TrajectoryData data);
void process();

int main() {

  std::cout << "Hello Smart Cards" << std::endl;

  for (int n = 0; n < 25; n++)
  {
    usbTerminal.load();
    bleTerminal.load();
    trajectory.load();
    transmitter.load();
    systemMonitor.load();
    overloadMonitor.load();
    service.load();

    usbTerminal.run();
    bleTerminal.run();
    trajectory.run();
    transmitter.run();
    systemMonitor.run();
    overloadMonitor.run();
    service.run();
  }

  move(TrajectoryData(TrajectoryInfo::ConnectD));
  move(TrajectoryData(TrajectoryInfo::Retract));
  move(TrajectoryData(TrajectoryInfo::ConnectB));

  messages.toTrajectoryQueue.push(TrajectoryData(TrajectoryInfo::Panic).raw);
  process();
}

void move(TrajectoryData data) {
  messages.toTrajectoryQueue.push(data.raw);
  messages.toSystemMonitorQueue.push(SystemStatusData(SystemStatusInfo::Moving).raw);

  process();
}
void process()
{
  for (int n = 0; n < 200; n++)
  {
    usbTerminal.load();
    bleTerminal.load();
    trajectory.load();
    transmitter.load();
    systemMonitor.load();
    overloadMonitor.load();

    usbTerminal.run();
    bleTerminal.run();
    trajectory.run();
    transmitter.run();
    systemMonitor.run();
    overloadMonitor.run();
  }
}
*/

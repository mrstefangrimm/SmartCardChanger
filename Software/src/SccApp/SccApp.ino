#include <Arduino.h>
#include <Servo.h>
#include <HardwareSerial.h>
#include <EEPROM.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "SystemMonitor/SystemMonitorAo.h"
#include "PulsWidthTransmitter/PulsWidthTransmitterAo.h"
#include "Trajectory/TrajectoryAo.h"
#include "UsbTerminal/UsbTerminalAo.h"
#include "BleTerminal/BleTerminalAo.h"
#include "OverloadMonitor/OverloadMonitorAo.h"
#include "Service/ServiceAo.h"
#include "Shared/Messages.h"
#include "Shared/Singleton.h"

void rtassert(bool condition) {
  digitalWrite(13, HIGH);
  if (condition) return;

  while (true) {
    Serial.println(F("rtassert - reboot required"));
    delay(1000);
  }
}

struct FakeLogger {
  void begin(uint16_t a) {}
  template<typename T>
  void print(T ch) {}
  template<typename T>
  void print(T ch, uint8_t mode) {}
  void println() {}
  template<typename T>
  void println(T ch) {}
  template<typename T>
  void println(T ch, uint8_t mode) {}
  operator bool() { return true; }
};
typedef Serial_ Logger_t;  // FakeLogger, HardwareSerial, Serial_
Logger_t logger;
using LogType = Singleton<Logger_t>;
template<> Logger_t& LogType::instance = logger;

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
Messages messages;

nssm::SystemMonitorAo<LogType> systemMonitor(messages);
nspls::PulsWidthTransmitterAo<LogType, Servo, EEPROMClass> transmitter(messages, EEPROM);
nstj::TrajectoryAo<LogType> trajectory(messages);
nsusb::UsbTerminalAo<LogType, Serial_> usbTerminal(messages, Serial);
nsble::BleTerminalAo<LogType, Adafruit_BluefruitLE_SPI> bleTerminal(messages, ble);
nsom::OverloadMonitorAo<LogType> overloadMonitor(messages);
nssvc::ServiceAo<LogType, Servo, EEPROMClass> service(messages, EEPROM);

void setup() {
  while (logger)
    ;
  logger.begin(9600);
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

  /*
  const float ArduinoVoltage = 3.3;
  int acsValueLng = analogRead(A0);
  int acsValueRtn = analogRead(A1);

   if (acsValueLng < 770 || /*acsValueRtn < 810 ||/ acsValueRtn > 820) {
    Serial.print(acsValueLng);
    Serial.print(" Rtn: ");
    Serial.println(acsValueRtn);
  }
  */
  //float acsValueF = (3.95 - (acsValue * (5.0 / 1024.0))) / 0.066;
  //Serial.println(acsValueF);

  delay(100);
}

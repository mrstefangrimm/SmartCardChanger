// Copyright (c) 2025 Stefan Grimm. All rights reserved.
// Licensed under the LGPL. See LICENSE file in the project root for full license information.
//
#ifndef Messages_h
#define Messages_h

#include "ValueQueue.h"

using MessageData_t = uint32_t;

struct Messages {
  ValueQueue<1, MessageData_t> toPulsWidthTransmitterQueue;
  ValueQueue<2, MessageData_t> toTrajectoryQueue;
  ValueQueue<5, MessageData_t> toSystemMonitorQueue;
  ValueQueue<3, MessageData_t> toOverloadMonitorQueue;
  ValueQueue<1, MessageData_t> toServiceQueue;
};

enum class SystemStatusInfo : uint8_t {
  AtPosition,
  Moving,
  Error,
  NeedsCalibration,
  Service
};
struct SystemStatusData {
  SystemStatusData() : raw(0) {}
  explicit SystemStatusData(MessageData_t rawValue) : raw(rawValue) {}
  explicit SystemStatusData(SystemStatusInfo info) :
    atPosition(info == SystemStatusInfo::AtPosition),
    moving(info == SystemStatusInfo::Moving),
    error(info == SystemStatusInfo::Error),
    needsCalibration(info == SystemStatusInfo::NeedsCalibration),
    service(info == SystemStatusInfo::Service) {
  }

  union {
    struct {
      MessageData_t atPosition : 1;
      MessageData_t moving : 1;
      MessageData_t error : 1;
      MessageData_t needsCalibration : 1;
      MessageData_t service : 1;
    };
    MessageData_t raw;
  };
};

struct TargetPosData {
  TargetPosData() : raw(0) {}
  explicit TargetPosData(MessageData_t rawValue) : raw(rawValue) {}
  TargetPosData(uint8_t servoNo, float position, float stepSize) :
    reset(0),
    servoNo(servoNo),
    position(position * 100),
    stepSize(stepSize * 10) {
  }
  explicit TargetPosData(bool reset) :
    reset(reset),
    servoNo(0),
    position(0),
    stepSize(0) {
  }

  union {
    struct {
      MessageData_t reset : 1;
      MessageData_t servoNo : 1;
      MessageData_t position : 16; // [0..65535] (e.g. 655.35 mm)
      MessageData_t stepSize : 8; // [0..255] (e.g. 25.5 deg/tick)
    };
    MessageData_t raw;
  };
};

enum class TrajectoryInfo : uint8_t {
  ConnectA,
  ConnectB,
  ConnectC,
  ConnectD,
  ConnectE,
  Retract,
  Panic,
  Overload
};

struct TrajectoryData {
  TrajectoryData() : raw(0) {}
  explicit TrajectoryData(MessageData_t rawValue) : raw(rawValue) {}
  explicit TrajectoryData(TrajectoryInfo info) :
    connectA(info == TrajectoryInfo::ConnectA),
    connectB(info == TrajectoryInfo::ConnectB),
    connectC(info == TrajectoryInfo::ConnectC),
    connectD(info == TrajectoryInfo::ConnectD),
    connectE(info == TrajectoryInfo::ConnectE),
    retract(info == TrajectoryInfo::Retract),
    panic(info == TrajectoryInfo::Panic),
    overload(info == TrajectoryInfo::Overload) {
  }

  union {
    struct {
      MessageData_t connectA : 1;
      MessageData_t connectB : 1;
      MessageData_t connectC : 1;
      MessageData_t connectD : 1;
      MessageData_t connectE : 1;
      MessageData_t retract : 1;
      MessageData_t panic : 1;
      MessageData_t overload : 1;
    };
    MessageData_t raw;
  };
};

enum class ServiceCmdInfo : uint8_t {
  Enter,
  Print,
  Quit,
  AdjustRtnServoOffset,
  AdjustLngServoOffset,
  AdjustRtnOverloadLower,
  AdjustRtnOverloadUpper,
  AdjustLngOverloadLower,
  AdjustLngOverloadUpper
};

struct ServiceCmdData {
  ServiceCmdData() : raw(0) {}
  explicit ServiceCmdData(MessageData_t rawValue) : raw(rawValue) {}
  explicit ServiceCmdData(ServiceCmdInfo info) :
    info((uint8_t)info) {
  }
  explicit ServiceCmdData(ServiceCmdInfo info, int16_t data) :
    info((uint8_t)info), data(data) {
  }

  union {
    struct {
      MessageData_t info : 8;
      MessageData_t data : 16;
    };
    MessageData_t raw;
  };
};

#endif

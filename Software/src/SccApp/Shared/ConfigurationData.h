#ifndef ConfigurationData_h
#define ConfigurationData_h

struct CalibrationData_V1 {
  float rtnCoefficients[4] = { 0 };
  float rtnBacklashCoefficients[4] = { 0 };
  float lngCoefficients[4] = { 0 };
  float lngBacklashCoefficients[4] = { 0 };

  uint16_t rtnOverflowLowerThreshold = 700;
  uint16_t rtnOverflowUpperThreshold = 1000;
  uint16_t lngOverflowLowerThreshold = 700;
  uint16_t lngOverflowUpperThreshold = 1000;
};

#endif

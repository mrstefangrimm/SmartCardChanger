#ifndef ConfigurationData_h
#define ConfigurationData_h

struct CalibrationData_V1 {
  float rtnCoefficients[4] = { 0 };
  float rtnBacklashCoefficients[4] = { 0 };
  float lngCoefficients[4] = { 0 };
  float lngBacklashCoefficients[4] = { 0 };

  uint16_t rtnOverflowLowerThreshold = 0;
  uint16_t rtnOverflowUpperThreshold = 0;
  uint16_t lngOverflowLowerThreshold = 0;
  uint16_t lngOverflowUpperThreshold = 0;
};

#endif

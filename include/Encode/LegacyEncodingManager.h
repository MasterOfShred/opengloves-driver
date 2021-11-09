#pragma once

#include "Encode/EncodingManager.h"

class LegacyEncodingManager : public EncodingManager {
 public:
  explicit LegacyEncodingManager(float maxAnalogValue);

  VRInputData Decode(std::string input) override;
  std::string Encode(const VRFFBData& input) override;
};
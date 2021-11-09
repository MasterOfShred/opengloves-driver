#include <Encode/AlphaEncodingManager.h>

#include <sstream>

/* Alpha encoding uses the wasted data in the delimiter from legacy to allow for optional arguments and redundancy over smaller packets */
enum class VRCommDataAlphaEncodingCharacter : char {
  FinThumb = 'A',
  FinIndex = 'B',
  FinMiddle = 'C',
  FinRing = 'D',
  FinPinky = 'E',
  JoyX = 'F',
  JoyY = 'G',
  JoyBtn = 'H',
  BtnTrg = 'I',
  BtnA = 'J',
  BtnB = 'K',
  GesGrab = 'L',
  GesPinch = 'M',
  BtnMenu = 'N',
  BtnCalib = 'O',
};

constexpr char VRCommDataAlphaEncodingCharacters[] = {
    static_cast<char>(VRCommDataAlphaEncodingCharacter::FinThumb),
    static_cast<char>(VRCommDataAlphaEncodingCharacter::FinIndex),
    static_cast<char>(VRCommDataAlphaEncodingCharacter::FinMiddle),
    static_cast<char>(VRCommDataAlphaEncodingCharacter::FinRing),
    static_cast<char>(VRCommDataAlphaEncodingCharacter::FinPinky),
    static_cast<char>(VRCommDataAlphaEncodingCharacter::JoyX),
    static_cast<char>(VRCommDataAlphaEncodingCharacter::JoyY),
    static_cast<char>(VRCommDataAlphaEncodingCharacter::JoyBtn),
    static_cast<char>(VRCommDataAlphaEncodingCharacter::BtnTrg),
    static_cast<char>(VRCommDataAlphaEncodingCharacter::BtnA),
    static_cast<char>(VRCommDataAlphaEncodingCharacter::BtnB),
    static_cast<char>(VRCommDataAlphaEncodingCharacter::GesGrab),
    static_cast<char>(VRCommDataAlphaEncodingCharacter::GesPinch),
    static_cast<char>(VRCommDataAlphaEncodingCharacter::BtnMenu),
    static_cast<char>(VRCommDataAlphaEncodingCharacter::BtnCalib),
    static_cast<char>(0)  // Turns into a null terminated string
};

AlphaEncodingManager::AlphaEncodingManager(const float maxAnalogValue) : EncodingManager(maxAnalogValue) {}

VRInputData AlphaEncodingManager::Decode(const std::string input) {
  auto getCharPos = [=](const VRCommDataAlphaEncodingCharacter chr, size_t& pos) {
    pos = input.find(static_cast<char>(chr));

    return pos != std::string::npos;
  };

  auto charExists = [=](const VRCommDataAlphaEncodingCharacter chr) {
    size_t pos;

    return getCharPos(chr, pos);
  };

  auto scalarExists = [=](const VRCommDataAlphaEncodingCharacter chr, float& value) {
    size_t charPos;

    if (const bool exists = getCharPos(chr, charPos); !exists) return false;

    // characters may not necessarily be in order, so end at any letter
    const size_t end = input.find_first_of(VRCommDataAlphaEncodingCharacters, charPos + 1);

    value = stof(input.substr(charPos + 1, end - (charPos + 1)));

    return true;
  };

  float outValue;

  VRInputData inputData(
      std::array<float, 5>{
          scalarExists(VRCommDataAlphaEncodingCharacter::FinThumb, outValue) ? outValue / _maxAnalogValue : -1,
          scalarExists(VRCommDataAlphaEncodingCharacter::FinIndex, outValue) ? outValue / _maxAnalogValue : -1,
          scalarExists(VRCommDataAlphaEncodingCharacter::FinMiddle, outValue) ? outValue / _maxAnalogValue : -1,
          scalarExists(VRCommDataAlphaEncodingCharacter::FinRing, outValue) ? outValue / _maxAnalogValue : -1,
          scalarExists(VRCommDataAlphaEncodingCharacter::FinPinky, outValue) ? outValue / _maxAnalogValue : -1},
      scalarExists(VRCommDataAlphaEncodingCharacter::JoyX, outValue) ? 2 * outValue / _maxAnalogValue - 1 : 0,
      scalarExists(VRCommDataAlphaEncodingCharacter::JoyY, outValue) ? 2 * outValue / _maxAnalogValue - 1 : 0,
      charExists(VRCommDataAlphaEncodingCharacter::JoyBtn),
      charExists(VRCommDataAlphaEncodingCharacter::BtnTrg),
      charExists(VRCommDataAlphaEncodingCharacter::BtnA),
      charExists(VRCommDataAlphaEncodingCharacter::BtnB),
      charExists(VRCommDataAlphaEncodingCharacter::GesGrab),
      charExists(VRCommDataAlphaEncodingCharacter::GesPinch),
      charExists(VRCommDataAlphaEncodingCharacter::BtnMenu),
      charExists(VRCommDataAlphaEncodingCharacter::BtnCalib));

  return inputData;
}

std::string AlphaEncodingManager::Encode(const VRFFBData& input) {
  std::string result = StringFormat(
      "%c%d%c%d%c%d%c%d%c%d\n",
      static_cast<char>(VRCommDataAlphaEncodingCharacter::FinThumb),
      input.thumbCurl,
      static_cast<char>(VRCommDataAlphaEncodingCharacter::FinIndex),
      input.indexCurl,
      static_cast<char>(VRCommDataAlphaEncodingCharacter::FinMiddle),
      input.middleCurl,
      static_cast<char>(VRCommDataAlphaEncodingCharacter::FinRing),
      input.ringCurl,
      static_cast<char>(VRCommDataAlphaEncodingCharacter::FinPinky),
      input.pinkyCurl);

  return result;
}

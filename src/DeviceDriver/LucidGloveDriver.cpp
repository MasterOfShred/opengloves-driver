#include "DeviceDriver/LucidGloveDriver.h"

static const char* c_deviceControllerType = "lucidgloves";
static const char* c_deviceModelNumber = "lucidgloves1";
static const char* c_inputProfilePath = "{openglove}/input/openglove_profile.json";

LucidGloveDeviceDriver::LucidGloveDeviceDriver(
    std::unique_ptr<CommunicationManager> communicationManager,
    std::shared_ptr<BoneAnimator> boneAnimator,
    const std::string& serialNumber,
    const VRDeviceConfiguration configuration)
    : DeviceDriver(std::move(communicationManager), std::move(boneAnimator), serialNumber, configuration), _inputComponentHandles() {}

void LucidGloveDeviceDriver::HandleInput(const VRInputData data) {
  auto updateScalar = [=](LucidGloveDeviceComponentIndex index, const float value) {
    vr::VRDriverInput()->UpdateScalarComponent(_inputComponentHandles[static_cast<int>(index)], value, 0);
  };

  auto updateBool = [=](LucidGloveDeviceComponentIndex index, const bool value) {
    vr::VRDriverInput()->UpdateBooleanComponent(_inputComponentHandles[static_cast<int>(index)], value, 0);
  };

  updateScalar(LucidGloveDeviceComponentIndex::JoyX, data.joyX);
  updateScalar(LucidGloveDeviceComponentIndex::JoyY, data.joyY);

  updateBool(LucidGloveDeviceComponentIndex::JoyBtn, data.joyButton);
  updateBool(LucidGloveDeviceComponentIndex::BtnTrg, data.trgButton);
  updateBool(LucidGloveDeviceComponentIndex::BtnA, data.aButton);
  updateBool(LucidGloveDeviceComponentIndex::BtnB, data.bButton);

  updateBool(LucidGloveDeviceComponentIndex::GesGrab, data.grab);
  updateBool(LucidGloveDeviceComponentIndex::GesPinch, data.pinch);

  updateScalar(LucidGloveDeviceComponentIndex::TrgThumb, data.flexion[0]);
  updateScalar(LucidGloveDeviceComponentIndex::TrgIndex, data.flexion[1]);
  updateScalar(LucidGloveDeviceComponentIndex::TrgMiddle, data.flexion[2]);
  updateScalar(LucidGloveDeviceComponentIndex::TrgRing, data.flexion[3]);
  updateScalar(LucidGloveDeviceComponentIndex::TrgPinky, data.flexion[4]);

  updateBool(LucidGloveDeviceComponentIndex::BtnMenu, data.menu);
}

void LucidGloveDeviceDriver::SetupProps(vr::PropertyContainerHandle_t& props) {
  const bool isRightHand = IsRightHand();

  vr::VRProperties()->SetInt32Property(props, vr::Prop_ControllerHandSelectionPriority_Int32, 2147483647);
  vr::VRProperties()->SetStringProperty(
      props, vr::Prop_InputProfilePath_String, c_inputProfilePath);  // tell OpenVR where to get your driver's Input Profile
  vr::VRProperties()->SetInt32Property(props, vr::Prop_ControllerRoleHint_Int32, _configuration.role);  // tells OpenVR what kind of device this is
  vr::VRProperties()->SetStringProperty(props, vr::Prop_SerialNumber_String, GetSerialNumber().c_str());
  vr::VRProperties()->SetStringProperty(props, vr::Prop_ModelNumber_String, c_deviceModelNumber);
  vr::VRProperties()->SetStringProperty(props, vr::Prop_ManufacturerName_String, c_deviceDriverManufacturer);
  vr::VRProperties()->SetInt32Property(props, vr::Prop_DeviceClass_Int32, vr::TrackedDeviceClass_Controller);
  vr::VRProperties()->SetStringProperty(props, vr::Prop_ControllerType_String, c_deviceControllerType);

  auto createScalar = [=](const char* name, LucidGloveDeviceComponentIndex index, const bool twoSided = false) {
    vr::VRDriverInput()->CreateScalarComponent(
        props,
        name,
        &_inputComponentHandles[static_cast<int>(index)],
        vr::VRScalarType_Absolute,
        twoSided ? vr::VRScalarUnits_NormalizedTwoSided : vr::VRScalarUnits_NormalizedOneSided);
  };

  auto createBool = [=](const char* name, LucidGloveDeviceComponentIndex index) {
    vr::VRDriverInput()->CreateBooleanComponent(props, name, &_inputComponentHandles[static_cast<int>(index)]);
  };

  createScalar("/input/joystick/x", LucidGloveDeviceComponentIndex::JoyX, true);
  createScalar("/input/joystick/y", LucidGloveDeviceComponentIndex::JoyY, true);
  createBool("/input/joystick/click", LucidGloveDeviceComponentIndex::JoyBtn);

  createBool("/input/trigger/click", LucidGloveDeviceComponentIndex::BtnTrg);

  createBool(isRightHand ? "/input/A/click" : "/input/system/click", LucidGloveDeviceComponentIndex::BtnA);
  createBool("/input/B/click", LucidGloveDeviceComponentIndex::BtnB);

  createBool("/input/grab/click", LucidGloveDeviceComponentIndex::GesGrab);
  createBool("/input/pinch/click", LucidGloveDeviceComponentIndex::GesPinch);

  vr::VRDriverInput()->CreateHapticComponent(
      props, "output/haptic", &_inputComponentHandles[static_cast<int>(LucidGloveDeviceComponentIndex::Haptic)]);

  createScalar("/input/finger/thumb", LucidGloveDeviceComponentIndex::TrgThumb);
  createScalar("/input/finger/index", LucidGloveDeviceComponentIndex::TrgIndex);
  createScalar("/input/finger/middle", LucidGloveDeviceComponentIndex::TrgMiddle);
  createScalar("/input/finger/ring", LucidGloveDeviceComponentIndex::TrgRing);
  createScalar("/input/finger/pinky", LucidGloveDeviceComponentIndex::TrgPinky);

  createBool("/input/system/click", LucidGloveDeviceComponentIndex::BtnMenu);
}

void LucidGloveDeviceDriver::StartingDevice() {}

void LucidGloveDeviceDriver::StoppingDevice() {}

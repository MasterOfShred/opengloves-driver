#include "DeviceDriver/KnuckleDriver.h"

#include <utility>

KnuckleDeviceDriver::KnuckleDeviceDriver(
    std::unique_ptr<CommunicationManager> communicationManager,
    std::shared_ptr<BoneAnimator> boneAnimator,
    std::string serialNumber,
    const VRDeviceConfiguration configuration)
    : DeviceDriver(std::move(communicationManager), std::move(boneAnimator), std::move(serialNumber), configuration),
      inputComponentHandles_(),
      haptic_() {}

void KnuckleDeviceDriver::HandleInput(const VRInputData data) {
  // clang-format off
  vr::VRDriverInput()->UpdateScalarComponent(inputComponentHandles_[static_cast<int>(KnuckleDeviceComponentIndex::ThumbstickX)], data.joyX, 0);
  vr::VRDriverInput()->UpdateScalarComponent(inputComponentHandles_[static_cast<int>(KnuckleDeviceComponentIndex::ThumbstickY)], data.joyY, 0);

  vr::VRDriverInput()->UpdateBooleanComponent(inputComponentHandles_[static_cast<int>(KnuckleDeviceComponentIndex::ThumbstickClick)], data.joyButton, 0);
  vr::VRDriverInput()->UpdateBooleanComponent(inputComponentHandles_[static_cast<int>(KnuckleDeviceComponentIndex::ThumbstickTouch)], data.joyButton, 0);

  vr::VRDriverInput()->UpdateBooleanComponent(inputComponentHandles_[static_cast<int>(KnuckleDeviceComponentIndex::TriggerClick)], data.trgButton, 0);
  vr::VRDriverInput()->UpdateScalarComponent(inputComponentHandles_[static_cast<int>(KnuckleDeviceComponentIndex::TriggerValue)], data.flexion[1], 0);

  vr::VRDriverInput()->UpdateBooleanComponent(inputComponentHandles_[static_cast<int>(KnuckleDeviceComponentIndex::AClick)], data.aButton, 0);
  vr::VRDriverInput()->UpdateBooleanComponent(inputComponentHandles_[static_cast<int>(KnuckleDeviceComponentIndex::ATouch)], data.aButton, 0);

  vr::VRDriverInput()->UpdateBooleanComponent(inputComponentHandles_[static_cast<int>(KnuckleDeviceComponentIndex::BClick)], data.bButton, 0);
  vr::VRDriverInput()->UpdateBooleanComponent(inputComponentHandles_[static_cast<int>(KnuckleDeviceComponentIndex::BTouch)], data.bButton, 0);

  vr::VRDriverInput()->UpdateBooleanComponent(inputComponentHandles_[static_cast<int>(KnuckleDeviceComponentIndex::GripForce)], data.grab, 0);
  vr::VRDriverInput()->UpdateBooleanComponent(inputComponentHandles_[static_cast<int>(KnuckleDeviceComponentIndex::GripTouch)], data.grab, 0);
  vr::VRDriverInput()->UpdateScalarComponent(inputComponentHandles_[static_cast<int>(KnuckleDeviceComponentIndex::GripValue)], data.grab, 0);

  vr::VRDriverInput()->UpdateBooleanComponent(inputComponentHandles_[static_cast<int>(KnuckleDeviceComponentIndex::SystemClick)], data.menu, 0);

  // We don't have a thumb on the index
  // vr::VRDriverInput()->UpdateScalarComponent(inputComponentHandles_[(int)KnuckleDeviceComponentIndex::THUMB], data.flexion[0], 0);
  vr::VRDriverInput()->UpdateScalarComponent(inputComponentHandles_[static_cast<int>(KnuckleDeviceComponentIndex::FingerIndex)], data.flexion[1], 0);
  vr::VRDriverInput()->UpdateScalarComponent(inputComponentHandles_[static_cast<int>(KnuckleDeviceComponentIndex::FingerMiddle)], data.flexion[2], 0);
  vr::VRDriverInput()->UpdateScalarComponent(inputComponentHandles_[static_cast<int>(KnuckleDeviceComponentIndex::FingerRing)], data.flexion[3], 0);
  vr::VRDriverInput()->UpdateScalarComponent(inputComponentHandles_[static_cast<int>(KnuckleDeviceComponentIndex::FingerPinky)], data.flexion[4], 0);
  // clang-format on
}

void KnuckleDeviceDriver::SetupProps(vr::PropertyContainerHandle_t& props) {
  // clang-format off
  vr::VRProperties()->SetInt32Property(props, vr::Prop_ControllerHandSelectionPriority_Int32, 2147483647);
  vr::VRProperties()->SetStringProperty(props, vr::Prop_SerialNumber_String, GetSerialNumber().c_str());
  vr::VRProperties()->SetBoolProperty(props, vr::Prop_WillDriftInYaw_Bool, false);
  vr::VRProperties()->SetBoolProperty(props, vr::Prop_DeviceIsWireless_Bool, true);
  vr::VRProperties()->SetBoolProperty(props, vr::Prop_DeviceIsCharging_Bool, false);
  vr::VRProperties()->SetFloatProperty(props, vr::Prop_DeviceBatteryPercentage_Float, 1.f);  // Always charged

  vr::HmdMatrix34_t l_matrix = {-1.f, 0.f, 0.f, 0.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f};
  vr::VRProperties()->SetProperty(props, vr::Prop_StatusDisplayTransform_Matrix34, &l_matrix, sizeof(vr::HmdMatrix34_t), vr::k_unHmdMatrix34PropertyTag);

  vr::VRProperties()->SetBoolProperty(props, vr::Prop_Firmware_UpdateAvailable_Bool, false);
  vr::VRProperties()->SetBoolProperty(props, vr::Prop_Firmware_ManualUpdate_Bool, false);
  vr::VRProperties()->SetStringProperty(props, vr::Prop_Firmware_ManualUpdateURL_String, "https://developer.valvesoftware.com/wiki/SteamVR/HowTo_Update_Firmware");
  vr::VRProperties()->SetBoolProperty(props, vr::Prop_DeviceProvidesBatteryStatus_Bool, true);
  vr::VRProperties()->SetBoolProperty(props, vr::Prop_DeviceCanPowerOff_Bool, false);
  vr::VRProperties()->SetInt32Property(props, vr::Prop_DeviceClass_Int32, vr::TrackedDeviceClass_Controller);
  vr::VRProperties()->SetBoolProperty(props, vr::Prop_Firmware_ForceUpdateRequired_Bool, false);
  vr::VRProperties()->SetBoolProperty(props, vr::Prop_Identifiable_Bool, true);
  vr::VRProperties()->SetBoolProperty(props, vr::Prop_Firmware_RemindUpdate_Bool, false);
  vr::VRProperties()->SetInt32Property(props, vr::Prop_Axis0Type_Int32, vr::k_eControllerAxis_TrackPad);
  vr::VRProperties()->SetInt32Property(props, vr::Prop_Axis1Type_Int32, vr::k_eControllerAxis_Trigger);
  vr::VRProperties()->SetInt32Property(props, vr::Prop_ControllerRoleHint_Int32, IsRightHand() ? vr::TrackedControllerRole_RightHand : vr::TrackedControllerRole_LeftHand);
  vr::VRProperties()->SetBoolProperty(props, vr::Prop_HasDisplayComponent_Bool, false);
  vr::VRProperties()->SetBoolProperty(props, vr::Prop_HasCameraComponent_Bool, false);
  vr::VRProperties()->SetBoolProperty(props, vr::Prop_HasDriverDirectModeComponent_Bool, false);
  vr::VRProperties()->SetBoolProperty(props, vr::Prop_HasVirtualDisplayComponent_Bool, false);
  vr::VRProperties()->SetInt32Property(props, vr::Prop_ControllerHandSelectionPriority_Int32, 2147483647);
  vr::VRProperties()->SetStringProperty(props, vr::Prop_ModelNumber_String, IsRightHand() ? "Knuckles Right" : "Knuckles Left");
  vr::VRProperties()->SetStringProperty(props, vr::Prop_RenderModelName_String, IsRightHand() ? "{indexcontroller}valve_controller_knu_1_0_right" : "{indexcontroller}valve_controller_knu_1_0_left");
  vr::VRProperties()->SetStringProperty(props, vr::Prop_ManufacturerName_String, c_deviceDriverManufacturer);
  vr::VRProperties()->SetStringProperty(props, vr::Prop_TrackingFirmwareVersion_String, "1562916277 watchman@ValveBuilder02 2019-07-12 FPGA 538(2.26/10/2) BL 0 VRC 1562916277 Radio 1562882729");
  vr::VRProperties()->SetStringProperty(props, vr::Prop_HardwareRevision_String, "product 17 rev 14.1.9 lot 2019/4/20 0");
  vr::VRProperties()->SetStringProperty(props, vr::Prop_ConnectedWirelessDongle_String, "C2F75F5986-DIY");
  vr::VRProperties()->SetUint64Property(props, vr::Prop_HardwareRevision_Uint64, 286130441U);
  vr::VRProperties()->SetUint64Property(props, vr::Prop_FirmwareVersion_Uint64, 1562916277U);
  vr::VRProperties()->SetUint64Property(props, vr::Prop_FPGAVersion_Uint64, 538U);
  vr::VRProperties()->SetUint64Property(props, vr::Prop_VRCVersion_Uint64, 1562916277U);
  vr::VRProperties()->SetUint64Property(props, vr::Prop_RadioVersion_Uint64, 1562882729U);
  vr::VRProperties()->SetUint64Property(props, vr::Prop_DongleVersion_Uint64, 1558748372U);
  vr::VRProperties()->SetStringProperty(props, vr::Prop_Firmware_ProgrammingTarget_String, IsRightHand() ? "LHR-E217CD01" : "LHR-E217CD00");
  vr::VRProperties()->SetStringProperty(props, vr::Prop_ResourceRoot_String, "indexcontroller");
  vr::VRProperties()->SetStringProperty(props, vr::Prop_RegisteredDeviceType_String, IsRightHand() ? "valve/index_controllerLHR-E217CD01" : "valve/index_controllerLHR-E217CD00");
  vr::VRProperties()->SetStringProperty(props, vr::Prop_InputProfilePath_String, "{indexcontroller}/input/index_controller_profile.json");
  vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceOff_String, IsRightHand() ? "{openglove}/icons/right_controller_status_off.png" : "{openglove}/icons/left_controller_status_off.png");
  vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearching_String, IsRightHand() ? "{openglove}/icons/right_controller_status_searching.gif" : "{openglove}/icons/left_controller_status_searching.gif");
  vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearchingAlert_String, IsRightHand() ? "{openglove}/icons/right_controller_status_searching_alert.gif" : "{openglove}/icons/left_controller_status_searching_alert.gif");
  vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReady_String, IsRightHand() ? "{openglove}/icons/right_controller_status_ready.png" : "{openglove}/icons/left_controller_status_ready.png");
  vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReadyAlert_String, IsRightHand() ? "{openglove}/icons/right_controller_status_ready_alert.png" : "{openglove}/icons/left_controller_status_ready_alert.png");
  vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceNotReady_String, IsRightHand() ? "{openglove}/icons/right_controller_status_error.png" : "{openglove}/icons/left_controller_status_error.png");
  vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceStandby_String, IsRightHand() ? "{openglove}/icons/right_controller_status_off.png" : "{openglove}/icons/left_controller_status_off.png");
  vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceAlertLow_String, IsRightHand() ? "{openglove}/icons/right_controller_status_ready_low.png" : "{openglove}/icons/left_controller_status_ready_low.png");

  vr::VRProperties()->SetInt32Property(props, vr::Prop_Axis2Type_Int32, vr::k_eControllerAxis_Trigger);
  vr::VRProperties()->SetStringProperty(props, vr::Prop_ControllerType_String, "knuckles");

  auto createScalar = [=](const char* name, KnuckleDeviceComponentIndex index, const bool twoSided = false) {
    vr::VRDriverInput()->CreateScalarComponent(
        props,
        name,
        &inputComponentHandles_[static_cast<int>(index)],
        vr::VRScalarType_Absolute,
        (twoSided ? vr::VRScalarUnits_NormalizedTwoSided : vr::VRScalarUnits_NormalizedOneSided));
  };

  auto createBool = [=](const char* name, KnuckleDeviceComponentIndex index) {
    vr::VRDriverInput()->CreateBooleanComponent(props, name, &inputComponentHandles_[static_cast<int>(index)]);
  };

  createBool("/input/system/click", KnuckleDeviceComponentIndex::SystemClick);
  createBool("/input/system/touch", KnuckleDeviceComponentIndex::SystemTouch);
  createBool("/input/trigger/click", KnuckleDeviceComponentIndex::TriggerClick);
  createScalar("/input/trigger/value", KnuckleDeviceComponentIndex::TriggerValue);
  createScalar("/input/trackpad/x", KnuckleDeviceComponentIndex::TrackpadX, true);
  createScalar("/input/trackpad/y", KnuckleDeviceComponentIndex::TrackpadY, true);
  createBool("/input/trackpad/touch", KnuckleDeviceComponentIndex::TrackpadTouch);
  createScalar("/input/trackpad/force", KnuckleDeviceComponentIndex::TrackpadForce);
  createBool("/input/grip/touch", KnuckleDeviceComponentIndex::GripTouch);
  createScalar("/input/grip/force", KnuckleDeviceComponentIndex::GripForce);
  createScalar("/input/grip/value", KnuckleDeviceComponentIndex::GripValue);
  createBool("/input/thumbstick/click", KnuckleDeviceComponentIndex::ThumbstickClick);
  createBool("/input/thumbstick/touch", KnuckleDeviceComponentIndex::ThumbstickTouch);
  createScalar("/input/thumbstick/x", KnuckleDeviceComponentIndex::ThumbstickX, true);
  createScalar("/input/thumbstick/y", KnuckleDeviceComponentIndex::ThumbstickY, true);
  createBool("/input/a/click", KnuckleDeviceComponentIndex::AClick);
  createBool("/input/a/touch", KnuckleDeviceComponentIndex::ATouch);
  createBool("/input/b/click", KnuckleDeviceComponentIndex::BClick);
  createBool("/input/b/touch", KnuckleDeviceComponentIndex::BTouch);
  createScalar("/input/finger/index", KnuckleDeviceComponentIndex::FingerIndex);
  createScalar("/input/finger/middle", KnuckleDeviceComponentIndex::FingerMiddle);
  createScalar("/input/finger/ring", KnuckleDeviceComponentIndex::FingerRing);
  createScalar("/input/finger/pinky", KnuckleDeviceComponentIndex::FingerPinky);
  vr::VRDriverInput()->CreateHapticComponent(props, "/output/haptic", &haptic_);
  // clang-format on
}

void KnuckleDeviceDriver::StartingDevice() {
  if (!configuration_.feedbackEnabled) return;

  ffbProvider_ = std::make_unique<FFBListener>(
      [&](const VRFFBData data) {
        // Queue the force feedback data for sending.
        communicationManager_->QueueSend(data);
      },
      configuration_.role);

  ffbProvider_->Start();
}

void KnuckleDeviceDriver::StoppingDevice() {
  if (ffbProvider_) {
    ffbProvider_->Stop();
  }
}

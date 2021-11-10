#include "DeviceDriver/DeviceDriver.h"

#include <utility>

#include "DriverLog.h"

DeviceDriver::DeviceDriver(
    std::unique_ptr<CommunicationManager> communicationManager,
    std::shared_ptr<BoneAnimator> boneAnimator,
    std::string serialNumber,
    const VRDeviceConfiguration configuration)
    : communicationManager_(std::move(communicationManager)),
      boneAnimator_(std::move(boneAnimator)),
      configuration_(configuration),
      serialNumber_(std::move(serialNumber)),
      skeletalComponentHandle_(),
      handTransforms_(),
      hasActivated_(false),
      driverId_(vr::k_unTrackedDeviceIndexInvalid) {
  // copy a default bone transform to our hand transform for use in finger positioning later
  std::copy(
      std::begin(IsRightHand() ? rightOpenPose : leftOpenPose), std::end(IsRightHand() ? rightOpenPose : leftOpenPose), std::begin(handTransforms_));
}

vr::EVRInitError DeviceDriver::Activate(uint32_t unObjectId) {
  driverId_ = unObjectId;
  controllerPose_ = std::make_unique<ControllerPose>(configuration_.role, std::string(c_deviceDriverManufacturer), configuration_.poseConfiguration);

  vr::PropertyContainerHandle_t props = vr::VRProperties()->TrackedDeviceToPropertyContainer(
      driverId_);  // this gets a container object where you store all the information about your driver

  SetupProps(props);

  if (const vr::EVRInputError error = vr::VRDriverInput()->CreateSkeletonComponent(
          props,
          IsRightHand() ? "/input/skeleton/right" : "/input/skeleton/left",
          IsRightHand() ? "/skeleton/hand/right" : "/skeleton/hand/left",
          "/pose/raw",
          vr::VRSkeletalTracking_Partial,
          handTransforms_,
          NUM_BONES,
          &skeletalComponentHandle_);
      error != vr::VRInputError_None) {
    DebugDriverLog("CreateSkeletonComponent failed.  Error: %s\n", error);
  }

  StartDevice();

  hasActivated_ = true;

  return vr::VRInitError_None;
}

void DeviceDriver::Deactivate() {
  if (hasActivated_) {
    StoppingDevice();
    communicationManager_->Disconnect();
    driverId_ = vr::k_unTrackedDeviceIndexInvalid;
    hasActivated_ = false;
  }
}

void DeviceDriver::DebugRequest(const char* pchRequest, char* pchResponseBuffer, const uint32_t unResponseBufferSize) {
  if (unResponseBufferSize >= 1) pchResponseBuffer[0] = 0;
}

void DeviceDriver::EnterStandby() {}

void* DeviceDriver::GetComponent(const char* pchComponentNameAndVersion) {
  return nullptr;
}

vr::DriverPose_t DeviceDriver::GetPose() {
  if (hasActivated_) return controllerPose_->UpdatePose();

  return vr::DriverPose_t{0};
}

std::string DeviceDriver::GetSerialNumber() {
  return serialNumber_;
}

bool DeviceDriver::IsActive() {
  return hasActivated_;
}

void DeviceDriver::RunFrame() {
  if (hasActivated_) {
    vr::VRServerDriverHost()->TrackedDevicePoseUpdated(driverId_, controllerPose_->UpdatePose(), sizeof(vr::DriverPose_t));
  }
}

bool DeviceDriver::IsRightHand() const {
  return configuration_.role == vr::TrackedControllerRole_RightHand;
}

void DeviceDriver::StartDevice() {
  StartingDevice();

  vr::VRDriverInput()->UpdateSkeletonComponent(
      skeletalComponentHandle_, vr::VRSkeletalMotionRange_WithoutController, IsRightHand() ? rightOpenPose : leftOpenPose, NUM_BONES);
  vr::VRDriverInput()->UpdateSkeletonComponent(
      skeletalComponentHandle_, vr::VRSkeletalMotionRange_WithController, IsRightHand() ? rightOpenPose : leftOpenPose, NUM_BONES);

  communicationManager_->BeginListener([&](VRInputData data) {
    try {
      boneAnimator_->ComputeSkeletonTransforms(handTransforms_, data.flexion, IsRightHand());
      vr::VRDriverInput()->UpdateSkeletonComponent(skeletalComponentHandle_, vr::VRSkeletalMotionRange_WithoutController, handTransforms_, NUM_BONES);
      vr::VRDriverInput()->UpdateSkeletonComponent(skeletalComponentHandle_, vr::VRSkeletalMotionRange_WithController, handTransforms_, NUM_BONES);

      HandleInput(data);

      if (configuration_.poseConfiguration.calibrationButtonEnabled) {
        if (data.calibrate) {
          if (!controllerPose_->IsCalibrating()) controllerPose_->StartCalibration(CalibrationMethod::Hardware);
        } else {
          if (controllerPose_->IsCalibrating()) controllerPose_->CompleteCalibration(CalibrationMethod::Hardware);
        }
      }

    } catch (const std::exception&) {
      DebugDriverLog("Exception caught while parsing comm data");
    }
  });
}

#pragma once
#include <openvr.h>
#include <Windows.h>

struct ControllerPipeData {
  vr::TrackedDeviceIndex_t controllerId;
};

class PipeHelper {
 public:
  PipeHelper();

  bool ConnectAndSendPipe(const std::string& pipeName, ControllerPipeData data);

 private:
  HANDLE _pipeHandle;
};
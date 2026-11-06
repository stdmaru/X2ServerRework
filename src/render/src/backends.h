#pragma once

// The backend factories behind Device::Create (src/render/src/render.cpp).

#include "x2/render/render.h"

namespace x2::render::detail {

std::unique_ptr<Device> CreateNullDevice(const DeviceOptions& options, std::string& error);
std::unique_ptr<Device> CreateSdlGpuDevice(const DeviceOptions& options, std::string& error);

}  // namespace x2::render::detail

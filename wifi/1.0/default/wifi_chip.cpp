/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "wifi_chip.h"

#include <android-base/logging.h>

#include "failure_reason_util.h"

namespace android {
namespace hardware {
namespace wifi {
namespace V1_0 {
namespace implementation {

WifiChip::WifiChip(std::weak_ptr<WifiLegacyHal> legacy_hal)
    : legacy_hal_(legacy_hal) {}

void WifiChip::invalidate() {
  legacy_hal_.reset();
  callbacks_.clear();
}

Return<void> WifiChip::registerEventCallback(
    const sp<IWifiChipEventCallback>& callback) {
  if (!legacy_hal_.lock())
    return Void();
  // TODO(b/31632518): remove the callback when the client is destroyed
  callbacks_.insert(callback);
  return Void();
}

Return<void> WifiChip::getAvailableModes(getAvailableModes_cb cb) {
  if (!legacy_hal_.lock()) {
    cb(hidl_vec<ChipMode>());
    return Void();
  } else {
    // TODO add implementation
    return Void();
  }
}

Return<void> WifiChip::configureChip(uint32_t /*mode_id*/) {
  if (!legacy_hal_.lock())
    return Void();
  // TODO add implementation
  return Void();
}

Return<uint32_t> WifiChip::getMode() {
  if (!legacy_hal_.lock())
    return 0;
  // TODO add implementation
  return 0;
}

Return<void> WifiChip::requestChipDebugInfo() {
  if (!legacy_hal_.lock())
    return Void();
  // TODO add implementation
  return Void();
}

Return<void> WifiChip::requestDriverDebugDump() {
  // TODO implement
  return Void();
}

Return<void> WifiChip::requestFirmwareDebugDump() {
  // TODO implement
  return Void();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace wifi
}  // namespace hardware
}  // namespace android

/*
 * Copyright (C) 2024 The Android Open Source Project
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

#pragma once

#include <aidl/android/hardware/biometrics/face/virtualhal/BnVirtualHal.h>

#include "Face.h"

namespace aidl::android::hardware::biometrics::face {
using namespace virtualhal;
class VirtualHal : public BnVirtualHal {
  public:
    VirtualHal(std::shared_ptr<Face> fp) : mFp(fp) {}

    ::ndk::ScopedAStatus setEnrollments(const std::vector<int32_t>& in_id) override;
    ::ndk::ScopedAStatus setEnrollmentHit(int32_t in_hit_id) override;
    ::ndk::ScopedAStatus setNextEnrollment(
            const ::aidl::android::hardware::biometrics::face::NextEnrollment& in_next_enrollment)
            override;
    ::ndk::ScopedAStatus setAuthenticatorId(int64_t in_id) override;
    ::ndk::ScopedAStatus setChallenge(int64_t in_challenge) override;
    ::ndk::ScopedAStatus setOperationAuthenticateFails(bool in_fail) override;
    ::ndk::ScopedAStatus setOperationAuthenticateLatency(
            const std::vector<int32_t>& in_latency) override;
    ::ndk::ScopedAStatus setOperationAuthenticateDuration(int32_t in_duration) override;
    ::ndk::ScopedAStatus setOperationAuthenticateError(int32_t in_error) override;
    ::ndk::ScopedAStatus setOperationAuthenticateAcquired(
            const std::vector<AcquiredInfoAndVendorCode>& in_acquired) override;
    ::ndk::ScopedAStatus setOperationEnrollLatency(const std::vector<int32_t>& in_latency) override;
    ::ndk::ScopedAStatus setOperationDetectInteractionLatency(
            const std::vector<int32_t>& in_latency) override;
    ::ndk::ScopedAStatus setOperationDetectInteractionFails(bool in_fails) override;
    ::ndk::ScopedAStatus setLockout(bool in_lockout) override;
    ::ndk::ScopedAStatus setLockoutEnable(bool in_enable) override;
    ::ndk::ScopedAStatus setLockoutTimedEnable(bool in_enable) override;
    ::ndk::ScopedAStatus setLockoutTimedThreshold(int32_t in_threshold) override;
    ::ndk::ScopedAStatus setLockoutTimedDuration(int32_t in_duration) override;
    ::ndk::ScopedAStatus setLockoutPermanentThreshold(int32_t in_threshold) override;
    ::ndk::ScopedAStatus resetConfigurations() override;
    ::ndk::ScopedAStatus setType(
            ::aidl::android::hardware::biometrics::face::FaceSensorType in_type) override;
    ::ndk::ScopedAStatus setSensorStrength(common::SensorStrength in_strength) override;
    ::ndk::ScopedAStatus getFaceHal(std::shared_ptr<IFace>* _aidl_return);

  private:
    OptIntVec intVec2OptIntVec(const std::vector<int32_t>& intVec);
    OptIntVec acquiredInfoVec2OptIntVec(const std::vector<AcquiredInfoAndVendorCode>& intVec);
    ::ndk::ScopedAStatus sanityCheckLatency(const std::vector<int32_t>& in_latency);
    std::shared_ptr<Face> mFp;
};

}  // namespace aidl::android::hardware::biometrics::face

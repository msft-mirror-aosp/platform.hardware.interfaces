/*
 * Copyright (C) 2023 The Android Open Source Project
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

#include <atomic>
#include <mutex>
#include <vector>

#include "core-impl/Stream.h"
#include "r_submix/SubmixRoute.h"

namespace aidl::android::hardware::audio::core {

class StreamRemoteSubmix : public StreamCommonImpl {
  public:
    StreamRemoteSubmix(StreamContext* context, const Metadata& metadata);
    ~StreamRemoteSubmix();

    // Methods of 'DriverInterface'.
    ::android::status_t init() override;
    ::android::status_t drain(StreamDescriptor::DrainMode) override;
    ::android::status_t flush() override;
    ::android::status_t pause() override;
    ::android::status_t standby() override;
    ::android::status_t start() override;
    ::android::status_t transfer(void* buffer, size_t frameCount, size_t* actualFrameCount,
                                 int32_t* latencyMs) override;
    ::android::status_t refinePosition(StreamDescriptor::Position* position) override;
    void shutdown() override;

    // Overridden methods of 'StreamCommonImpl', called on a Binder thread.
    ndk::ScopedAStatus prepareToClose() override;
    ndk::ScopedAStatus setConnectedDevices(const ConnectedDevices& devices) override;

  private:
    long getDelayInUsForFrameCount(size_t frameCount);
    ::aidl::android::media::audio::common::AudioDeviceAddress getDeviceAddress() const {
        std::lock_guard guard(mLock);
        return mDeviceAddress;
    }
    size_t getStreamPipeSizeInFrames();
    ::android::status_t inRead(void* buffer, size_t frameCount, size_t* actualFrameCount);
    ::android::status_t outWrite(void* buffer, size_t frameCount, size_t* actualFrameCount);
    ::android::status_t setCurrentRoute();

    // Limit for the number of error log entries to avoid spamming the logs.
    static constexpr int kMaxErrorLogs = 5;
    // The duration of kMaxReadFailureAttempts * READ_ATTEMPT_SLEEP_MS must be strictly inferior
    // to the duration of a record buffer at the current record sample rate (of the device, not of
    // the recording itself). Here we have: 3 * 5ms = 15ms < 1024 frames * 1000 / 48000 = 21.333ms
    static constexpr int kMaxReadFailureAttempts = 3;
    // 5ms between two read attempts when pipe is empty
    static constexpr int kReadAttemptSleepUs = 5000;

    const bool mIsInput;
    const r_submix::AudioConfig mStreamConfig;

    mutable std::mutex mLock;
    ::aidl::android::media::audio::common::AudioDeviceAddress mDeviceAddress GUARDED_BY(mLock);
    std::atomic<bool> mDeviceAddressUpdated = false;

    // Used by the worker thread only.
    std::shared_ptr<r_submix::SubmixRoute> mCurrentRoute = nullptr;
    int64_t mStartTimeNs = 0;
    long mFramesSinceStart = 0;
    int mReadErrorCount = 0;
    int mReadFailureCount = 0;
    int mWriteShutdownCount = 0;
};

class StreamInRemoteSubmix final : public StreamIn, public StreamRemoteSubmix {
  public:
    friend class ndk::SharedRefBase;
    StreamInRemoteSubmix(
            StreamContext&& context,
            const ::aidl::android::hardware::audio::common::SinkMetadata& sinkMetadata,
            const std::vector<::aidl::android::media::audio::common::MicrophoneInfo>& microphones);

  private:
    void onClose(StreamDescriptor::State) override { defaultOnClose(); }
    ndk::ScopedAStatus getActiveMicrophones(
            std::vector<::aidl::android::media::audio::common::MicrophoneDynamicInfo>* _aidl_return)
            override;
};

class StreamOutRemoteSubmix final : public StreamOut, public StreamRemoteSubmix {
  public:
    friend class ndk::SharedRefBase;
    StreamOutRemoteSubmix(
            StreamContext&& context,
            const ::aidl::android::hardware::audio::common::SourceMetadata& sourceMetadata,
            const std::optional<::aidl::android::media::audio::common::AudioOffloadInfo>&
                    offloadInfo);

  private:
    void onClose(StreamDescriptor::State) override { defaultOnClose(); }
};

}  // namespace aidl::android::hardware::audio::core

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

#include <vector>

#define LOG_TAG "AHAL_ModulePrimary"
#include <Utils.h>
#include <android-base/logging.h>

#include "core-impl/ModulePrimary.h"
#include "core-impl/StreamOffloadStub.h"
#include "core-impl/StreamPrimary.h"
#include "core-impl/Telephony.h"

using aidl::android::hardware::audio::common::areAllBitPositionFlagsSet;
using aidl::android::hardware::audio::common::SinkMetadata;
using aidl::android::hardware::audio::common::SourceMetadata;
using aidl::android::media::audio::common::AudioIoFlags;
using aidl::android::media::audio::common::AudioOffloadInfo;
using aidl::android::media::audio::common::AudioOutputFlags;
using aidl::android::media::audio::common::AudioPort;
using aidl::android::media::audio::common::AudioPortConfig;
using aidl::android::media::audio::common::MicrophoneInfo;

namespace aidl::android::hardware::audio::core {

ndk::ScopedAStatus ModulePrimary::getTelephony(std::shared_ptr<ITelephony>* _aidl_return) {
    if (!mTelephony) {
        mTelephony = ndk::SharedRefBase::make<Telephony>();
    }
    *_aidl_return = mTelephony.getInstance();
    LOG(DEBUG) << __func__
               << ": returning instance of ITelephony: " << _aidl_return->get()->asBinder().get();
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus ModulePrimary::calculateBufferSizeFrames(
        const ::aidl::android::media::audio::common::AudioFormatDescription& format,
        int32_t latencyMs, int32_t sampleRateHz, int32_t* bufferSizeFrames) {
    if (format.type != ::aidl::android::media::audio::common::AudioFormatType::PCM &&
        StreamOffloadStub::getSupportedEncodings().count(format.encoding)) {
        *bufferSizeFrames = sampleRateHz / 2;  // 1/2 of a second.
        return ndk::ScopedAStatus::ok();
    }
    return Module::calculateBufferSizeFrames(format, latencyMs, sampleRateHz, bufferSizeFrames);
}

ndk::ScopedAStatus ModulePrimary::createInputStream(StreamContext&& context,
                                                    const SinkMetadata& sinkMetadata,
                                                    const std::vector<MicrophoneInfo>& microphones,
                                                    std::shared_ptr<StreamIn>* result) {
    return createStreamInstance<StreamInPrimary>(result, std::move(context), sinkMetadata,
                                                 microphones);
}

ndk::ScopedAStatus ModulePrimary::createOutputStream(
        StreamContext&& context, const SourceMetadata& sourceMetadata,
        const std::optional<AudioOffloadInfo>& offloadInfo, std::shared_ptr<StreamOut>* result) {
    if (!areAllBitPositionFlagsSet(
                context.getFlags().get<AudioIoFlags::output>(),
                {AudioOutputFlags::COMPRESS_OFFLOAD, AudioOutputFlags::NON_BLOCKING})) {
        return createStreamInstance<StreamOutPrimary>(result, std::move(context), sourceMetadata,
                                                      offloadInfo);
    } else {
        // "Stub" is used because there is no actual decoder. The stream just
        // extracts the clip duration from the media file header and simulates
        // playback over time.
        return createStreamInstance<StreamOutOffloadStub>(result, std::move(context),
                                                          sourceMetadata, offloadInfo);
    }
}

int32_t ModulePrimary::getNominalLatencyMs(const AudioPortConfig&) {
    // 85 ms is chosen considering 4096 frames @ 48 kHz. This is the value which allows
    // the virtual Android device implementation to pass CTS. Hardware implementations
    // should have significantly lower latency.
    static constexpr int32_t kLatencyMs = 85;
    return kLatencyMs;
}

}  // namespace aidl::android::hardware::audio::core

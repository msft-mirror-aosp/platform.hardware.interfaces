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

#include "core-impl/DriverStubImpl.h"
#include "core-impl/Stream.h"

namespace aidl::android::hardware::audio::core {

class StreamStub : public StreamCommonImpl, public DriverStubImpl {
  public:
    StreamStub(StreamContext* context, const Metadata& metadata);
    ~StreamStub();
};

class StreamInStub final : public StreamIn, public StreamStub {
  public:
    friend class ndk::SharedRefBase;
    StreamInStub(
            StreamContext&& context,
            const ::aidl::android::hardware::audio::common::SinkMetadata& sinkMetadata,
            const std::vector<::aidl::android::media::audio::common::MicrophoneInfo>& microphones);

  private:
    void onClose(StreamDescriptor::State) override { defaultOnClose(); }
};

class StreamOutStub final : public StreamOut, public StreamStub {
  public:
    friend class ndk::SharedRefBase;
    StreamOutStub(StreamContext&& context,
                  const ::aidl::android::hardware::audio::common::SourceMetadata& sourceMetadata,
                  const std::optional<::aidl::android::media::audio::common::AudioOffloadInfo>&
                          offloadInfo);

  private:
    void onClose(StreamDescriptor::State) override { defaultOnClose(); }
};

}  // namespace aidl::android::hardware::audio::core

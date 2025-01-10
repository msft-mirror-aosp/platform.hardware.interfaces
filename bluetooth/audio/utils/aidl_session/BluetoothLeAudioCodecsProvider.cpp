/*
 * Copyright (C) 2022 The Android Open Source Project
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

#include <optional>
#include <set>

#include "aidl/android/hardware/bluetooth/audio/ChannelMode.h"
#include "aidl/android/hardware/bluetooth/audio/CodecId.h"
#include "aidl/android/hardware/bluetooth/audio/CodecInfo.h"
#include "aidl/android/hardware/bluetooth/audio/ConfigurationFlags.h"
#include "aidl_android_hardware_bluetooth_audio_setting_enums.h"
#define LOG_TAG "BTAudioCodecsProviderAidl"

#include "BluetoothLeAudioCodecsProvider.h"

namespace aidl {
namespace android {
namespace hardware {
namespace bluetooth {
namespace audio {

static const char* kLeAudioCodecCapabilitiesFile =
    "/vendor/etc/le_audio_codec_capabilities.xml";

static const AudioLocation kStereoAudio = static_cast<AudioLocation>(
    static_cast<uint8_t>(AudioLocation::FRONT_LEFT) |
    static_cast<uint8_t>(AudioLocation::FRONT_RIGHT));
static const AudioLocation kMonoAudio = AudioLocation::UNKNOWN;

static std::vector<LeAudioCodecCapabilitiesSetting> leAudioCodecCapabilities;

static bool isInvalidFileContent = false;

std::optional<setting::LeAudioOffloadSetting>
BluetoothLeAudioCodecsProvider::ParseFromLeAudioOffloadSettingFile() {
  auto le_audio_offload_setting =
      setting::readLeAudioOffloadSetting(kLeAudioCodecCapabilitiesFile);
  if (!le_audio_offload_setting.has_value()) {
    LOG(ERROR) << __func__ << ": Failed to read "
               << kLeAudioCodecCapabilitiesFile;
  }
  return le_audio_offload_setting;
}

void add_flag(CodecInfo& codec_info, int32_t bitmask) {
  auto& transport =
      codec_info.transport.get<CodecInfo::Transport::Tag::leAudio>();
  if (!transport.flags.has_value()) transport.flags = ConfigurationFlags();
  transport.flags->bitmask |= bitmask;
}

// Compare 2 codec info to see if they are equal.
// Currently only compare bitdepth, frameDurationUs and samplingFrequencyHz
bool is_equal(CodecInfo& codec_info_a, CodecInfo& codec_info_b) {
  auto& transport_a =
      codec_info_a.transport.get<CodecInfo::Transport::Tag::leAudio>();
  auto& transport_b =
      codec_info_b.transport.get<CodecInfo::Transport::Tag::leAudio>();
  return codec_info_a.name == codec_info_b.name &&
         transport_a.bitdepth == transport_b.bitdepth &&
         transport_a.frameDurationUs == transport_b.frameDurationUs &&
         transport_a.samplingFrequencyHz == transport_b.samplingFrequencyHz;
}

std::unordered_map<SessionType, std::vector<CodecInfo>>
BluetoothLeAudioCodecsProvider::GetLeAudioCodecInfo(
    const std::optional<setting::LeAudioOffloadSetting>&
        le_audio_offload_setting) {
  // Load from previous storage if present
  if (!session_codecs_map_.empty()) return session_codecs_map_;

  isInvalidFileContent = true;
  if (!le_audio_offload_setting.has_value()) return {};

  // Load scenario, configuration, codec configuration and strategy
  LoadConfigurationToMap(le_audio_offload_setting);
  if (supported_scenarios_.empty() || configuration_map_.empty() ||
      codec_configuration_map_.empty() || strategy_configuration_map_.empty())
    return {};

  // Map each configuration into a CodecInfo
  std::unordered_map<std::string, CodecInfo> config_codec_info_map_;

  for (auto& p : configuration_map_) {
    // Initialize new CodecInfo for the config
    auto config_name = p.first;

    // Getting informations from codecConfig and strategyConfig
    const auto codec_config_name = p.second.getCodecConfiguration();
    const auto strategy_config_name = p.second.getStrategyConfiguration();
    const auto codec_configuration_map_iter =
        codec_configuration_map_.find(codec_config_name);
    if (codec_configuration_map_iter == codec_configuration_map_.end())
      continue;
    const auto strategy_configuration_map_iter =
        strategy_configuration_map_.find(strategy_config_name);
    if (strategy_configuration_map_iter == strategy_configuration_map_.end())
      continue;

    if (config_codec_info_map_.count(config_name) == 0)
      config_codec_info_map_[config_name] = CodecInfo();

    const auto& codec_config = codec_configuration_map_iter->second;
    const auto codec = codec_config.getCodec();
    const auto& strategy_config = strategy_configuration_map_iter->second;
    const auto strategy_config_channel_count =
        strategy_config.getChannelCount();

    // Initiate information
    auto& codec_info = config_codec_info_map_[config_name];
    switch (codec) {
      case setting::CodecType::LC3:
        codec_info.name = "LC3";
        codec_info.id = CodecId::Core::LC3;
        break;
      default:
        codec_info.name = "UNDEFINE";
        codec_info.id = CodecId::Vendor();
        break;
    }
    codec_info.transport =
        CodecInfo::Transport::make<CodecInfo::Transport::Tag::leAudio>();

    // Add low latency support by default
    add_flag(codec_info, ConfigurationFlags::LOW_LATENCY);

    // Mapping codec configuration information
    auto& transport =
        codec_info.transport.get<CodecInfo::Transport::Tag::leAudio>();
    transport.samplingFrequencyHz.push_back(
        codec_config.getSamplingFrequency());
    // Mapping octetsPerCodecFrame to bitdepth for easier comparison.
    transport.bitdepth.push_back(codec_config.getOctetsPerCodecFrame());
    transport.frameDurationUs.push_back(codec_config.getFrameDurationUs());
    if (strategy_config.hasAudioLocation()) {
      switch (strategy_config.getAudioLocation()) {
        case setting::AudioLocation::MONO:
          if (strategy_config_channel_count == 1)
            transport.channelMode.push_back(ChannelMode::MONO);
          else
            transport.channelMode.push_back(ChannelMode::DUALMONO);
          break;
        case setting::AudioLocation::STEREO:
          transport.channelMode.push_back(ChannelMode::STEREO);
          break;
        default:
          transport.channelMode.push_back(ChannelMode::UNKNOWN);
          break;
      }
    } else if (strategy_config.hasAudioChannelAllocation()) {
      auto count =
          std::bitset<32>(strategy_config.getAudioChannelAllocation()).count();
      if (count <= 1) {
        if (strategy_config_channel_count == 1)
          transport.channelMode.push_back(ChannelMode::MONO);
        else
          transport.channelMode.push_back(ChannelMode::DUALMONO);
      } else if (count == 2) {
        transport.channelMode.push_back(ChannelMode::STEREO);
      } else {
        transport.channelMode.push_back(ChannelMode::UNKNOWN);
      }
    } else {
      transport.channelMode.push_back(ChannelMode::UNKNOWN);
    }
  }

  // Goes through a list of scenarios and detect asymmetrical config using
  // codecConfiguration name.
  for (auto& s : supported_scenarios_) {
    if (s.hasEncode() && s.hasDecode() &&
        config_codec_info_map_.count(s.getEncode()) &&
        config_codec_info_map_.count(s.getDecode())) {
      // Check if it's actually using the different codec
      auto& encode_codec_info = config_codec_info_map_[s.getEncode()];
      auto& decode_codec_info = config_codec_info_map_[s.getDecode()];
      if (!is_equal(encode_codec_info, decode_codec_info)) {
        // Change both x and y to become asymmetrical
        add_flag(encode_codec_info,
                 ConfigurationFlags::ALLOW_ASYMMETRIC_CONFIGURATIONS);
        add_flag(decode_codec_info,
                 ConfigurationFlags::ALLOW_ASYMMETRIC_CONFIGURATIONS);
      }
    }
  }

  // Goes through every scenario, deduplicate configuration, skip the invalid
  // config references (e.g. the "invalid" entries in the xml file).
  std::set<std::string> encoding_config, decoding_config, broadcast_config;
  for (auto& s : supported_scenarios_) {
    if (s.hasEncode() && config_codec_info_map_.count(s.getEncode())) {
      encoding_config.insert(s.getEncode());
    }
    if (s.hasDecode() && config_codec_info_map_.count(s.getDecode())) {
      decoding_config.insert(s.getDecode());
    }
    if (s.hasBroadcast() && config_codec_info_map_.count(s.getBroadcast())) {
      broadcast_config.insert(s.getBroadcast());
    }
  }

  // Split by session types and add results
  const auto encoding_path =
      SessionType::LE_AUDIO_HARDWARE_OFFLOAD_ENCODING_DATAPATH;
  const auto decoding_path =
      SessionType::LE_AUDIO_HARDWARE_OFFLOAD_DECODING_DATAPATH;
  const auto broadcast_path =
      SessionType::LE_AUDIO_BROADCAST_HARDWARE_OFFLOAD_ENCODING_DATAPATH;
  session_codecs_map_ =
      std::unordered_map<SessionType, std::vector<CodecInfo>>();
  session_codecs_map_[encoding_path] = std::vector<CodecInfo>();
  session_codecs_map_[decoding_path] = std::vector<CodecInfo>();
  session_codecs_map_[broadcast_path] = std::vector<CodecInfo>();
  session_codecs_map_[encoding_path].reserve(encoding_config.size());
  session_codecs_map_[decoding_path].reserve(decoding_config.size());
  session_codecs_map_[broadcast_path].reserve(broadcast_config.size());
  for (auto& c : encoding_config)
    session_codecs_map_[encoding_path].push_back(config_codec_info_map_[c]);
  for (auto& c : decoding_config)
    session_codecs_map_[decoding_path].push_back(config_codec_info_map_[c]);
  for (auto& c : broadcast_config)
    session_codecs_map_[broadcast_path].push_back(config_codec_info_map_[c]);

  isInvalidFileContent = session_codecs_map_.empty();

  return session_codecs_map_;
}

std::vector<LeAudioCodecCapabilitiesSetting>
BluetoothLeAudioCodecsProvider::GetLeAudioCodecCapabilities(
    const std::optional<setting::LeAudioOffloadSetting>&
        le_audio_offload_setting) {
  if (!leAudioCodecCapabilities.empty()) {
    return leAudioCodecCapabilities;
  }

  isInvalidFileContent = true;

  if (!le_audio_offload_setting.has_value()) {
    LOG(ERROR)
        << __func__
        << ": input le_audio_offload_setting content need to be non empty";
    return {};
  }

  LoadConfigurationToMap(le_audio_offload_setting);
  if (supported_scenarios_.empty() || configuration_map_.empty() ||
      codec_configuration_map_.empty() || strategy_configuration_map_.empty())
    return {};

  leAudioCodecCapabilities =
      ComposeLeAudioCodecCapabilities(supported_scenarios_);
  isInvalidFileContent = leAudioCodecCapabilities.empty();

  return leAudioCodecCapabilities;
}

void BluetoothLeAudioCodecsProvider::ClearLeAudioCodecCapabilities() {
  leAudioCodecCapabilities.clear();
  configuration_map_.clear();
  codec_configuration_map_.clear();
  strategy_configuration_map_.clear();
  session_codecs_map_.clear();
  supported_scenarios_.clear();
}

std::vector<setting::Scenario> BluetoothLeAudioCodecsProvider::GetScenarios(
    const std::optional<setting::LeAudioOffloadSetting>&
        le_audio_offload_setting) {
  std::vector<setting::Scenario> supported_scenarios;
  if (le_audio_offload_setting->hasScenarioList()) {
    for (const auto& scenario_list :
         le_audio_offload_setting->getScenarioList()) {
      if (!scenario_list.hasScenario()) {
        continue;
      }
      for (const auto& scenario : scenario_list.getScenario()) {
        if (scenario.hasEncode() && scenario.hasDecode()) {
          supported_scenarios.push_back(scenario);
        }
      }
    }
  }
  return supported_scenarios;
}

void BluetoothLeAudioCodecsProvider::UpdateConfigurationsToMap(
    const std::optional<setting::LeAudioOffloadSetting>&
        le_audio_offload_setting) {
  if (le_audio_offload_setting->hasConfigurationList()) {
    for (const auto& configuration_list :
         le_audio_offload_setting->getConfigurationList()) {
      if (!configuration_list.hasConfiguration()) {
        continue;
      }
      for (const auto& configuration : configuration_list.getConfiguration()) {
        if (configuration.hasName() && configuration.hasCodecConfiguration() &&
            configuration.hasStrategyConfiguration()) {
          configuration_map_.insert(
              make_pair(configuration.getName(), configuration));
        }
      }
    }
  }
}

void BluetoothLeAudioCodecsProvider::UpdateCodecConfigurationsToMap(
    const std::optional<setting::LeAudioOffloadSetting>&
        le_audio_offload_setting) {
  if (le_audio_offload_setting->hasCodecConfigurationList()) {
    for (const auto& codec_configuration_list :
         le_audio_offload_setting->getCodecConfigurationList()) {
      if (!codec_configuration_list.hasCodecConfiguration()) {
        continue;
      }
      for (const auto& codec_configuration :
           codec_configuration_list.getCodecConfiguration()) {
        if (IsValidCodecConfiguration(codec_configuration)) {
          codec_configuration_map_.insert(
              make_pair(codec_configuration.getName(), codec_configuration));
        }
      }
    }
  }
}

void BluetoothLeAudioCodecsProvider::UpdateStrategyConfigurationsToMap(
    const std::optional<setting::LeAudioOffloadSetting>&
        le_audio_offload_setting) {
  if (le_audio_offload_setting->hasStrategyConfigurationList()) {
    for (const auto& strategy_configuration_list :
         le_audio_offload_setting->getStrategyConfigurationList()) {
      if (!strategy_configuration_list.hasStrategyConfiguration()) {
        continue;
      }
      for (const auto& strategy_configuration :
           strategy_configuration_list.getStrategyConfiguration()) {
        if (IsValidStrategyConfiguration(strategy_configuration)) {
          strategy_configuration_map_.insert(make_pair(
              strategy_configuration.getName(), strategy_configuration));
        }
      }
    }
  }
}

void BluetoothLeAudioCodecsProvider::LoadConfigurationToMap(
    const std::optional<setting::LeAudioOffloadSetting>&
        le_audio_offload_setting) {
  ClearLeAudioCodecCapabilities();

  supported_scenarios_ = GetScenarios(le_audio_offload_setting);
  if (supported_scenarios_.empty()) {
    LOG(ERROR) << __func__ << ": No scenarios in "
               << kLeAudioCodecCapabilitiesFile;
    return;
  }

  UpdateConfigurationsToMap(le_audio_offload_setting);
  if (configuration_map_.empty()) {
    LOG(ERROR) << __func__ << ": No configurations in "
               << kLeAudioCodecCapabilitiesFile;
    return;
  }

  UpdateCodecConfigurationsToMap(le_audio_offload_setting);
  if (codec_configuration_map_.empty()) {
    LOG(ERROR) << __func__ << ": No codec configurations in "
               << kLeAudioCodecCapabilitiesFile;
    return;
  }

  UpdateStrategyConfigurationsToMap(le_audio_offload_setting);
  if (strategy_configuration_map_.empty()) {
    LOG(ERROR) << __func__ << ": No strategy configurations in "
               << kLeAudioCodecCapabilitiesFile;
    return;
  }
}

std::vector<LeAudioCodecCapabilitiesSetting>
BluetoothLeAudioCodecsProvider::ComposeLeAudioCodecCapabilities(
    const std::vector<setting::Scenario>& supported_scenarios) {
  std::vector<LeAudioCodecCapabilitiesSetting> le_audio_codec_capabilities;
  for (const auto& scenario : supported_scenarios) {
    UnicastCapability unicast_encode_capability =
        GetUnicastCapability(scenario.getEncode());
    LOG(INFO) << __func__ << ": Unicast capability encode = "
              << unicast_encode_capability.toString();
    UnicastCapability unicast_decode_capability =
        GetUnicastCapability(scenario.getDecode());
    LOG(INFO) << __func__ << ": Unicast capability decode = "
              << unicast_decode_capability.toString();
    BroadcastCapability broadcast_capability = {.codecType =
                                                    CodecType::UNKNOWN};

    if (scenario.hasBroadcast()) {
      broadcast_capability = GetBroadcastCapability(scenario.getBroadcast());
    }

    // At least one capability should be valid
    if (unicast_encode_capability.codecType == CodecType::UNKNOWN &&
        unicast_decode_capability.codecType == CodecType::UNKNOWN &&
        broadcast_capability.codecType == CodecType::UNKNOWN) {
      LOG(ERROR) << __func__ << ": None of the capability is valid.";
      continue;
    }

    le_audio_codec_capabilities.push_back(
        {.unicastEncodeCapability = unicast_encode_capability,
         .unicastDecodeCapability = unicast_decode_capability,
         .broadcastCapability = broadcast_capability});
  }
  return le_audio_codec_capabilities;
}

UnicastCapability BluetoothLeAudioCodecsProvider::GetUnicastCapability(
    const std::string& coding_direction) {
  if (coding_direction == "invalid") {
    return {.codecType = CodecType::UNKNOWN};
  }

  auto configuration_iter = configuration_map_.find(coding_direction);
  if (configuration_iter == configuration_map_.end()) {
    return {.codecType = CodecType::UNKNOWN};
  }

  auto codec_configuration_iter = codec_configuration_map_.find(
      configuration_iter->second.getCodecConfiguration());
  if (codec_configuration_iter == codec_configuration_map_.end()) {
    return {.codecType = CodecType::UNKNOWN};
  }

  auto strategy_configuration_iter = strategy_configuration_map_.find(
      configuration_iter->second.getStrategyConfiguration());
  if (strategy_configuration_iter == strategy_configuration_map_.end()) {
    return {.codecType = CodecType::UNKNOWN};
  }

  // Populate audio location
  AudioLocation audio_location = AudioLocation::UNKNOWN;
  if (strategy_configuration_iter->second.hasAudioLocation()) {
    audio_location = GetAudioLocation(
        strategy_configuration_iter->second.getAudioLocation());
  }

  // Populate audio channel allocation
  std::optional<CodecSpecificConfigurationLtv::AudioChannelAllocation>
      audio_channel_allocation = std::nullopt;
  if (strategy_configuration_iter->second.hasAudioChannelAllocation()) {
    LOG(INFO) << __func__ << ": has allocation";
    CodecSpecificConfigurationLtv::AudioChannelAllocation tmp;
    tmp.bitmask =
        strategy_configuration_iter->second.getAudioChannelAllocation();
    audio_channel_allocation = tmp;
  }

  CodecType codec_type =
      GetCodecType(codec_configuration_iter->second.getCodec());
  if (codec_type == CodecType::LC3) {
    return ComposeUnicastCapability(
        codec_type, audio_location, audio_channel_allocation,
        strategy_configuration_iter->second.getConnectedDevice(),
        strategy_configuration_iter->second.getChannelCount(),
        ComposeLc3Capability(codec_configuration_iter->second));
  } else if (codec_type == CodecType::APTX_ADAPTIVE_LE ||
             codec_type == CodecType::APTX_ADAPTIVE_LEX) {
    return ComposeUnicastCapability(
        codec_type, audio_location, audio_channel_allocation,
        strategy_configuration_iter->second.getConnectedDevice(),
        strategy_configuration_iter->second.getChannelCount(),
        ComposeAptxAdaptiveLeCapability(codec_configuration_iter->second));
  }
  return {.codecType = CodecType::UNKNOWN};
}

BroadcastCapability BluetoothLeAudioCodecsProvider::GetBroadcastCapability(
    const std::string& coding_direction) {
  if (coding_direction == "invalid") {
    return {.codecType = CodecType::UNKNOWN};
  }

  auto configuration_iter = configuration_map_.find(coding_direction);
  if (configuration_iter == configuration_map_.end()) {
    return {.codecType = CodecType::UNKNOWN};
  }

  auto codec_configuration_iter = codec_configuration_map_.find(
      configuration_iter->second.getCodecConfiguration());
  if (codec_configuration_iter == codec_configuration_map_.end()) {
    return {.codecType = CodecType::UNKNOWN};
  }

  auto strategy_configuration_iter = strategy_configuration_map_.find(
      configuration_iter->second.getStrategyConfiguration());
  if (strategy_configuration_iter == strategy_configuration_map_.end()) {
    return {.codecType = CodecType::UNKNOWN};
  }

  CodecType codec_type =
      GetCodecType(codec_configuration_iter->second.getCodec());
  std::vector<std::optional<Lc3Capabilities>> bcastLc3Cap(
      1, std::optional(ComposeLc3Capability(codec_configuration_iter->second)));

  // Populate audio location
  AudioLocation audio_location = AudioLocation::UNKNOWN;
  if (strategy_configuration_iter->second.hasAudioLocation()) {
    audio_location = GetAudioLocation(
        strategy_configuration_iter->second.getAudioLocation());
  }

  // Populate audio channel allocation
  std::optional<CodecSpecificConfigurationLtv::AudioChannelAllocation>
      audio_channel_allocation = std::nullopt;
  if (strategy_configuration_iter->second.hasAudioChannelAllocation()) {
    LOG(INFO) << __func__ << ": has allocation";
    CodecSpecificConfigurationLtv::AudioChannelAllocation tmp;
    tmp.bitmask =
        strategy_configuration_iter->second.getAudioChannelAllocation();
    audio_channel_allocation = tmp;
  }

  if (codec_type == CodecType::LC3) {
    return ComposeBroadcastCapability(
        codec_type, audio_location, audio_channel_allocation,
        strategy_configuration_iter->second.getChannelCount(), bcastLc3Cap);
  }
  return {.codecType = CodecType::UNKNOWN};
}

template <class T>
BroadcastCapability BluetoothLeAudioCodecsProvider::ComposeBroadcastCapability(
    const CodecType& codec_type, const AudioLocation& audio_location,
    const std::optional<CodecSpecificConfigurationLtv::AudioChannelAllocation>&
        audio_channel_allocation,
    const uint8_t& channel_count, const std::vector<T>& capability) {
  return {.codecType = codec_type,
          .supportedChannel = audio_location,
          .channelCountPerStream = channel_count,
          .leAudioCodecCapabilities = std::optional(capability),
          .audioLocation = audio_channel_allocation};
}

template <class T>
UnicastCapability BluetoothLeAudioCodecsProvider::ComposeUnicastCapability(
    const CodecType& codec_type, const AudioLocation& audio_location,
    const std::optional<CodecSpecificConfigurationLtv::AudioChannelAllocation>&
        audio_channel_allocation,
    const uint8_t& device_cnt, const uint8_t& channel_count,
    const T& capability) {
  return {
      .codecType = codec_type,
      .supportedChannel = audio_location,
      .deviceCount = device_cnt,
      .channelCountPerDevice = channel_count,
      .leAudioCodecCapabilities =
          UnicastCapability::LeAudioCodecCapabilities(capability),
      .audioLocation = audio_channel_allocation,
  };
}

Lc3Capabilities BluetoothLeAudioCodecsProvider::ComposeLc3Capability(
    const setting::CodecConfiguration& codec_configuration) {
  return {.samplingFrequencyHz = {codec_configuration.getSamplingFrequency()},
          .frameDurationUs = {codec_configuration.getFrameDurationUs()},
          .octetsPerFrame = {codec_configuration.getOctetsPerCodecFrame()}};
}

AptxAdaptiveLeCapabilities
BluetoothLeAudioCodecsProvider::ComposeAptxAdaptiveLeCapability(
    const setting::CodecConfiguration& codec_configuration) {
  return {.samplingFrequencyHz = {codec_configuration.getSamplingFrequency()},
          .frameDurationUs = {codec_configuration.getFrameDurationUs()},
          .octetsPerFrame = {codec_configuration.getOctetsPerCodecFrame()}};
}

AudioLocation BluetoothLeAudioCodecsProvider::GetAudioLocation(
    const setting::AudioLocation& audio_location) {
  switch (audio_location) {
    case setting::AudioLocation::MONO:
      return kMonoAudio;
    case setting::AudioLocation::STEREO:
      return kStereoAudio;
    default:
      return AudioLocation::UNKNOWN;
  }
}

CodecType BluetoothLeAudioCodecsProvider::GetCodecType(
    const setting::CodecType& codec_type) {
  switch (codec_type) {
    case setting::CodecType::LC3:
      return CodecType::LC3;
    case setting::CodecType::APTX_ADAPTIVE_LE:
      return CodecType::APTX_ADAPTIVE_LE;
    case setting::CodecType::APTX_ADAPTIVE_LEX:
      return CodecType::APTX_ADAPTIVE_LEX;
    default:
      return CodecType::UNKNOWN;
  }
}

bool BluetoothLeAudioCodecsProvider::IsValidCodecConfiguration(
    const setting::CodecConfiguration& codec_configuration) {
  return codec_configuration.hasName() && codec_configuration.hasCodec() &&
         codec_configuration.hasSamplingFrequency() &&
         codec_configuration.hasFrameDurationUs() &&
         codec_configuration.hasOctetsPerCodecFrame();
}

bool IsValidStereoAudioLocation(
    const setting::StrategyConfiguration& strategy_configuration) {
  if ((strategy_configuration.getConnectedDevice() == 2 &&
       strategy_configuration.getChannelCount() == 1) ||
      (strategy_configuration.getConnectedDevice() == 1 &&
       strategy_configuration.getChannelCount() == 2)) {
    // Stereo
    // 1. two connected device, one for L one for R
    // 2. one connected device for both L and R
    return true;
  } else if (strategy_configuration.getConnectedDevice() == 0 &&
             strategy_configuration.getChannelCount() == 2) {
    // Broadcast
    return true;
  }
  return false;
}

bool IsValidMonoAudioLocation(
    const setting::StrategyConfiguration& strategy_configuration) {
  if (strategy_configuration.getConnectedDevice() == 1 &&
      strategy_configuration.getChannelCount() == 1) {
    return true;
  }
  return false;
}

bool IsValidAudioLocation(
    const setting::StrategyConfiguration& strategy_configuration) {
  if (strategy_configuration.getAudioLocation() ==
      setting::AudioLocation::STEREO)
    return IsValidStereoAudioLocation(strategy_configuration);
  else if (strategy_configuration.getAudioLocation() ==
           setting::AudioLocation::MONO)
    return IsValidMonoAudioLocation(strategy_configuration);
  return false;
}

bool IsValidAudioChannelAllocation(
    const setting::StrategyConfiguration& strategy_configuration) {
  // First, ensure that there's only 2 bitmask enabled
  int audio_channel_allocation =
      strategy_configuration.getAudioChannelAllocation();
  int count = 0;
  for (int bit = 0; bit < 32; ++bit)
    if (audio_channel_allocation & (1 << bit)) ++count;
  if (count > 2) {
    LOG(WARNING) << "Cannot parse more than 2 audio location, input is "
                 << audio_channel_allocation;
    return false;
  }

  if (count == 2)
    return IsValidStereoAudioLocation(strategy_configuration);
  else
    return IsValidMonoAudioLocation(strategy_configuration);
}

bool BluetoothLeAudioCodecsProvider::IsValidStrategyConfiguration(
    const setting::StrategyConfiguration& strategy_configuration) {
  if (!strategy_configuration.hasName() ||
      !strategy_configuration.hasConnectedDevice() ||
      !strategy_configuration.hasChannelCount()) {
    return false;
  }

  // Both audio location field cannot be empty
  if (!strategy_configuration.hasAudioLocation() &&
      !strategy_configuration.hasAudioChannelAllocation())
    return false;

  // Any audio location field that presents must be valid
  if (strategy_configuration.hasAudioLocation() &&
      !IsValidAudioLocation(strategy_configuration))
    return false;

  if (strategy_configuration.hasAudioChannelAllocation() &&
      !IsValidAudioChannelAllocation(strategy_configuration))
    return false;

  return true;
}

}  // namespace audio
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
}  // namespace aidl

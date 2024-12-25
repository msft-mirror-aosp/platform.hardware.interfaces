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

#include <gtest/gtest.h>

#include <optional>
#include <tuple>

#include "BluetoothLeAudioCodecsProvider.h"
#include "aidl/android/hardware/bluetooth/audio/CodecInfo.h"
#include "aidl/android/hardware/bluetooth/audio/ConfigurationFlags.h"
#include "aidl/android/hardware/bluetooth/audio/SessionType.h"

using aidl::android::hardware::bluetooth::audio::BluetoothLeAudioCodecsProvider;
using aidl::android::hardware::bluetooth::audio::CodecInfo;
using aidl::android::hardware::bluetooth::audio::ConfigurationFlags;
using aidl::android::hardware::bluetooth::audio::
    LeAudioCodecCapabilitiesSetting;
using aidl::android::hardware::bluetooth::audio::SessionType;
using aidl::android::hardware::bluetooth::audio::setting::AudioLocation;
using aidl::android::hardware::bluetooth::audio::setting::CodecConfiguration;
using aidl::android::hardware::bluetooth::audio::setting::
    CodecConfigurationList;
using aidl::android::hardware::bluetooth::audio::setting::CodecType;
using aidl::android::hardware::bluetooth::audio::setting::Configuration;
using aidl::android::hardware::bluetooth::audio::setting::ConfigurationList;
using aidl::android::hardware::bluetooth::audio::setting::LeAudioOffloadSetting;
using aidl::android::hardware::bluetooth::audio::setting::Scenario;
using aidl::android::hardware::bluetooth::audio::setting::ScenarioList;
using aidl::android::hardware::bluetooth::audio::setting::StrategyConfiguration;
using aidl::android::hardware::bluetooth::audio::setting::
    StrategyConfigurationList;

typedef std::tuple<std::vector<ScenarioList>, std::vector<ConfigurationList>,
                   std::vector<CodecConfigurationList>,
                   std::vector<StrategyConfigurationList>>
    OffloadSetting;

// Define valid components for each list
// Scenario
static const Scenario kValidScenario(std::make_optional("OneChanStereo_16_1"),
                                     std::make_optional("OneChanStereo_16_1"),
                                     std::nullopt);
static const Scenario kValidBroadcastScenario(
    std::nullopt, std::nullopt, std::make_optional("BcastStereo_16_2"));

static const Scenario kValidAsymmetricScenario(
    std::make_optional("OneChanStereo_32_1"),
    std::make_optional("OneChanStereo_16_1"), std::nullopt);

// Configuration
static const Configuration kValidConfigOneChanStereo_16_1(
    std::make_optional("OneChanStereo_16_1"), std::make_optional("LC3_16k_1"),
    std::make_optional("STEREO_ONE_CIS_PER_DEVICE"));

static const Configuration kValidConfigOneChanStereo_32_1(
    std::make_optional("OneChanStereo_32_1"), std::make_optional("LC3_32k_1"),
    std::make_optional("STEREO_ONE_CIS_PER_DEVICE"));

// CodecConfiguration
static const CodecConfiguration kValidCodecLC3_16k_1(
    std::make_optional("LC3_16k_1"), std::make_optional(CodecType::LC3),
    std::nullopt, std::make_optional(16000), std::make_optional(7500),
    std::make_optional(30), std::nullopt);

static const CodecConfiguration kValidCodecLC3_32k_1(
    std::make_optional("LC3_32k_1"), std::make_optional(CodecType::LC3),
    std::nullopt, std::make_optional(32000), std::make_optional(7500),
    std::make_optional(30), std::nullopt);

// StrategyConfiguration
static const StrategyConfiguration kValidStrategyStereoOneCis(
    std::make_optional("STEREO_ONE_CIS_PER_DEVICE"),
    std::make_optional(AudioLocation::STEREO), std::make_optional(2),
    std::make_optional(1), std::nullopt);
static const StrategyConfiguration kValidStrategyStereoTwoCis(
    std::make_optional("STEREO_TWO_CISES_PER_DEVICE"),
    std::make_optional(AudioLocation::STEREO), std::make_optional(1),
    std::make_optional(2), std::nullopt);
static const StrategyConfiguration kValidStrategyMonoOneCis(
    std::make_optional("MONO_ONE_CIS_PER_DEVICE"),
    std::make_optional(AudioLocation::MONO), std::make_optional(1),
    std::make_optional(1), std::nullopt);
static const StrategyConfiguration kValidStrategyBroadcastStereo(
    std::make_optional("BROADCAST_STEREO"),
    std::make_optional(AudioLocation::STEREO), std::make_optional(0),
    std::make_optional(2), std::nullopt);

static const StrategyConfiguration kValidStrategyStereoOneCisInt(
    std::make_optional("STEREO_ONE_CIS_PER_DEVICE"), std::nullopt,
    std::make_optional(2), std::make_optional(1), std::make_optional(3));
static const StrategyConfiguration kValidStrategyStereoTwoCisInt(
    std::make_optional("STEREO_TWO_CISES_PER_DEVICE"), std::nullopt,
    std::make_optional(1), std::make_optional(2), std::make_optional(3));
static const StrategyConfiguration kValidStrategyMonoOneCisInt(
    std::make_optional("MONO_ONE_CIS_PER_DEVICE"), std::nullopt,
    std::make_optional(1), std::make_optional(1), std::make_optional(4));
static const StrategyConfiguration kValidStrategyBroadcastStereoInt(
    std::make_optional("BROADCAST_STEREO"), std::nullopt, std::make_optional(0),
    std::make_optional(2), std::make_optional(3));

static const StrategyConfiguration kValidStrategyStereoOneCisBoth(
    std::make_optional("STEREO_ONE_CIS_PER_DEVICE"),
    std::make_optional(AudioLocation::STEREO), std::make_optional(2),
    std::make_optional(1), std::make_optional(3));
static const StrategyConfiguration kValidStrategyStereoTwoCisBoth(
    std::make_optional("STEREO_TWO_CISES_PER_DEVICE"),
    std::make_optional(AudioLocation::STEREO), std::make_optional(1),
    std::make_optional(2), std::make_optional(3));
static const StrategyConfiguration kValidStrategyMonoOneCisBoth(
    std::make_optional("MONO_ONE_CIS_PER_DEVICE"),
    std::make_optional(AudioLocation::MONO), std::make_optional(1),
    std::make_optional(1), std::make_optional(4));
static const StrategyConfiguration kValidStrategyBroadcastStereoBoth(
    std::make_optional("BROADCAST_STEREO"),
    std::make_optional(AudioLocation::STEREO), std::make_optional(0),
    std::make_optional(2), std::make_optional(3));

// List of all invalid strategy configuration
const auto kInvalidStrategyStereoTwoCisTwoDevice = StrategyConfigurationList(
    std::vector<StrategyConfiguration>{StrategyConfiguration(
        std::make_optional("STEREO_ONE_CIS_PER_DEVICE"),
        std::make_optional(AudioLocation::STEREO), std::make_optional(2),
        std::make_optional(2), std::nullopt)});
const auto kInvalidStrategyMonoTwoCisTwoDevice = StrategyConfigurationList(
    std::vector<StrategyConfiguration>{StrategyConfiguration(
        std::make_optional("MONO_ONE_CIS_PER_DEVICE"),
        std::make_optional(AudioLocation::STEREO), std::make_optional(2),
        std::make_optional(2), std::nullopt)});
const auto kInvalidStrategyNoName = StrategyConfigurationList(
    std::vector<StrategyConfiguration>{StrategyConfiguration(
        std::nullopt, std::make_optional(AudioLocation::STEREO),
        std::make_optional(2), std::make_optional(1), std::nullopt)});
const auto kInvalidStrategyNoLocation = StrategyConfigurationList(
    std::vector<StrategyConfiguration>{StrategyConfiguration(
        std::make_optional("STEREO_ONE_CIS_PER_DEVICE"), std::nullopt,
        std::make_optional(2), std::make_optional(1), std::nullopt)});
const auto kInvalidStrategyNoDevice = StrategyConfigurationList(
    std::vector<StrategyConfiguration>{StrategyConfiguration(
        std::make_optional("STEREO_ONE_CIS_PER_DEVICE"),
        std::make_optional(AudioLocation::STEREO), std::nullopt,
        std::make_optional(1), std::nullopt)});
const auto kInvalidStrategyNoChannel = StrategyConfigurationList(
    std::vector<StrategyConfiguration>{StrategyConfiguration(
        std::make_optional("STEREO_ONE_CIS_PER_DEVICE"),
        std::make_optional(AudioLocation::STEREO), std::make_optional(2),
        std::nullopt, std::nullopt)});
const auto kInvalidStrategyIntMoreBitmask = StrategyConfigurationList(
    std::vector<StrategyConfiguration>{StrategyConfiguration(
        std::make_optional("STEREO_ONE_CIS_PER_DEVICE"),
        std::make_optional(AudioLocation::STEREO), std::make_optional(2),
        std::make_optional(1), std::make_optional(7))});
const auto kInvalidStrategyIntStereoTwoCisTwoDevice = StrategyConfigurationList(
    std::vector<StrategyConfiguration>{StrategyConfiguration(
        std::make_optional("STEREO_ONE_CIS_PER_DEVICE"), std::nullopt,
        std::make_optional(2), std::make_optional(2), std::make_optional(3))});
const auto kInvalidStrategyIntMonoTwoCisTwoDevice = StrategyConfigurationList(
    std::vector<StrategyConfiguration>{StrategyConfiguration(
        std::make_optional("MONO_ONE_CIS_PER_DEVICE"), std::nullopt,
        std::make_optional(2), std::make_optional(2), std::make_optional(4))});
const auto kInvalidStrategyIntBroadcast = StrategyConfigurationList(
    std::vector<StrategyConfiguration>{StrategyConfiguration(
        std::make_optional("MONO_ONE_CIS_PER_DEVICE"), std::nullopt,
        std::make_optional(0), std::make_optional(1), std::make_optional(3))});
const auto kInvalidStrategyBothStereoMonoInt = StrategyConfigurationList(
    std::vector<StrategyConfiguration>{StrategyConfiguration(
        std::make_optional("STEREO_ONE_CIS_PER_DEVICE"),
        std::make_optional(AudioLocation::STEREO), std::make_optional(2),
        std::make_optional(1), std::make_optional(4))});

// Define valid test list built from above valid components
// Scenario, Configuration, CodecConfiguration, StrategyConfiguration
static const std::vector<ScenarioList> kValidScenarioList = {ScenarioList(
    std::vector<Scenario>{kValidScenario, kValidBroadcastScenario})};
static const std::vector<ConfigurationList> kValidConfigurationList = {
    ConfigurationList(
        std::vector<Configuration>{kValidConfigOneChanStereo_16_1})};
static const std::vector<CodecConfigurationList> kValidCodecConfigurationList =
    {CodecConfigurationList(
        std::vector<CodecConfiguration>{kValidCodecLC3_16k_1})};

static const std::vector<StrategyConfigurationList>
    kValidStrategyConfigurationList = {
        StrategyConfigurationList(std::vector<StrategyConfiguration>{
            kValidStrategyStereoOneCis, kValidStrategyStereoTwoCis,
            kValidStrategyMonoOneCis, kValidStrategyBroadcastStereo,
            kValidStrategyStereoOneCisInt, kValidStrategyStereoTwoCisInt,
            kValidStrategyMonoOneCisInt, kValidStrategyBroadcastStereoInt,
            kValidStrategyStereoOneCisBoth, kValidStrategyStereoTwoCisBoth,
            kValidStrategyMonoOneCisBoth, kValidStrategyBroadcastStereoBoth})};

// Define some valid asymmetric scenario list
static const std::vector<ScenarioList> kValidAsymmetricScenarioList = {
    ScenarioList(std::vector<Scenario>{kValidAsymmetricScenario})};
static const std::vector<ConfigurationList> kValidAsymmetricConfigurationList =
    {ConfigurationList(std::vector<Configuration>{
        kValidConfigOneChanStereo_16_1, kValidConfigOneChanStereo_32_1})};
static const std::vector<CodecConfigurationList>
    kValidAsymmetricCodecConfigurationList = {
        CodecConfigurationList(std::vector<CodecConfiguration>{
            kValidCodecLC3_16k_1, kValidCodecLC3_32k_1})};

class BluetoothLeAudioCodecsProviderTest
    : public ::testing::TestWithParam<OffloadSetting> {
 public:
  static std::vector<OffloadSetting> CreateTestCases(
      const std::vector<ScenarioList>& scenario_lists,
      const std::vector<ConfigurationList>& configuration_lists,
      const std::vector<CodecConfigurationList>& codec_configuration_lists,
      const std::vector<StrategyConfigurationList>&
          strategy_configuration_lists) {
    // make each vector in output test_cases has only one element
    // to match the input of test params
    // normally only one vector in input has multiple elements
    // we just split elements in this vector to several vector
    std::vector<OffloadSetting> test_cases;
    for (const auto& scenario_list : scenario_lists) {
      for (const auto& configuration_list : configuration_lists) {
        for (const auto& codec_configuration_list : codec_configuration_lists) {
          for (const auto& strategy_configuration_list :
               strategy_configuration_lists) {
            test_cases.push_back(CreateTestCase(
                scenario_list, configuration_list, codec_configuration_list,
                strategy_configuration_list));
          }
        }
      }
    }
    return test_cases;
  }

 protected:
  void Initialize() {
    BluetoothLeAudioCodecsProvider::ClearLeAudioCodecCapabilities();
  }

  std::vector<LeAudioCodecCapabilitiesSetting> RunTestCase() {
    auto& [scenario_lists, configuration_lists, codec_configuration_lists,
           strategy_configuration_lists] = GetParam();
    LeAudioOffloadSetting le_audio_offload_setting(
        scenario_lists, configuration_lists, codec_configuration_lists,
        strategy_configuration_lists);
    auto le_audio_codec_capabilities =
        BluetoothLeAudioCodecsProvider::GetLeAudioCodecCapabilities(
            std::make_optional(le_audio_offload_setting));
    return le_audio_codec_capabilities;
  }

  std::unordered_map<SessionType, std::vector<CodecInfo>>
  RunCodecInfoTestCase() {
    auto& [scenario_lists, configuration_lists, codec_configuration_lists,
           strategy_configuration_lists] = GetParam();
    LeAudioOffloadSetting le_audio_offload_setting(
        scenario_lists, configuration_lists, codec_configuration_lists,
        strategy_configuration_lists);
    auto le_audio_codec_capabilities =
        BluetoothLeAudioCodecsProvider::GetLeAudioCodecInfo(
            std::make_optional(le_audio_offload_setting));
    return le_audio_codec_capabilities;
  }

 private:
  static inline OffloadSetting CreateTestCase(
      const ScenarioList& scenario_list,
      const ConfigurationList& configuration_list,
      const CodecConfigurationList& codec_configuration_list,
      const StrategyConfigurationList& strategy_configuration_list) {
    return std::make_tuple(
        std::vector<ScenarioList>{scenario_list},
        std::vector<ConfigurationList>{configuration_list},
        std::vector<CodecConfigurationList>{codec_configuration_list},
        std::vector<StrategyConfigurationList>{strategy_configuration_list});
  }
};

class GetScenariosTest : public BluetoothLeAudioCodecsProviderTest {
 public:
  static std::vector<ScenarioList> CreateInvalidScenarios() {
    std::vector<ScenarioList> invalid_scenario_test_cases;
    invalid_scenario_test_cases.push_back(ScenarioList(std::vector<Scenario>{
        Scenario(std::nullopt, std::make_optional("OneChanStereo_16_1"),
                 std::nullopt)}));

    invalid_scenario_test_cases.push_back(ScenarioList(
        std::vector<Scenario>{Scenario(std::make_optional("OneChanStereo_16_1"),
                                       std::nullopt, std::nullopt)}));

    invalid_scenario_test_cases.push_back(ScenarioList(std::vector<Scenario>{
        Scenario(std::nullopt, std::nullopt, std::nullopt)}));

    invalid_scenario_test_cases.push_back(
        ScenarioList(std::vector<Scenario>{}));

    return invalid_scenario_test_cases;
  }
};

TEST_P(GetScenariosTest, InvalidScenarios) {
  Initialize();
  auto le_audio_codec_capabilities = RunTestCase();
  ASSERT_TRUE(le_audio_codec_capabilities.empty());
}

class UpdateConfigurationsToMapTest
    : public BluetoothLeAudioCodecsProviderTest {
 public:
  static std::vector<ConfigurationList> CreateInvalidConfigurations() {
    std::vector<ConfigurationList> invalid_configuration_test_cases;
    invalid_configuration_test_cases.push_back(
        ConfigurationList(std::vector<Configuration>{
            Configuration(std::nullopt, std::make_optional("LC3_16k_1"),
                          std::make_optional("STEREO_ONE_CIS_PER_DEVICE"))}));

    invalid_configuration_test_cases.push_back(
        ConfigurationList(std::vector<Configuration>{Configuration(
            std::make_optional("OneChanStereo_16_1"), std::nullopt,
            std::make_optional("STEREO_ONE_CIS_PER_DEVICE"))}));

    invalid_configuration_test_cases.push_back(
        ConfigurationList(std::vector<Configuration>{
            Configuration(std::make_optional("OneChanStereo_16_1"),
                          std::make_optional("LC3_16k_1"), std::nullopt)}));

    invalid_configuration_test_cases.push_back(
        ConfigurationList(std::vector<Configuration>{}));

    return invalid_configuration_test_cases;
  }
};

TEST_P(UpdateConfigurationsToMapTest, InvalidConfigurations) {
  Initialize();
  auto le_audio_codec_capabilities = RunTestCase();
  ASSERT_TRUE(le_audio_codec_capabilities.empty());
}

class UpdateCodecConfigurationsToMapTest
    : public BluetoothLeAudioCodecsProviderTest {
 public:
  static std::vector<CodecConfigurationList>
  CreateInvalidCodecConfigurations() {
    std::vector<CodecConfigurationList> invalid_codec_configuration_test_cases;
    invalid_codec_configuration_test_cases.push_back(CodecConfigurationList(
        std::vector<CodecConfiguration>{CodecConfiguration(
            std::nullopt, std::make_optional(CodecType::LC3), std::nullopt,
            std::make_optional(16000), std::make_optional(7500),
            std::make_optional(30), std::nullopt)}));

    invalid_codec_configuration_test_cases.push_back(CodecConfigurationList(
        std::vector<CodecConfiguration>{CodecConfiguration(
            std::make_optional("LC3_16k_1"), std::nullopt, std::nullopt,
            std::make_optional(16000), std::make_optional(7500),
            std::make_optional(30), std::nullopt)}));

    invalid_codec_configuration_test_cases.push_back(CodecConfigurationList(
        std::vector<CodecConfiguration>{CodecConfiguration(
            std::make_optional("LC3_16k_1"), std::make_optional(CodecType::LC3),
            std::nullopt, std::nullopt, std::make_optional(7500),
            std::make_optional(30), std::nullopt)}));

    invalid_codec_configuration_test_cases.push_back(CodecConfigurationList(
        std::vector<CodecConfiguration>{CodecConfiguration(
            std::make_optional("LC3_16k_1"), std::make_optional(CodecType::LC3),
            std::nullopt, std::make_optional(16000), std::nullopt,
            std::make_optional(30), std::nullopt)}));

    invalid_codec_configuration_test_cases.push_back(CodecConfigurationList(
        std::vector<CodecConfiguration>{CodecConfiguration(
            std::make_optional("LC3_16k_1"), std::make_optional(CodecType::LC3),
            std::nullopt, std::make_optional(16000), std::make_optional(7500),
            std::nullopt, std::nullopt)}));

    invalid_codec_configuration_test_cases.push_back(
        CodecConfigurationList(std::vector<CodecConfiguration>{}));

    return invalid_codec_configuration_test_cases;
  }
};

TEST_P(UpdateCodecConfigurationsToMapTest, InvalidCodecConfigurations) {
  Initialize();
  auto le_audio_codec_capabilities = RunTestCase();
  ASSERT_TRUE(le_audio_codec_capabilities.empty());
}

class UpdateStrategyConfigurationsToMapTest
    : public BluetoothLeAudioCodecsProviderTest {
 public:
  static std::vector<StrategyConfigurationList>
  CreateInvalidStrategyConfigurations() {
    std::vector<StrategyConfigurationList>
        invalid_strategy_configuration_test_cases = {
            kInvalidStrategyStereoTwoCisTwoDevice,
            kInvalidStrategyMonoTwoCisTwoDevice,
            kInvalidStrategyNoName,
            kInvalidStrategyNoLocation,
            kInvalidStrategyNoDevice,
            kInvalidStrategyNoChannel,
            kInvalidStrategyIntMoreBitmask,
            kInvalidStrategyIntStereoTwoCisTwoDevice,
            kInvalidStrategyIntMonoTwoCisTwoDevice,
            kInvalidStrategyIntBroadcast,
            kInvalidStrategyBothStereoMonoInt,
            StrategyConfigurationList(std::vector<StrategyConfiguration>{})};

    return invalid_strategy_configuration_test_cases;
  }
};

TEST_P(UpdateStrategyConfigurationsToMapTest, InvalidStrategyConfigurations) {
  Initialize();
  auto le_audio_codec_capabilities = RunTestCase();
  ASSERT_TRUE(le_audio_codec_capabilities.empty());
}

class ComposeLeAudioCodecCapabilitiesTest
    : public BluetoothLeAudioCodecsProviderTest {
 public:
};

TEST_P(ComposeLeAudioCodecCapabilitiesTest, CodecCapabilitiesNotEmpty) {
  Initialize();
  auto le_audio_codec_capabilities = RunTestCase();
  ASSERT_TRUE(!le_audio_codec_capabilities.empty());
}

class ComposeLeAudioAymmetricCodecInfoTest
    : public BluetoothLeAudioCodecsProviderTest {
 public:
};

TEST_P(ComposeLeAudioAymmetricCodecInfoTest, AsymmetricCodecInfoNotEmpty) {
  Initialize();
  auto le_audio_codec_info_map = RunCodecInfoTestCase();
  ASSERT_TRUE(!le_audio_codec_info_map.empty());
  // Check true asymmetric codec info
  ASSERT_TRUE(!le_audio_codec_info_map
                   [SessionType::LE_AUDIO_HARDWARE_OFFLOAD_ENCODING_DATAPATH]
                       .empty());
  ASSERT_TRUE(!le_audio_codec_info_map
                   [SessionType::LE_AUDIO_HARDWARE_OFFLOAD_DECODING_DATAPATH]
                       .empty());
  auto required_flag = ConfigurationFlags();
  required_flag.bitmask |= ConfigurationFlags::ALLOW_ASYMMETRIC_CONFIGURATIONS;

  auto codec_info = le_audio_codec_info_map
      [SessionType::LE_AUDIO_HARDWARE_OFFLOAD_ENCODING_DATAPATH][0];
  ASSERT_EQ(codec_info.transport.getTag(), CodecInfo::Transport::Tag::leAudio);
  auto& transport =
      codec_info.transport.get<CodecInfo::Transport::Tag::leAudio>();
  ASSERT_EQ(transport.flags, std::make_optional(required_flag));

  codec_info = le_audio_codec_info_map
      [SessionType::LE_AUDIO_HARDWARE_OFFLOAD_DECODING_DATAPATH][0];
  ASSERT_EQ(codec_info.transport.getTag(), CodecInfo::Transport::Tag::leAudio);
  transport = codec_info.transport.get<CodecInfo::Transport::Tag::leAudio>();
  ASSERT_EQ(transport.flags, std::make_optional(required_flag));
}

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(GetScenariosTest);
INSTANTIATE_TEST_SUITE_P(
    BluetoothLeAudioCodecsProviderTest, GetScenariosTest,
    ::testing::ValuesIn(BluetoothLeAudioCodecsProviderTest::CreateTestCases(
        GetScenariosTest::CreateInvalidScenarios(), kValidConfigurationList,
        kValidCodecConfigurationList, kValidStrategyConfigurationList)));

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(UpdateConfigurationsToMapTest);
INSTANTIATE_TEST_SUITE_P(
    BluetoothLeAudioCodecsProviderTest, UpdateConfigurationsToMapTest,
    ::testing::ValuesIn(BluetoothLeAudioCodecsProviderTest::CreateTestCases(
        kValidScenarioList,
        UpdateConfigurationsToMapTest::CreateInvalidConfigurations(),
        kValidCodecConfigurationList, kValidStrategyConfigurationList)));

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(
    UpdateCodecConfigurationsToMapTest);
INSTANTIATE_TEST_SUITE_P(
    BluetoothLeAudioCodecsProviderTest, UpdateCodecConfigurationsToMapTest,
    ::testing::ValuesIn(BluetoothLeAudioCodecsProviderTest::CreateTestCases(
        kValidScenarioList, kValidConfigurationList,
        UpdateCodecConfigurationsToMapTest::CreateInvalidCodecConfigurations(),
        kValidStrategyConfigurationList)));

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(
    UpdateStrategyConfigurationsToMapTest);
INSTANTIATE_TEST_SUITE_P(
    BluetoothLeAudioCodecsProviderTest, UpdateStrategyConfigurationsToMapTest,
    ::testing::ValuesIn(BluetoothLeAudioCodecsProviderTest::CreateTestCases(
        kValidScenarioList, kValidConfigurationList,
        kValidCodecConfigurationList,
        UpdateStrategyConfigurationsToMapTest::
            CreateInvalidStrategyConfigurations())));

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(
    ComposeLeAudioCodecCapabilitiesTest);
INSTANTIATE_TEST_SUITE_P(
    BluetoothLeAudioCodecsProviderTest, ComposeLeAudioCodecCapabilitiesTest,
    ::testing::ValuesIn(BluetoothLeAudioCodecsProviderTest::CreateTestCases(
        kValidScenarioList, kValidConfigurationList,
        kValidCodecConfigurationList, kValidStrategyConfigurationList)));

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(
    ComposeLeAudioAymmetricCodecInfoTest);
INSTANTIATE_TEST_SUITE_P(
    BluetoothLeAudioCodecsProviderTest, ComposeLeAudioAymmetricCodecInfoTest,
    ::testing::ValuesIn(BluetoothLeAudioCodecsProviderTest::CreateTestCases(
        kValidAsymmetricScenarioList, kValidAsymmetricConfigurationList,
        kValidAsymmetricCodecConfigurationList,
        kValidStrategyConfigurationList)));

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

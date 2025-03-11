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

//! HwCryptoKey tests.

use android_hardware_security_see_hwcrypto::aidl::android::hardware::security::see::hwcrypto::IHwCryptoKey::{
    DerivedKeyParameters::DerivedKeyParameters, DerivedKeyPolicy::DerivedKeyPolicy,
    DiceBoundDerivationKey::DiceBoundDerivationKey, KeySlot::KeySlot,
};
use android_hardware_security_see_hwcrypto::aidl::android::hardware::security::see::hwcrypto::types::{
    HalErrorCode, AesKey::AesKey, ExplicitKeyMaterial::ExplicitKeyMaterial, KeyType::KeyType, KeyLifetime::KeyLifetime, KeyUse::KeyUse,
    HmacKey::HmacKey, ProtectionId::ProtectionId,
};
use android_hardware_security_see_hwcrypto::aidl::android::hardware::security::see::hwcrypto::KeyPolicy::KeyPolicy;
use hwcryptohal_common;
use rdroidtest::{ignore_if, rdroidtest};

#[rdroidtest]
#[ignore_if(hwcryptohal_vts_test::ignore_test())]
fn test_hwcrypto_key_connection() {
    let hw_crypto_key = hwcryptohal_vts_test::get_hwcryptokey();
    assert!(hw_crypto_key.is_ok(), "Couldn't get back a hwcryptokey binder object");
}

#[rdroidtest]
#[ignore_if(hwcryptohal_vts_test::ignore_test())]
fn test_hwcrypto_key_get_current_dice_policy() {
    let hw_crypto_key = hwcryptohal_vts_test::get_hwcryptokey()
        .expect("Couldn't get back a hwcryptokey binder object");
    let dice_policy = hw_crypto_key.getCurrentDicePolicy().expect("Couldn't get dice policy back");
    assert!(!dice_policy.is_empty(), "received empty dice policy");
}

#[rdroidtest]
#[ignore_if(hwcryptohal_vts_test::ignore_test())]
fn test_hwcrypto_get_keyslot_data() {
    let hw_crypto_key = hwcryptohal_vts_test::get_hwcryptokey()
        .expect("Couldn't get back a hwcryptokey binder object");
    let key = hw_crypto_key.getKeyslotData(KeySlot::KEYMINT_SHARED_HMAC_KEY);
    assert_eq!(
        key.err()
            .expect("should not be able to access this keylost from the host")
            .service_specific_error(),
        HalErrorCode::UNAUTHORIZED,
        "wrong error type received"
    );
}

#[rdroidtest]
#[ignore_if(hwcryptohal_vts_test::ignore_test())]
fn test_hwcrypto_import_clear_key() {
    let hw_crypto_key = hwcryptohal_vts_test::get_hwcryptokey()
        .expect("Couldn't get back a hwcryptokey binder object");
    let clear_key = ExplicitKeyMaterial::Aes(AesKey::Aes128([0; 16]));
    let mut policy = KeyPolicy {
        usage: KeyUse::ENCRYPT_DECRYPT,
        keyLifetime: KeyLifetime::PORTABLE,
        keyPermissions: Vec::new(),
        keyManagementKey: false,
        keyType: KeyType::AES_128_GCM,
    };
    let key = hw_crypto_key.importClearKey(&clear_key, &policy).expect("couldn't import key");
    assert!(key.getPublicKey().is_err(), "symmetric keys don't have a public key");
    let imported_policy = key.getKeyPolicy().expect("couldn't get key policy");
    let serialized_policy =
        hwcryptohal_common::key_policy_to_cbor(&policy).expect("couldn't serialize policy");
    let serialized_impoorted_policy = hwcryptohal_common::key_policy_to_cbor(&imported_policy)
        .expect("couldn't serialize policy");
    assert_eq!(serialized_policy, serialized_impoorted_policy, "policies should match");
    policy.keyLifetime = KeyLifetime::EPHEMERAL;
    let key = hw_crypto_key.importClearKey(&clear_key, &policy);
    assert!(key.is_err(), "imported keys should be of type PORTABLE");
    policy.keyLifetime = KeyLifetime::HARDWARE;
    let key = hw_crypto_key.importClearKey(&clear_key, &policy);
    assert!(key.is_err(), "imported keys should be of type PORTABLE");
}

#[rdroidtest]
#[ignore_if(hwcryptohal_vts_test::ignore_test())]
fn test_hwcrypto_token_export_import() {
    // This test is not representative of the complete flow because here the exporter and importer
    // are the same client, which is not something we would usually do
    let hw_crypto_key = hwcryptohal_vts_test::get_hwcryptokey()
        .expect("Couldn't get back a hwcryptokey binder object");
    let clear_key = ExplicitKeyMaterial::Hmac(HmacKey::Sha256([0; 32]));
    let policy = KeyPolicy {
        usage: KeyUse::DERIVE,
        keyLifetime: KeyLifetime::PORTABLE,
        keyPermissions: Vec::new(),
        keyManagementKey: false,
        keyType: KeyType::HMAC_SHA256,
    };
    let key = hw_crypto_key.importClearKey(&clear_key, &policy).expect("couldn't import clear key");
    let dice_policy = hw_crypto_key.getCurrentDicePolicy().expect("Couldn't get dice policy back");
    let token =
        key.getShareableToken(dice_policy.as_slice()).expect("Couldn't get shareable token");
    let imported_key = hw_crypto_key
        .keyTokenImport(&token, dice_policy.as_slice());
    assert!(imported_key.is_ok(), "Couldn't import shareable token");
    // TODO: Use operations to verify that the keys match
}

#[rdroidtest]
#[ignore_if(hwcryptohal_vts_test::ignore_test())]
fn test_hwcrypto_android_invalid_calls() {
    let hw_crypto_key = hwcryptohal_vts_test::get_hwcryptokey()
        .expect("Couldn't get back a hwcryptokey binder object");
    let clear_key = ExplicitKeyMaterial::Hmac(HmacKey::Sha256([0; 32]));
    let policy = KeyPolicy {
        usage: KeyUse::DERIVE,
        keyLifetime: KeyLifetime::PORTABLE,
        keyPermissions: Vec::new(),
        keyManagementKey: false,
        keyType: KeyType::HMAC_SHA256,
    };
    let key = hw_crypto_key.importClearKey(&clear_key, &policy).expect("couldn't import clear key");
    let protections = Vec::new();
    let res = key.setProtectionId(ProtectionId::WIDEVINE_OUTPUT_BUFFER, &protections);
    assert_eq!(
        res.err()
            .expect("should not be call this function from the host")
            .service_specific_error(),
        HalErrorCode::UNAUTHORIZED,
        "wrong error type received"
    );
    let derivation_key = DiceBoundDerivationKey::OpaqueKey(Some(key));
    let res = hw_crypto_key.deriveCurrentDicePolicyBoundKey(&derivation_key);
    assert_eq!(
        res.err()
            .expect("should not be call this function from the host")
            .service_specific_error(),
        HalErrorCode::UNAUTHORIZED,
        "wrong error type received"
    );
    let fake_policy = Vec::new();
    let res = hw_crypto_key.deriveDicePolicyBoundKey(&derivation_key, &fake_policy);
    assert_eq!(
        res.err()
            .expect("should not be call this function from the host")
            .service_specific_error(),
        HalErrorCode::UNAUTHORIZED,
        "wrong error type received"
    );
    let key = hw_crypto_key.importClearKey(&clear_key, &policy).expect("couldn't import clear key");
    let derived_policy = DerivedKeyPolicy::OpaqueKey(Vec::new());
    let derived_parameters = DerivedKeyParameters {
        derivationKey: Some(key),
        keyPolicy: derived_policy,
        context: Vec::new(),
    };
    let res = hw_crypto_key.deriveKey(&derived_parameters);
    assert_eq!(
        res.err()
            .expect("should not be call this function from the host")
            .service_specific_error(),
        HalErrorCode::UNAUTHORIZED,
        "wrong error type received"
    );
}

rdroidtest::test_main!();

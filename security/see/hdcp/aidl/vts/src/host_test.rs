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

//! Test for asserting the non-existence of an IHdcpAuthControl.aidl

#![cfg(test)]

use binder;

const HDCP_INTERFACE_NAME: &str = "android.hardware.security.see.hdcp.IHdcpAuthControl";

#[test]
fn test_hdcp_auth_control_non_existence() {
    let hdcp_instances =  match binder::get_declared_instances(HDCP_INTERFACE_NAME) {
        Ok(vec) => vec,
        Err(e) => {
            panic!("failed to retrieve the declared interfaces for HdcpAuthControl: {:?}", e);
        }
    };
    assert!(hdcp_instances.is_empty());
}

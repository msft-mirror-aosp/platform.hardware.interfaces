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

package android.hardware.radio.network;
import android.hardware.radio.AccessNetwork;
import android.hardware.radio.network.EmergencyScanType;

/** @hide */
@VintfStability
@JavaDerive(toString=true)
parcelable EmergencyNetworkScanTrigger {
    /**
     * Access network to be prioritized during emergency scan. The 1st entry has the highest
     * priority.
     */
    AccessNetwork[] accessNetwork;

    /**
     * Scan type indicates the type of scans to be performed i.e. limited scan, full service scan or
     * any scan.
     */
    EmergencyScanType scanType = EmergencyScanType.NO_PREFERENCE;
}

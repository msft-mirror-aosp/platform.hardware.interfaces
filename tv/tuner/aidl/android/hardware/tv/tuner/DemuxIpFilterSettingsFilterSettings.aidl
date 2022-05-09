/*
 * Copyright 2021 The Android Open Source Project
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

package android.hardware.tv.tuner;

import android.hardware.tv.tuner.DemuxFilterSectionSettings;

/**
 * @hide
 */
@VintfStability
union DemuxIpFilterSettingsFilterSettings {
    /**
     * Not additional parameters. it's used by NTP, IP_PAYLOAD,
     * PAYLOAD_THROUGH subtype filters.
     */
    boolean noinit;

    DemuxFilterSectionSettings section;

    /**
     * true if the data from IP subtype go to next filter directly
     */
    boolean bPassthrough;
}

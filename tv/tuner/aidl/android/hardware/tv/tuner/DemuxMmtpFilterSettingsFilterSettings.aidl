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

import android.hardware.tv.tuner.DemuxFilterAvSettings;
import android.hardware.tv.tuner.DemuxFilterDownloadSettings;
import android.hardware.tv.tuner.DemuxFilterPesDataSettings;
import android.hardware.tv.tuner.DemuxFilterRecordSettings;
import android.hardware.tv.tuner.DemuxFilterSectionSettings;

/**
 * The different types of MMTP Filter Settings that can be set by client.
 * @hide
 */
@VintfStability
union DemuxMmtpFilterSettingsFilterSettings {
    /**
     * Not additional parameters. it's used by MMTP subtype filters.
     */
    boolean noinit;

    DemuxFilterSectionSettings section;

    DemuxFilterAvSettings av;

    DemuxFilterPesDataSettings pesData;

    DemuxFilterRecordSettings record;

    DemuxFilterDownloadSettings download;
}

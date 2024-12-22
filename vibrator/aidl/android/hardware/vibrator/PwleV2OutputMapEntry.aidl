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

package android.hardware.vibrator;

@VintfStability
parcelable PwleV2OutputMapEntry {
    /**
     * Absolute frequency point in the units of hertz
     *
     */
    float frequencyHz;

    /**
     * Max output acceleration for the specified frequency in units of Gs.
     *
     * This value represents the maximum safe output acceleration (in Gs) achievable at the
     * specified frequency, typically determined during calibration. The actual output acceleration
     * is assumed to scale linearly with the input amplitude within the range of [0, 1].
     */
    float maxOutputAccelerationGs;
}

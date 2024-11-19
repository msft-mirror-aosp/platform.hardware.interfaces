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

package android.hardware.tv.mediaquality;

import android.hardware.tv.mediaquality.AmbientBacklightSettings;
import android.hardware.tv.mediaquality.IMediaQualityCallback;
import android.hardware.tv.mediaquality.IPictureProfileAdjustmentListener;
import android.hardware.tv.mediaquality.IPictureProfileChangedListener;
import android.hardware.tv.mediaquality.ISoundProfileAdjustmentListener;
import android.hardware.tv.mediaquality.ISoundProfileChangedListener;
import android.hardware.tv.mediaquality.PictureParameters;
import android.hardware.tv.mediaquality.SoundParameters;

/**
 * Interface for the media quality service
 */
@VintfStability
interface IMediaQuality {
    /**
     * Sets a callback for events.
     *
     * @param callback Callback object to pass events.
     */
    void setCallback(in IMediaQualityCallback callback);

    /**
     * Sets the ambient backlight detector settings.
     *
     * @param settings Ambient backlight detector settings.
     */
    void setAmbientBacklightDetector(in AmbientBacklightSettings settings);

    /**
     * Sets the ambient backlight detection enabled or disabled. The ambient backlight is the
     * projection of light against the wall driven by the current content playing. Enable will
     * detects the Ambient backlight metadata and ambient control app can control the related
     * device as configured before.
     *
     * @param enabled True to enable the ambient backlight detection, false to disable.
     */
    void setAmbientBacklightDetectionEnabled(in boolean enabled);

    /**
     * Gets the ambient backlight detection enabled status. The ambient backlight is enabled by
     * calling setAmbientBacklightDetectionEnabled(in boolean enabled). True to enable the
     * ambient light detection and False to disable the ambient backlight detection.
     *
     * @return True if the ambient backlight detection is enabled, false otherwise.
     */
    boolean getAmbientBacklightDetectionEnabled();

    /**
     * Get picture profile changed listener.
     *
     * @return the IPictureProfileChangedListener.
     */
    IPictureProfileChangedListener getPictureProfileListener();

    /**
     * Sets the listener for picture adjustment from the HAL.
     *
     * @param IPictureProfileAdjustmentListener listener object to pass picture profile.
     */
    void setPictureProfileAdjustmentListener(IPictureProfileAdjustmentListener listener);

    /**
     * Get the picture parameters by PictureProfile id. Check PictureParameters for its' detail.
     *
     * @param pictureProfileId The PictureProfile id that associate with the PictureProfile.
     * @return PictureParameters with all the pre-defined parameters and vendor defined parameters.
     */
    PictureParameters getPictureParameters(long pictureProfileId);

    /**
     * Get sound profile changed listener.
     *
     * @return the ISoundProfileChangedListener.
     */
    ISoundProfileChangedListener getSoundProfileListener();

    /**
     * Sets the listener for sound adjustment from the HAL.
     *
     * @param ISoundProfileAdjustmentListener listener object to pass sound profile.
     */
    void setSoundProfileAdjustmentListener(ISoundProfileAdjustmentListener listener);

    /**
     * Get the sound parameters by SoundProfile id. Check SoundParameters for its' detail.
     *
     * @param soundProfileId The SoundProfile id that associate with a SoundProfile.
     * @return SoundParameters with all the pre-defined parameters and vendor defined parameters.
     */
    SoundParameters getSoundParameters(long soundProfileId);
}

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

import android.hardware.tv.mediaquality.ParamCapability;
import android.hardware.tv.mediaquality.PictureProfile;
import android.hardware.tv.mediaquality.VendorParamCapability;

@VintfStability
oneway interface IPictureProfileAdjustmentListener {
    /**
     * Notifies Media Quality Manager when the picture profile changed.
     *
     * @param pictureProfile Picture profile.
     */
    void onPictureProfileAdjusted(in PictureProfile pictureProfile);

    /**
     * Notifies Media Quality Manager when parameter capabilities changed.
     *
     * @param pictureProfileId the ID of the profile used by the media content. -1 if there
     *                         is no associated profile.
     * @param caps the updated capabilities.
     */
    void onParamCapabilityChanged(long pictureProfileId, in ParamCapability[] caps);

    /**
     * Notifies Media Quality Manager when vendor parameter capabilities changed.
     *
     * <p>This should be also called when the listener is registered to let the client know
     * what vendor parameters are supported.
     *
     * @param pictureProfileId the ID of the profile used by the media content. -1 if there
     *                         is no associated profile.
     * @param caps the updated vendor capabilities.
     */
    void onVendorParamCapabilityChanged(long pictureProfileId, in VendorParamCapability[] caps);

    /**
     * Request the picture parameters by picture profile id. Check PictureParameters for its detail.
     * This is called from the HAL to media quality framework.
     *
     * The requested picture parameters will get from IMediaQuality::sendPictureParameters called
     * by the framework.
     *
     * @param pictureProfileId The PictureProfile id that associate with the PictureProfile.
     */
    void requestPictureParameters(long pictureProfileId);
}

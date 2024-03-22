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

package android.hardware.biometrics.fingerprint;

import android.hardware.biometrics.common.SensorStrength;
import android.hardware.biometrics.fingerprint.FingerprintSensorType;
import android.hardware.biometrics.fingerprint.SensorLocation;

/**
 * @hide
 */
@VintfStability
oneway interface IVirtualHal {
    /**
     * The operation failed due to invalid input parameters, the error messages should
     * gives more details
     */
    const int STATUS_INVALID_PARAMETER = 1;

    /**
     * Set Fingerprint Virtual HAL behavior parameters
     */

    /**
     * setEnrollments
     *
     * Set the ids of the fingerprints that were currently enrolled in the Virtual HAL,
     *
     * @param ids ids can contain 1 or more ids, each must be larger than 0
     */
    void setEnrollments(in int[] id);

    /**
     * setEnrollmentHit
     *
     * Set current fingerprint enrollment ids in Fingerprint Virtual HAL,
     *
     * @param ids ids can contain 1 or more ids, each must be larger than 0
     */
    void setEnrollmentHit(in int hit_id);

    /**
     * setAuthenticatorId
     *
     * Set authenticator id in virtual HAL, the id is returned in ISession#getAuthenticatorId() call
     *
     * @param id authenticator id value, only applied to the sensor with SensorStrength::STRONG.
     */
    void setAuthenticatorId(in long id);

    /**
     * setChallenge
     *
     * Set the challenge generated by the virtual HAL, which is returned in
     * ISessionCallback#onChallengeGenerated()
     *
     * @param challenge
     */
    void setChallenge(in long challenge);

    /**
     * setOperationAuthenticateFails
     *
     * Set whether to force authentication to fail. If true, the virtual hal will report failure on
     * authentication attempt until it is set to false
     *
     * @param fail  if true, then the next authentication will fail
     */
    void setOperationAuthenticateFails(in boolean fail);

    /**
     * setOperationAuthenticateLatency
     *
     * Set authentication latency in the virtual hal in a fixed value (single element) or random
     * values (two elements representing the bound values)
     * The latency simulates the delay from the time framework requesting HAL to authetication to
     * the time when HAL is ready to perform authentication operations.
     *
     * This method fails with STATUS_INVALID_PARAMETERS if the passed-in array falls in any of
     * the following conditions
     *   1. the array contains no element
     *   2. the array contains more than two elements
     *   3. the array contains any negative value
     * The accompanying error message gives more detail
     *
     * @param latencyMs[]  value(s) are in milli-seconds
     */
    void setOperationAuthenticateLatency(in int[] latencyMs);

    /**
     * setOperationAuthenticateDuration
     *
     * Set authentication duration covering the HAL authetication from start to end, including
     * fingerprint capturing, and matching, acquired info reporting. In case a sequence of acquired
     * info code are specified via setOperationAuthenticateAcquired(), the reporting is evenly
     * distributed over the duration.
     *
     * This method fails with STATUS_INVALID_PARAMETERS if the passed-in value is negative
     *
     * @param duration  value is in milli-seconds
     */
    void setOperationAuthenticateDuration(in int durationMs);

    /**
     * setOperationAuthenticateError
     *
     * Force authentication to error out for non-zero error
     * Check hardware/interfaces/biometrics/fingerprint/aidl/default/README.md for valid error codes
     *
     * @param error if error < 1000
     *                  non-vendor error
     *              else
     *                  vendor error
     */
    void setOperationAuthenticateError(in int error);

    /**
     * setOperationAuthenticateAcquired
     *
     * Set one of more acquired info codes for the virtual hal to report during authentication
     * Check hardware/interfaces/biometrics/fingerprint/aidl/default/README.md for valid acquired
     * info codes
     *
     * @param acquired[], one or more acquired info codes
     */
    void setOperationAuthenticateAcquired(in int[] acquired);

    /**
     * setOperationEnrollError
     *
     * Force enrollment operation to error out for non-zero error
     * Check hardware/interfaces/biometrics/fingerprint/aidl/default/README.md for valid error codes
     *
     * @param error if error < 1000
     *                  non-vendor error
     *              else
     *                  vendor error
     */
    void setOperationEnrollError(in int error);

    /**
     * setOperationEnrollLatency
     *
     * Set enrollment latency in the virtual hal in a fixed value (single element) or random
     * values (two elements representing the bound values)
     * The latency simulates the delay from the time framework requesting HAL to enroll to the
     * time when HAL is ready to perform enrollment operations.
     *
     * This method fails with STATUS_INVALID_PARAMETERS if the passed-in array falls in any of
     * the following conditions
     *   1. the array contains no element
     *   2. the array contains more than two elements
     *   3. the array contains any negative value
     * The accompanying error message gives more detail
     *
     * @param latencyMs[]  value(s) are in milli-seconds
     */
    void setOperationEnrollLatency(in int[] latencyMs);

    /**
     * setOperationDetectInteractionLatency
     *
     * Set detect interaction latency in the virtual hal in a fixed value (single element) or random
     * values (two elements representing the bound values)
     * The latency simulates the delay from the time framework requesting HAL to detect interaction
     * to the time when HAL is ready to perform detect interaction operations.
     *
     * This method fails with STATUS_INVALID_PARAMETERS if the passed-in array falls in any of
     * the following conditions
     *   1. the array contains no element
     *   2. the array contains more than two elements
     *   3. the array contains any negative value
     * The accompanying error message gives more detail
     *
     * @param latencyMs[]  value(s) are in milli-seconds
     */
    void setOperationDetectInteractionLatency(in int[] latencyMs);

    /**
     * setOperationDetectInteractionError
     *
     * Force detect interaction operation to error out for non-zero error
     * Check hardware/interfaces/biometrics/fingerprint/aidl/default/README.md for valid error codes
     *
     * @param error if error < 1000
     *                  non-vendor error
     *              else
     *                  vendor error
     */
    void setOperationDetectInteractionError(in int error);

    /**
     * setOperationDetectInteractionDuration
     *
     * Set detect interaction duration covering the HAL authetication from start to end, including
     * fingerprint detect and acquired info reporting. In case a sequence of acquired info code are
     * specified via setOperationDetectInteractionAcquired(), the reporting is evenly distributed
     * over the duration.
     *
     * This method fails with STATUS_INVALID_PARAMETERS if the passed-in value is negative
     *
     * @param duration  value is in milli-seconds
     */
    void setOperationDetectInteractionDuration(in int durationMs);

    /**
     * setOperationDetectInteractionAcquired
     *
     * Set one of more acquired info codes for the virtual hal to report during detect interaction
     * Check hardware/interfaces/biometrics/fingerprint/aidl/default/README.md for valid acquired
     * info codes
     *
     * @param acquired[], one or more acquired info codes
     */
    void setOperationDetectInteractionAcquired(in int[] acquired);

    /**
     * setLockout
     *
     * Whether to force to lockout on authentcation operation. If true, the virtual hal will report
     * permanent lockout in processing authentication requrest, regardless of whether
     * setLockoutEnable(true) is called or not.
     *
     * @param lockout, set to true if lockout is desired
     */
    void setLockout(in boolean lockout);

    /**
     * setLockoutEnable
     *
     * Whether to enable authentication-fail-based lockout tracking or not. The lock tracking
     * includes both timed-based (aka temporary) lockout and permanent lockout.
     *
     * @param enable, set true to enable the lockout tracking
     */
    void setLockoutEnable(in boolean enable);

    /**
     * setLockoutTimedThreshold
     *
     * Set the number of consecutive authentication failures that triggers the timed-based lock to
     * occur
     *
     * This method fails with STATUS_INVALID_PARAMETERS if the passed-in value is negative
     *
     * @param threshold, the number of consecutive failures
     */
    void setLockoutTimedThreshold(in int threshold);

    /**
     * setLockoutTimedDuration
     *
     * Set the duration to expire timed-based lock during which there is no authentication failure
     *
     * This method fails with STATUS_INVALID_PARAMETERS if the passed-in value is negative
     *
     * @param duration, in milli-seconds
     */
    void setLockoutTimedDuration(in int durationMs);

    /**
     * setLockoutPermanentThreshold
     *
     * Set the number of consecutive authentication failures that triggers the permanent lock to
     * occur
     *
     * This method fails with STATUS_INVALID_PARAMETERS if the passed-in value is negative
     *
     * @param threshold, the number of consecutive failures
     */
    void setLockoutPermanentThreshold(in int threshold);

    /**
     * The following functions are used to configure Fingerprint Virtual HAL sensor properties
     *  refer to SensorProps.aidl and CommonProps.aidl for details of each property
     */
    void setType(in FingerprintSensorType type);
    void setSensorId(in int id);
    void setSensorStrength(in SensorStrength strength);
    void setMaxEnrollmentPerUser(in int max);
    void setSensorLocation(in SensorLocation loc);
    void setNavigationGuesture(in boolean v);
    void setDetectInteraction(in boolean v);
    void setDisplayTouch(in boolean v);
    void setControlIllumination(in boolean v);
}

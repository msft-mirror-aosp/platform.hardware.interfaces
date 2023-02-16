/*
 * Copyright (C) 2023 The Android Open Source Project
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

package android.hardware.automotive.vehicle;

/**
 * Used to enumerate the current warning state of Driver Attention Monitoring.
 */
@VintfStability
@Backing(type="int")
enum DriverAttentionMonitoringWarning {
    /**
     * This state is used as an alternative for any DriverAttentionMonitoringWarning value that is
     * defined in the platform. Ideally, implementations of
     * VehicleProperty#DRIVER_ATTENTION_MONITORING_WARNING should not use this state. The framework
     * can use this field to remain backwards compatible if DriverAttentionMonitoringWarning is
     * extended to include additional states.
     */
    OTHER = 0,
    /**
     * Driver Attention Monitoring is enabled and the driver's current state does not warrant
     * sending a warning.
     */
    NO_WARNING = 1,
    /**
     * Driver Attention Monitoring is enabled and the driver has been distracted for too long of a
     * duration, and the vehicle is sending a warning to the driver as a consequence of this.
     */
    WARNING = 2,
}

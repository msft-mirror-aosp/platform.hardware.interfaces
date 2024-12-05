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
 *//*
 * Autogenerated from camera metadata definitions in
 * /system/media/camera/docs/metadata_definitions.xml
 * *** DO NOT EDIT BY HAND ***
 */
///////////////////////////////////////////////////////////////////////////////
// THIS FILE IS IMMUTABLE. DO NOT EDIT IN ANY CASE.                          //
///////////////////////////////////////////////////////////////////////////////

// This file is a snapshot of an AIDL file. Do not edit it manually. There are
// two cases:
// 1). this is a frozen version file - do not edit this in any case.
// 2). this is a 'current' file. If you make a backwards compatible change to
//     the interface (from the latest frozen version), the build system will
//     prompt you to update this file with `m <name>-update-api`.
//
// You must not make a backward incompatible change to any AIDL file built
// with the aidl_interface module type with versions property set. The module
// type is used to build AIDL files in a way that they can be used across
// independently updatable components of the system. If a device is shipped
// with such a backward incompatible change, it has a high risk of breaking
// later when a module using the interface is updated, e.g., Mainline modules.

package android.hardware.camera.metadata;
@Backing(type="int") @VintfStability
enum CameraMetadataSectionStart {
  ANDROID_COLOR_CORRECTION_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_COLOR_CORRECTION << 16) /* 0 */,
  ANDROID_CONTROL_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_CONTROL << 16) /* 65536 */,
  ANDROID_DEMOSAIC_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_DEMOSAIC << 16) /* 131072 */,
  ANDROID_EDGE_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_EDGE << 16) /* 196608 */,
  ANDROID_FLASH_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_FLASH << 16) /* 262144 */,
  ANDROID_FLASH_INFO_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_FLASH_INFO << 16) /* 327680 */,
  ANDROID_HOT_PIXEL_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_HOT_PIXEL << 16) /* 393216 */,
  ANDROID_JPEG_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_JPEG << 16) /* 458752 */,
  ANDROID_LENS_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_LENS << 16) /* 524288 */,
  ANDROID_LENS_INFO_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_LENS_INFO << 16) /* 589824 */,
  ANDROID_NOISE_REDUCTION_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_NOISE_REDUCTION << 16) /* 655360 */,
  ANDROID_QUIRKS_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_QUIRKS << 16) /* 720896 */,
  ANDROID_REQUEST_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_REQUEST << 16) /* 786432 */,
  ANDROID_SCALER_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_SCALER << 16) /* 851968 */,
  ANDROID_SENSOR_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_SENSOR << 16) /* 917504 */,
  ANDROID_SENSOR_INFO_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_SENSOR_INFO << 16) /* 983040 */,
  ANDROID_SHADING_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_SHADING << 16) /* 1048576 */,
  ANDROID_STATISTICS_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_STATISTICS << 16) /* 1114112 */,
  ANDROID_STATISTICS_INFO_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_STATISTICS_INFO << 16) /* 1179648 */,
  ANDROID_TONEMAP_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_TONEMAP << 16) /* 1245184 */,
  ANDROID_LED_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_LED << 16) /* 1310720 */,
  ANDROID_INFO_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_INFO << 16) /* 1376256 */,
  ANDROID_BLACK_LEVEL_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_BLACK_LEVEL << 16) /* 1441792 */,
  ANDROID_SYNC_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_SYNC << 16) /* 1507328 */,
  ANDROID_REPROCESS_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_REPROCESS << 16) /* 1572864 */,
  ANDROID_DEPTH_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_DEPTH << 16) /* 1638400 */,
  ANDROID_LOGICAL_MULTI_CAMERA_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_LOGICAL_MULTI_CAMERA << 16) /* 1703936 */,
  ANDROID_DISTORTION_CORRECTION_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_DISTORTION_CORRECTION << 16) /* 1769472 */,
  ANDROID_HEIC_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_HEIC << 16) /* 1835008 */,
  ANDROID_HEIC_INFO_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_HEIC_INFO << 16) /* 1900544 */,
  ANDROID_AUTOMOTIVE_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_AUTOMOTIVE << 16) /* 1966080 */,
  ANDROID_AUTOMOTIVE_LENS_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_AUTOMOTIVE_LENS << 16) /* 2031616 */,
  ANDROID_EXTENSION_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_EXTENSION << 16) /* 2097152 */,
  ANDROID_JPEGR_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_JPEGR << 16) /* 2162688 */,
  ANDROID_SHARED_SESSION_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_SHARED_SESSION << 16) /* 2228224 */,
  ANDROID_DESKTOP_EFFECTS_START = (android.hardware.camera.metadata.CameraMetadataSection.ANDROID_DESKTOP_EFFECTS << 16) /* 2293760 */,
  VENDOR_SECTION_START = (android.hardware.camera.metadata.CameraMetadataSection.VENDOR_SECTION << 16) /* -2147483648 */,
}

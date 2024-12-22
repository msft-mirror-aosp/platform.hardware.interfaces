/**
 * Copyright (c) 2021, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ReadbackVts.h"
#include <aidl/android/hardware/graphics/common/BufferUsage.h>
#include <cmath>
#include "RenderEngineVts.h"
#include "renderengine/ExternalTexture.h"
#include "renderengine/impl/ExternalTexture.h"

namespace aidl::android::hardware::graphics::composer3::vts {

const std::vector<ColorMode> ReadbackHelper::colorModes = {ColorMode::SRGB, ColorMode::DISPLAY_P3};
const std::vector<Dataspace> ReadbackHelper::dataspaces = {common::Dataspace::SRGB,
                                                           common::Dataspace::DISPLAY_P3};

void TestLayer::write(ComposerClientWriter& writer) {
    writer.setLayerDisplayFrame(mDisplay, mLayer, mDisplayFrame);
    writer.setLayerSourceCrop(mDisplay, mLayer, mSourceCrop);
    writer.setLayerZOrder(mDisplay, mLayer, mZOrder);
    writer.setLayerSurfaceDamage(mDisplay, mLayer, mSurfaceDamage);
    writer.setLayerTransform(mDisplay, mLayer, mTransform);
    writer.setLayerPlaneAlpha(mDisplay, mLayer, mAlpha);
    writer.setLayerBlendMode(mDisplay, mLayer, mBlendMode);
    writer.setLayerBrightness(mDisplay, mLayer, mBrightness);
    writer.setLayerDataspace(mDisplay, mLayer, mDataspace);
}

std::string ReadbackHelper::getColorModeString(ColorMode mode) {
    switch (mode) {
        case ColorMode::SRGB:
            return {"SRGB"};
        case ColorMode::DISPLAY_P3:
            return {"DISPLAY_P3"};
        default:
            return {"Unsupported color mode for readback"};
    }
}

std::string ReadbackHelper::getDataspaceString(common::Dataspace dataspace) {
    switch (dataspace) {
        case common::Dataspace::SRGB:
            return {"SRGB"};
        case common::Dataspace::DISPLAY_P3:
            return {"DISPLAY_P3"};
        case common::Dataspace::UNKNOWN:
            return {"UNKNOWN"};
        default:
            return {"Unsupported dataspace for readback"};
    }
}

Dataspace ReadbackHelper::getDataspaceForColorMode(ColorMode mode) {
    switch (mode) {
        case ColorMode::DISPLAY_P3:
            return Dataspace::DISPLAY_P3;
        case ColorMode::SRGB:
            return Dataspace::SRGB;
        default:
            return Dataspace::UNKNOWN;
    }
}

LayerSettings TestLayer::toRenderEngineLayerSettings() {
    LayerSettings layerSettings;

    layerSettings.alpha = ::android::half(mAlpha);
    layerSettings.disableBlending = mBlendMode == BlendMode::NONE;
    layerSettings.source.buffer.isOpaque = mBlendMode == BlendMode::NONE;
    layerSettings.geometry.boundaries = ::android::FloatRect(
            static_cast<float>(mDisplayFrame.left), static_cast<float>(mDisplayFrame.top),
            static_cast<float>(mDisplayFrame.right), static_cast<float>(mDisplayFrame.bottom));

    const ::android::mat4 translation = ::android::mat4::translate(::android::vec4(
            (static_cast<uint64_t>(mTransform) & static_cast<uint64_t>(Transform::FLIP_H)
                     ? static_cast<float>(-mDisplayFrame.right)
                     : 0.0f),
            (static_cast<uint64_t>(mTransform) & static_cast<uint64_t>(Transform::FLIP_V)
                     ? static_cast<float>(-mDisplayFrame.bottom)
                     : 0.0f),
            0.0f, 1.0f));

    const ::android::mat4 scale = ::android::mat4::scale(::android::vec4(
            static_cast<uint64_t>(mTransform) & static_cast<uint64_t>(Transform::FLIP_H) ? -1.0f
                                                                                         : 1.0f,
            static_cast<uint64_t>(mTransform) & static_cast<uint64_t>(Transform::FLIP_V) ? -1.0f
                                                                                         : 1.0f,
            1.0f, 1.0f));

    layerSettings.geometry.positionTransform = scale * translation;
    layerSettings.whitePointNits = mWhitePointNits;
    layerSettings.sourceDataspace = static_cast<::android::ui::Dataspace>(mDataspace);

    return layerSettings;
}

int32_t ReadbackHelper::GetBitsPerChannel(common::PixelFormat pixelFormat) {
    switch (pixelFormat) {
        case common::PixelFormat::RGBA_1010102:
            return 10;
        case common::PixelFormat::RGBA_8888:
        case common::PixelFormat::RGB_888:
            return 8;
        default:
            return -1;
    }
}

int32_t ReadbackHelper::GetAlphaBits(common::PixelFormat pixelFormat) {
    switch (pixelFormat) {
        case common::PixelFormat::RGBA_8888:
            return 8;
        case common::PixelFormat::RGBA_1010102:
            return 2;
        case common::PixelFormat::RGB_888:
            return 0;
        default:
            return -1;
    }
}

void ReadbackHelper::fillBuffer(uint32_t width, uint32_t height, uint32_t stride,
                                int32_t bytesPerPixel, void* bufferData,
                                common::PixelFormat pixelFormat,
                                std::vector<Color> desiredPixelColors) {
    ASSERT_TRUE(pixelFormat == common::PixelFormat::RGB_888 ||
                pixelFormat == common::PixelFormat::RGBA_8888 ||
                pixelFormat == common::PixelFormat::RGBA_1010102);
    int32_t bitsPerChannel = GetBitsPerChannel(pixelFormat);
    int32_t alphaBits = GetAlphaBits(pixelFormat);
    ASSERT_NE(-1, alphaBits);
    ASSERT_NE(-1, bitsPerChannel);
    ASSERT_NE(-1, bytesPerPixel);

    uint32_t maxValue = (1 << bitsPerChannel) - 1;
    uint32_t maxAlphaValue = (1 << alphaBits) - 1;
    for (uint32_t row = 0; row < height; row++) {
        for (uint32_t col = 0; col < width; col++) {
            auto pixel = row * width + col;
            Color srcColor = desiredPixelColors[static_cast<size_t>(pixel)];

            uint32_t offset = (row * stride + col) * static_cast<uint32_t>(bytesPerPixel);

            uint32_t* pixelStart = (uint32_t*)((uint8_t*)bufferData + offset);

            uint32_t red = static_cast<uint32_t>(std::round(maxValue * srcColor.r));
            uint32_t green = static_cast<uint32_t>(std::round(maxValue * srcColor.g));
            uint32_t blue = static_cast<uint32_t>(std::round(maxValue * srcColor.b));

            // Boo we're not word aligned so special case this.
            if (pixelFormat == common::PixelFormat::RGB_888) {
                uint8_t* pixelColor = (uint8_t*)pixelStart;
                pixelColor[0] = static_cast<uint8_t>(red);
                pixelColor[1] = static_cast<uint8_t>(green);
                pixelColor[2] = static_cast<uint8_t>(blue);
            } else {
                uint32_t alpha = static_cast<uint32_t>(std::round(maxAlphaValue * srcColor.a));
                uint32_t color = (alpha << (32 - alphaBits)) |
                                 (blue << (32 - alphaBits - bitsPerChannel)) |
                                 (green << (32 - alphaBits - bitsPerChannel * 2)) |
                                 (red << (32 - alphaBits - bitsPerChannel * 3));
                *pixelStart = color;
            }
        }
    }
}

void ReadbackHelper::clearColors(std::vector<Color>& expectedColors, int32_t width, int32_t height,
                                 int32_t displayWidth) {
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int pixel = row * displayWidth + col;
            expectedColors[static_cast<size_t>(pixel)] = BLACK;
        }
    }
}

void ReadbackHelper::fillColorsArea(std::vector<Color>& expectedColors, int32_t stride, Rect area,
                                    Color color) {
    for (int row = area.top; row < area.bottom; row++) {
        for (int col = area.left; col < area.right; col++) {
            int pixel = row * stride + col;
            expectedColors[static_cast<size_t>(pixel)] = color;
        }
    }
}

bool ReadbackHelper::readbackSupported(const common::PixelFormat& pixelFormat,
                                       const common::Dataspace& dataspace) {
    if (pixelFormat != common::PixelFormat::RGB_888 &&
        pixelFormat != common::PixelFormat::RGBA_8888 &&
        pixelFormat != common::PixelFormat::RGBA_1010102) {
        return false;
    }
    if (std::find(dataspaces.begin(), dataspaces.end(), dataspace) == dataspaces.end()) {
        return false;
    }
    return true;
}

void ReadbackHelper::compareColorBuffers(const std::vector<Color>& expectedColors, void* bufferData,
                                         const uint32_t stride, int32_t bytesPerPixel,
                                         const uint32_t width, const uint32_t height,
                                         common::PixelFormat pixelFormat) {
    int32_t bitsPerChannel = GetBitsPerChannel(pixelFormat);
    int32_t alphaBits = GetAlphaBits(pixelFormat);
    ASSERT_GT(bytesPerPixel, 0);
    ASSERT_NE(-1, alphaBits);
    ASSERT_NE(-1, bitsPerChannel);
    uint32_t maxValue = (1 << bitsPerChannel) - 1;
    uint32_t maxAlphaValue = (1 << alphaBits) - 1;
    for (uint32_t row = 0; row < height; row++) {
        for (uint32_t col = 0; col < width; col++) {
            auto pixel = row * width + col;
            const Color expectedColor = expectedColors[static_cast<size_t>(pixel)];

            uint32_t offset = (row * stride + col) * static_cast<uint32_t>(bytesPerPixel);
            uint32_t* pixelStart = (uint32_t*)((uint8_t*)bufferData + offset);

            uint32_t expectedRed = static_cast<uint32_t>(std::round(maxValue * expectedColor.r));
            uint32_t expectedGreen = static_cast<uint32_t>(std::round(maxValue * expectedColor.g));
            uint32_t expectedBlue = static_cast<uint32_t>(std::round(maxValue * expectedColor.b));

            // Boo we're not word aligned so special case this.
            if (pixelFormat == common::PixelFormat::RGB_888) {
                uint8_t* pixelColor = (uint8_t*)pixelStart;
                ASSERT_EQ(pixelColor[0], static_cast<uint8_t>(expectedRed))
                        << "Red channel mismatch at (" << row << ", " << col << ")";
                ASSERT_EQ(pixelColor[1], static_cast<uint8_t>(expectedGreen))
                        << "Green channel mismatch at (" << row << ", " << col << ")";
                ASSERT_EQ(pixelColor[2], static_cast<uint8_t>(expectedBlue))
                        << "Blue channel mismatch at (" << row << ", " << col << ")";
            } else {
                uint32_t expectedAlpha =
                        static_cast<uint32_t>(std::round(maxAlphaValue * expectedColor.a));

                uint32_t actualRed =
                        (*pixelStart >> (32 - alphaBits - bitsPerChannel * 3)) & maxValue;
                uint32_t actualGreen =
                        (*pixelStart >> (32 - alphaBits - bitsPerChannel * 2)) & maxValue;
                uint32_t actualBlue = (*pixelStart >> (32 - alphaBits - bitsPerChannel)) & maxValue;
                uint32_t actualAlpha = (*pixelStart >> (32 - alphaBits)) & maxAlphaValue;

                ASSERT_EQ(expectedRed, actualRed)
                        << "Red channel mismatch at (" << row << ", " << col << ")";
                ASSERT_EQ(expectedGreen, actualGreen)
                        << "Green channel mismatch at (" << row << ", " << col << ")";
                ASSERT_EQ(expectedBlue, actualBlue)
                        << "Blue channel mismatch at (" << row << ", " << col << ")";
            }
        }
    }
}

void ReadbackHelper::compareColorBuffers(void* expectedBuffer, void* actualBuffer,
                                         const uint32_t stride, int32_t bytesPerPixel,
                                         const uint32_t width, const uint32_t height,
                                         common::PixelFormat pixelFormat) {
    int32_t bitsPerChannel = GetBitsPerChannel(pixelFormat);
    int32_t alphaBits = GetAlphaBits(pixelFormat);
    ASSERT_GT(bytesPerPixel, 0);
    ASSERT_NE(-1, alphaBits);
    ASSERT_NE(-1, bitsPerChannel);
    uint32_t maxValue = (1 << bitsPerChannel) - 1;
    uint32_t maxAlphaValue = (1 << alphaBits) - 1;
    for (uint32_t row = 0; row < height; row++) {
        for (uint32_t col = 0; col < width; col++) {
            uint32_t offset = (row * stride + col) * static_cast<uint32_t>(bytesPerPixel);
            uint32_t* expectedStart = (uint32_t*)((uint8_t*)expectedBuffer + offset);
            uint32_t* actualStart = (uint32_t*)((uint8_t*)actualBuffer + offset);

            // Boo we're not word aligned so special case this.
            if (pixelFormat == common::PixelFormat::RGB_888) {
                uint8_t* expectedPixel = (uint8_t*)expectedStart;
                uint8_t* actualPixel = (uint8_t*)actualStart;
                ASSERT_EQ(actualPixel[0], expectedPixel[0])
                        << "Red channel mismatch at (" << row << ", " << col << ")";
                ASSERT_EQ(actualPixel[1], expectedPixel[1])
                        << "Green channel mismatch at (" << row << ", " << col << ")";
                ASSERT_EQ(actualPixel[2], expectedPixel[2])
                        << "Blue channel mismatch at (" << row << ", " << col << ")";
            } else {
                uint32_t expectedRed =
                        (*expectedStart >> (32 - alphaBits - bitsPerChannel * 3)) & maxValue;
                uint32_t expectedGreen =
                        (*expectedStart >> (32 - alphaBits - bitsPerChannel * 2)) & maxValue;
                uint32_t expectedBlue =
                        (*expectedStart >> (32 - alphaBits - bitsPerChannel)) & maxValue;
                uint32_t expectedAlpha = (*expectedStart >> (32 - alphaBits)) & maxAlphaValue;

                uint32_t actualRed =
                        (*actualStart >> (32 - alphaBits - bitsPerChannel * 3)) & maxValue;
                uint32_t actualGreen =
                        (*actualStart >> (32 - alphaBits - bitsPerChannel * 2)) & maxValue;
                uint32_t actualBlue =
                        (*actualStart >> (32 - alphaBits - bitsPerChannel)) & maxValue;
                uint32_t actualAlpha = (*actualStart >> (32 - alphaBits)) & maxAlphaValue;

                ASSERT_EQ(expectedRed, actualRed)
                        << "Red channel mismatch at (" << row << ", " << col << ")";
                ASSERT_EQ(expectedGreen, actualGreen)
                        << "Green channel mismatch at (" << row << ", " << col << ")";
                ASSERT_EQ(expectedBlue, actualBlue)
                        << "Blue channel mismatch at (" << row << ", " << col << ")";
            }
        }
    }
}

ReadbackBuffer::ReadbackBuffer(int64_t display, const std::shared_ptr<VtsComposerClient>& client,
                               int32_t width, int32_t height, common::PixelFormat pixelFormat,
                               common::Dataspace dataspace)
    : mComposerClient(client) {
    mDisplay = display;

    mPixelFormat = pixelFormat;
    mDataspace = dataspace;

    mWidth = static_cast<uint32_t>(width);
    mHeight = static_cast<uint32_t>(height);
    mLayerCount = 1;
    mUsage = static_cast<uint64_t>(static_cast<uint64_t>(common::BufferUsage::CPU_READ_OFTEN) |
                                   static_cast<uint64_t>(common::BufferUsage::GPU_TEXTURE));

    mAccessRegion.top = 0;
    mAccessRegion.left = 0;
    mAccessRegion.right = static_cast<int32_t>(width);
    mAccessRegion.bottom = static_cast<int32_t>(height);
}

::android::sp<::android::GraphicBuffer> ReadbackBuffer::allocateBuffer() {
    return ::android::sp<::android::GraphicBuffer>::make(
            mWidth, mHeight, static_cast<::android::PixelFormat>(mPixelFormat), mLayerCount, mUsage,
            "ReadbackBuffer");
}

void ReadbackBuffer::setReadbackBuffer() {
    mGraphicBuffer = allocateBuffer();
    ASSERT_NE(nullptr, mGraphicBuffer);
    ASSERT_EQ(::android::OK, mGraphicBuffer->initCheck());
    const auto& bufferHandle = mGraphicBuffer->handle;
    ::ndk::ScopedFileDescriptor fence = ::ndk::ScopedFileDescriptor(-1);
    EXPECT_TRUE(mComposerClient->setReadbackBuffer(mDisplay, bufferHandle, fence).isOk());
}

void ReadbackBuffer::checkReadbackBuffer(const std::vector<Color>& expectedColors) {
    ASSERT_NE(nullptr, mGraphicBuffer);
    // lock buffer for reading
    const auto& [fenceStatus, bufferFence] = mComposerClient->getReadbackBufferFence(mDisplay);
    EXPECT_TRUE(fenceStatus.isOk());

    int bytesPerPixel = -1;
    int bytesPerStride = -1;
    void* bufData = nullptr;

    auto status = mGraphicBuffer->lockAsync(mUsage, mAccessRegion, &bufData, dup(bufferFence.get()),
                                            &bytesPerPixel, &bytesPerStride);
    EXPECT_EQ(::android::OK, status);
    ASSERT_TRUE(mPixelFormat == PixelFormat::RGB_888 || mPixelFormat == PixelFormat::RGBA_8888 ||
                mPixelFormat == PixelFormat::RGBA_1010102);
    const uint32_t stride = (bytesPerPixel > 0 && bytesPerStride > 0)
                                    ? static_cast<uint32_t>(bytesPerStride / bytesPerPixel)
                                    : mGraphicBuffer->getStride();
    ReadbackHelper::compareColorBuffers(expectedColors, bufData, stride, bytesPerPixel, mWidth,
                                        mHeight, mPixelFormat);
    status = mGraphicBuffer->unlock();
    EXPECT_EQ(::android::OK, status);
}

::android::sp<::android::GraphicBuffer> ReadbackBuffer::getBuffer() {
    const auto& [fenceStatus, bufferFence] = mComposerClient->getReadbackBufferFence(mDisplay);
    EXPECT_TRUE(fenceStatus.isOk());
    if (bufferFence.get() != -1) {
        sync_wait(bufferFence.get(), -1);
    }
    return mGraphicBuffer;
}

void TestColorLayer::write(ComposerClientWriter& writer) {
    TestLayer::write(writer);
    writer.setLayerCompositionType(mDisplay, mLayer, Composition::SOLID_COLOR);
    writer.setLayerColor(mDisplay, mLayer, mColor);
}

LayerSettings TestColorLayer::toRenderEngineLayerSettings() {
    LayerSettings layerSettings = TestLayer::toRenderEngineLayerSettings();

    layerSettings.source.solidColor = ::android::half3(mColor.r, mColor.g, mColor.b);
    layerSettings.alpha = mAlpha * mColor.a;
    return layerSettings;
}

TestBufferLayer::TestBufferLayer(const std::shared_ptr<VtsComposerClient>& client,
                                 TestRenderEngine& renderEngine, int64_t display, uint32_t width,
                                 uint32_t height, common::PixelFormat format,
                                 ComposerClientWriter& writer, Composition composition)
    : TestLayer{client, display, writer}, mRenderEngine(renderEngine) {
    mComposition = composition;
    mWidth = width;
    mHeight = height;
    mLayerCount = 1;
    mPixelFormat = format;
    mUsage = (static_cast<uint64_t>(common::BufferUsage::CPU_READ_OFTEN) |
              static_cast<uint64_t>(common::BufferUsage::CPU_WRITE_OFTEN) |
              static_cast<uint64_t>(common::BufferUsage::COMPOSER_OVERLAY) |
              static_cast<uint64_t>(common::BufferUsage::GPU_TEXTURE));

    mAccessRegion.top = 0;
    mAccessRegion.left = 0;
    mAccessRegion.right = static_cast<int32_t>(width);
    mAccessRegion.bottom = static_cast<int32_t>(height);

    setSourceCrop({0, 0, (float)width, (float)height});
}

void TestBufferLayer::write(ComposerClientWriter& writer) {
    TestLayer::write(writer);
    writer.setLayerCompositionType(mDisplay, mLayer, mComposition);
    writer.setLayerVisibleRegion(mDisplay, mLayer, std::vector<Rect>(1, mDisplayFrame));
    if (mGraphicBuffer) {
        writer.setLayerBuffer(mDisplay, mLayer, /*slot*/ 0, mGraphicBuffer->handle, mFillFence);
    }
}

LayerSettings TestBufferLayer::toRenderEngineLayerSettings() {
    LayerSettings layerSettings = TestLayer::toRenderEngineLayerSettings();
    layerSettings.source.buffer.buffer =
            std::make_shared<::android::renderengine::impl::ExternalTexture>(
                    mGraphicBuffer, mRenderEngine.getInternalRenderEngine(),
                    ::android::renderengine::impl::ExternalTexture::Usage::READABLE);

    layerSettings.source.buffer.usePremultipliedAlpha = mBlendMode == BlendMode::PREMULTIPLIED;

    const float scaleX = (mSourceCrop.right - mSourceCrop.left) / (static_cast<float>(mWidth));
    const float scaleY = (mSourceCrop.bottom - mSourceCrop.top) / (static_cast<float>(mHeight));
    const float translateX = mSourceCrop.left / (static_cast<float>(mWidth));
    const float translateY = mSourceCrop.top / (static_cast<float>(mHeight));

    layerSettings.source.buffer.textureTransform =
            ::android::mat4::translate(::android::vec4(translateX, translateY, 0.0f, 1.0f)) *
            ::android::mat4::scale(::android::vec4(scaleX, scaleY, 1.0f, 1.0f));

    return layerSettings;
}

void TestBufferLayer::fillBuffer(std::vector<Color>& expectedColors) {
    void* bufData;
    int32_t bytesPerPixel = -1;
    int32_t bytesPerStride = -1;
    auto status = mGraphicBuffer->lock(mUsage, &bufData, &bytesPerPixel, &bytesPerStride);
    const uint32_t stride = (bytesPerPixel > 0 && bytesPerStride > 0)
                                    ? static_cast<uint32_t>(bytesPerStride / bytesPerPixel)
                                    : mGraphicBuffer->getStride();
    EXPECT_EQ(::android::OK, status);
    ASSERT_NO_FATAL_FAILURE(ReadbackHelper::fillBuffer(mWidth, mHeight, stride, bytesPerPixel,
                                                       bufData, mPixelFormat, expectedColors));

    const auto unlockStatus = mGraphicBuffer->unlockAsync(&mFillFence);
    ASSERT_EQ(::android::OK, unlockStatus);
}

void TestBufferLayer::setBuffer(std::vector<Color> colors) {
    mGraphicBuffer = allocateBuffer();
    ASSERT_NE(nullptr, mGraphicBuffer);
    ASSERT_EQ(::android::OK, mGraphicBuffer->initCheck());
    ASSERT_NO_FATAL_FAILURE(fillBuffer(colors));
}

::android::sp<::android::GraphicBuffer> TestBufferLayer::allocateBuffer() {
    return ::android::sp<::android::GraphicBuffer>::make(
            mWidth, mHeight, static_cast<::android::PixelFormat>(mPixelFormat), mLayerCount, mUsage,
            "TestBufferLayer");
}

void TestBufferLayer::setToClientComposition(ComposerClientWriter& writer) {
    writer.setLayerCompositionType(mDisplay, mLayer, Composition::CLIENT);
}

}  // namespace aidl::android::hardware::graphics::composer3::vts

// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/shell/platform/darwin/ios/ios_external_image_gl.h"
#include "flutter/shell/platform/darwin/ios/platform_view_ios.h"
#include "flutter/common/threads.h"
#include "third_party/skia/include/gpu/GrTexture.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "flutter/lib/ui/painting/resource_context.h"
#include "third_party/skia/include/gpu/GrTypes.h"
#include "third_party/skia/include/gpu/GrBackendSurface.h"

namespace shell {

IOSExternalImageGL::~IOSExternalImageGL() {}

IOSExternalImageGL::IOSExternalImageGL() {
  ftl::AutoResetWaitableEvent latch;
  blink::Threads::IO()->PostTask([this, &latch]() {
    CVReturn err = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, [EAGLContext currentContext], NULL, &cache_);
    if (err != noErr) {
      FTL_LOG(INFO) << "Failed to create GLES texture cache";
    }
    latch.Signal();
  });
  latch.Wait();
}

sk_sp<SkImage> IOSExternalImageGL::MakeSkImage(int width, int height, GrContext *grContext) {
  ASSERT_IS_GPU_THREAD;
  CVPixelBufferRef buffer;
  ftl::AutoResetWaitableEvent latch;
  blink::Threads::IO()->PostTask([this, &latch, &buffer]() {
    buffer = PlatformViewIOS::GetPixelBuffer(image_id());
    latch.Signal();
  });
  latch.Wait();
  if (buffer == nullptr) {
    return nullptr;
  }
  CVOpenGLESTextureRef texture;
  CVReturn err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault, cache_, buffer, nullptr, GL_TEXTURE_2D, GL_RGBA8_OES, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0, &texture);
  if (err != noErr) {
    FTL_LOG(INFO) << "Could not create texture from pixel buffer: " << err;
    return nullptr;
  }
  GrGLTextureInfo textureInfo = {CVOpenGLESTextureGetTarget(texture), CVOpenGLESTextureGetName(texture)};
  GrBackendTexture backendTexture(width, height, kRGBA_8888_GrPixelConfig, textureInfo);
  sk_sp<SkImage> sk_image = SkImage::MakeFromTexture(
     grContext,
     backendTexture,
     kBottomLeft_GrSurfaceOrigin,
     SkAlphaType::kPremul_SkAlphaType,
     nullptr
  );
  return sk_image;
}

} // namespace shell

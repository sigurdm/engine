// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/shell/platform/darwin/ios/ios_external_image_gl.h"
#include "flutter/common/threads.h"
#include "third_party/skia/include/gpu/GrTexture.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "flutter/lib/ui/painting/resource_context.h"
#include "third_party/skia/include/gpu/GrTypes.h"
#include "third_party/skia/include/gpu/GrBackendSurface.h"

namespace shell {

IOSExternalImageGL::~IOSExternalImageGL(){}
IOSExternalImageGL::IOSExternalImageGL(PlatformViewIOS* view): view_(view) {
  FTL_LOG(INFO) << "Creating ext image for " << view_;
  ftl::AutoResetWaitableEvent latch;
  blink::Threads::IO()->PostTask([this, &latch]() {
    FTL_LOG(INFO) << "EAGLContext " << [EAGLContext currentContext];
    CVReturn err = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, [EAGLContext currentContext], NULL, &cache_);
    if (err != noErr) {
      FTL_LOG(INFO) << "Failed to create GLES texture cache: " << err;
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
  if (buffer != nullptr) {
    if (texture_ != nullptr) CFRelease(texture_);
    CVReturn err = CVOpenGLESTextureCacheCreateTextureFromImage(
      kCFAllocatorDefault, cache_, buffer, nullptr,
      GL_TEXTURE_2D,
      GL_RGBA,
      (int) CVPixelBufferGetWidth(buffer),
      (int) CVPixelBufferGetHeight(buffer),
      GL_BGRA,
      GL_UNSIGNED_BYTE,
      0,
      &texture_);
      CFRelease(buffer);
    if (err != noErr) {
      FTL_LOG(INFO) << "Could not create texture from pixel buffer: " << err;
      return nullptr;
    }
  }
  if (texture_ == nullptr) {
    return nullptr;
  }
  GrGLTextureInfo textureInfo = {CVOpenGLESTextureGetTarget(texture_), CVOpenGLESTextureGetName(texture_)};
  GrBackendTexture backendTexture(width, height, kRGBA_8888_GrPixelConfig, textureInfo);
  sk_sp<SkImage> sk_image = SkImage::MakeFromTexture(
     grContext,
     backendTexture,
     kBottomLeft_GrSurfaceOrigin,
     SkAlphaType::kPremul_SkAlphaType,
     nullptr
  );
  CVOpenGLESTextureCacheFlush(cache_, 0);
  return sk_image;
}

} // namespace shell

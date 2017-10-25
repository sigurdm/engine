// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/shell/platform/darwin/ios/ios_external_texture_gl.h"

#include "flutter/common/threads.h"
#include "flutter/lib/ui/painting/resource_context.h"
#include "flutter/shell/platform/darwin/ios/framework/Source/vsync_waiter_ios.h"
#include "third_party/skia/include/core/SkSurface.h"
// #include "third_party/skia/include/gpu/GrBackendSurface.h"
#include "third_party/skia/include/gpu/GrTexture.h"
// #include "third_party/skia/include/gpu/GrTypes.h"

#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/gpu/GrBackendSurface.h"
#include "third_party/skia/include/gpu/GrContext.h"
#include "third_party/skia/include/gpu/GrTexture.h"
#include "third_party/skia/include/gpu/GrTypes.h"


namespace shell {

IOSExternalTextureGL::IOSExternalTextureGL(NSObject<FlutterTexture>* externalTexture)
    : external_texture_(externalTexture) {
  FXL_DCHECK(external_texture_);
  [external_texture_ retain];
}

IOSExternalTextureGL::~IOSExternalTextureGL() {
  [external_texture_ release];
}

void IOSExternalTextureGL::DrawOnCanvas(int x,
                                       int y,
                                       int width,
                                       int height,
                                       SkCanvas& canvas) {
  ASSERT_IS_GPU_THREAD;
  if (!cache_ref_) {
    CVOpenGLESTextureCacheRef cache;
    CVReturn err = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL,
                                                [EAGLContext currentContext], NULL, &cache);
    if (err == noErr) {
      cache_ref_.Reset(cache);
    } else {
      FXL_LOG(WARNING) << "Failed to create GLES texture cache: " << err;
      return;
    }
  }
  fml::CFRef<CVPixelBufferRef> bufferRef;
  bufferRef.Reset([external_texture_ copyPixelBuffer]);
  if (bufferRef != nullptr) {
    CVOpenGLESTextureRef texture;
    CVReturn err = CVOpenGLESTextureCacheCreateTextureFromImage(
        kCFAllocatorDefault, cache_ref_, bufferRef, nullptr, GL_TEXTURE_2D, GL_RGBA,
        static_cast<int>(CVPixelBufferGetWidth(bufferRef)),
        static_cast<int>(CVPixelBufferGetHeight(bufferRef)), GL_BGRA, GL_UNSIGNED_BYTE, 0,
        &texture);
    texture_ref_.Reset(texture);
    if (err != noErr) {
      FXL_LOG(WARNING) << "Could not create texture from pixel buffer: " << err;
      return;
    }
  }
  if (!texture_ref_) {
    return;
  }
  GrGLTextureInfo textureInfo = {CVOpenGLESTextureGetTarget(texture_ref_),
                                 CVOpenGLESTextureGetName(texture_ref_)};
  GrBackendTexture backendTexture(width, height, kRGBA_8888_GrPixelConfig, textureInfo);
  auto image = SkImage::MakeFromTexture(
      canvas.getGrContext(),
      backendTexture,
      kTopLeft_GrSurfaceOrigin,
      SkAlphaType::kPremul_SkAlphaType,
      nullptr);

  canvas.drawImage(image, x, y);
}

void IOSExternalTextureGL::OnGrContextCreated() {
  ASSERT_IS_GPU_THREAD
}

void IOSExternalTextureGL::OnGrContextDestroyed() {
  ASSERT_IS_GPU_THREAD
  texture_ref_.Reset(nullptr);
  cache_ref_.Reset(nullptr);
}

}  // namespace shell

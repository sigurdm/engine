// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <jni.h>
#include "flutter/fml/platform/android/jni_weak_ref.h"
#include "flutter/fml/platform/android/scoped_java_ref.h"
#include "flutter/fml/platform/android/jni_util.h"
#include "flutter/flow/platform_surface.h"

#ifndef FLUTTER_SHELL_PLATFORM_ANDROID_PLATFORM_SURFACE_GL_H_
#define FLUTTER_SHELL_PLATFORM_ANDROID_PLATFORM_SURFACE_GL_H_

namespace shell {

class AndroidPlatformSurfaceGL : public flow::PlatformSurface {
 public:
  AndroidPlatformSurfaceGL();

  ~AndroidPlatformSurfaceGL() override;

  uint32_t texture_id() { return texture_id_; }

  // Called from GPU thread.
  virtual sk_sp<SkImage> MakeSkImage(int width, int height, GrContext *grContext) override;

  // Called on platform thread.
  void MarkNewFrameAvailable();

 private:
  uint32_t texture_id_;
  bool new_frame_ready_ = false;
  bool first_frame_seen_ = false;
  FTL_DISALLOW_COPY_AND_ASSIGN(AndroidPlatformSurfaceGL);
};

}

#endif // FLUTTER_SHELL_PLATFORM_ANDROID_PLATFORM_SURFACE_GL_H_

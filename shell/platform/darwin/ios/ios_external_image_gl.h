// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import <CoreVideo/CoreVideo.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#include "flutter/flow/external_image.h"

#ifndef FLUTTER_SHELL_PLATFORM_IOS_EXTERNAL_IMAGE_GL_H_
#define FLUTTER_SHELL_PLATFORM_IOS_EXTERNAL_IMAGE_GL_H_

namespace shell {

class IOSExternalImageGL : public flow::ExternalImage {
 public:
  IOSExternalImageGL();

  ~IOSExternalImageGL() override;

  // Called from GPU thread.
  virtual sk_sp<SkImage> MakeSkImage(int width, int height, GrContext *grContext) override;

 private:
  CVOpenGLESTextureCacheRef cache_;
  FTL_DISALLOW_COPY_AND_ASSIGN(IOSExternalImageGL);
};

}

#endif // FLUTTER_SHELL_PLATFORM_IOS_EXTERNAL_IMAGE_GL_H_

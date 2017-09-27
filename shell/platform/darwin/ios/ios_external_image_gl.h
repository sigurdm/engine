// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import <CoreVideo/CoreVideo.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#include "flutter/flow/external_image.h"
#include "flutter/shell/platform/darwin/ios/platform_view_ios.h"
#include "flutter/shell/platform/darwin/ios/framework/Headers/FlutterExternalImage.h"

#ifndef FLUTTER_SHELL_PLATFORM_IOS_EXTERNAL_IMAGE_GL_H_
#define FLUTTER_SHELL_PLATFORM_IOS_EXTERNAL_IMAGE_GL_H_

namespace shell {

class IOSExternalImageGL : public flow::ExternalImage {
 public:
  IOSExternalImageGL(NSObject<FlutterExternalImage>* image);

  ~IOSExternalImageGL() override;

  // Called from GPU thread.
  virtual sk_sp<SkImage> MakeSkImage(int width, int height, GrContext *grContext) override;

 private:
  NSObject<FlutterExternalImage>* image_;
  CVOpenGLESTextureCacheRef cache_ = nullptr;
  CVOpenGLESTextureRef texture_ = nullptr;
  FTL_DISALLOW_COPY_AND_ASSIGN(IOSExternalImageGL);
};

}

#endif // FLUTTER_SHELL_PLATFORM_IOS_EXTERNAL_IMAGE_GL_H_

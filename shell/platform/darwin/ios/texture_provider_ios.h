// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_SHELL_PLATFORM_DARWIN_IOS_TEXTURE_PROVIDER_IOS_H_
#define FLUTTER_SHELL_PLATFORM_DARWIN_IOS_TEXTURE_PROVIDER_IOS_H_

#import <AVFoundation/AVFoundation.h>
#import <CoreMedia/CoreMedia.h>
#import <CoreVideo/CoreVideo.h>
#import <OpenGLES/EAGL.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <mach/mach.h>
#include <mach/task.h>
#include <cstdlib>
#include "flutter/flow/texture_provider.h"
#include "lib/ftl/macros.h"
#include "lib/ftl/logging.h"

namespace shell {

class TextureProviderIOS : public flow::TextureProvider {
 public:
  TextureProviderIOS();

  ~TextureProviderIOS();

  GrBackendTexture BindTexture(GrContext* gr_context, uint32_t texture_id) override;

 private:
  int bufferCount = -1;
  int sampleCount = -1;
  AVAsset* asset_;
  AVAssetReader* reader_;
  AVAssetReaderTrackOutput* track_output_;
  CVOpenGLESTextureCacheRef texture_cache_;
  FTL_DISALLOW_COPY_AND_ASSIGN(TextureProviderIOS);
};

}  // namespace shell

#endif  // FLUTTER_SHELL_PLATFORM_DARWIN_IOS_TEXTURE_PROVIDER_IOS_H_

// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_SHELL_PLATFORM_ANDROID_EXTERNAL_TEXTURE_GL_H_
#define FLUTTER_SHELL_PLATFORM_ANDROID_EXTERNAL_TEXTURE_GL_H_

#include <GLES/gl.h>
#include "flutter/flow/texture.h"
#include "flutter/fml/platform/android/jni_weak_ref.h"

namespace shell {

class AndroidExternalTextureGL : public flow::Texture {
 public:
  AndroidExternalTextureGL(
      const fml::jni::JavaObjectWeakGlobalRef& surfaceTexture);

  ~AndroidExternalTextureGL() override;

  virtual void DrawOnCanvas(int x,
                                                int y,
                                                int width,
                                                int height,
                                                SkCanvas &canvas) override;

  virtual void OnGrContextCreated() override;

  virtual void OnGrContextDestroyed() override;

  // Called on GPU thread.
  void MarkNewFrameAvailable();

 private:

  void GetTransform(float* matrix);

  void Attach(jint textureName);

  void Update();

  void Detach();

  enum class AttachmentState { uninitialized, attached, detached };

  fml::jni::JavaObjectWeakGlobalRef surface_texture_;

  AttachmentState state_ = AttachmentState::uninitialized;

  bool new_frame_ready_ = false;

  GLuint texture_name_ = 0;

  FXL_DISALLOW_COPY_AND_ASSIGN(AndroidExternalTextureGL);
};

}  // namespace shell

#endif  // FLUTTER_SHELL_PLATFORM_ANDROID_EXTERNAL_TEXTURE_GL_H_

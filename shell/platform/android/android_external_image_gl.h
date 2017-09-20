#include <jni.h>
#include "flutter/fml/platform/android/jni_weak_ref.h"
#include "flutter/fml/platform/android/scoped_java_ref.h"
#include "flutter/fml/platform/android/jni_util.h"
#include "flutter/flow/external_image.h"

#ifndef FLUTTER_SHELL_PLATFORM_ANDROID_EXTERNAL_IMAGE_GL_H_
#define FLUTTER_SHELL_PLATFORM_ANDROID_EXTERNAL_IMAGE_GL_H_

namespace shell {

class AndroidExternalImageGL : public flow::ExternalImage {
 public:
  AndroidExternalImageGL();

  ~AndroidExternalImageGL() override;

  uint32_t texture_id() { return texture_id_; }

  // Called from GPU thread.
  virtual sk_sp<SkImage> MakeSkImage(int width, int height, GrContext *grContext) override;

  // Called on platform thread.
  void mark_new_frame_available();

 private:
  uint32_t texture_id_;
  bool new_frame_ready_ = false;
  bool first_frame_seen_ = false;
  FTL_DISALLOW_COPY_AND_ASSIGN(AndroidExternalImageGL);
};

}

#endif // FLUTTER_SHELL_PLATFORM_ANDROID_EXTERNAL_IMAGE_GL_H_

#include <jni.h>
#include "flutter/fml/platform/android/jni_weak_ref.h"
#include "flutter/fml/platform/android/scoped_java_ref.h"
#include "flutter/fml/platform/android/jni_util.h"
#include "flutter/flow/external_image.h"

#ifndef FLUTTER_SHELL_PLATFORM_ANDROID_GL_TEXTURE_EXTERNAL_IMAGE_H_
#define FLUTTER_SHELL_PLATFORM_ANDROID_GL_TEXTURE_EXTERNAL_IMAGE_H_

namespace shell {

// TODO(sigurdm): Rename this to ExternalImageGLTexture.
class GlTextureExternalImage : public flow::ExternalImage {
  public:
   GlTextureExternalImage();

   ~GlTextureExternalImage() override;

  virtual void UpdateImage() override;

  // Called from GPU thread.
  virtual sk_sp<SkImage> MakeSkImage(int width, int height, GrContext *grContext) override;

   uint32_t texture_id() { return texture_id_; }

  private:
   uint32_t texture_id_;
   FTL_DISALLOW_COPY_AND_ASSIGN(GlTextureExternalImage);
};

}

#endif // FLUTTER_SHELL_PLATFORM_ANDROID_GL_TEXTURE_EXTERNAL_IMAGE_H_
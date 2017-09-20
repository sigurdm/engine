#include <jni.h>
#include "flutter/shell/platform/android/android_external_image_gl.h"
#include "flutter/shell/platform/android/platform_view_android_jni.h"
#include "flutter/common/threads.h"
#include "third_party/skia/include/gpu/GrTexture.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "flutter/lib/ui/painting/resource_context.h"
#include "third_party/skia/include/gpu/GrTypes.h"
#include "third_party/skia/include/gpu/GrBackendSurface.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES/gl.h>
#include <GLES/glext.h>

namespace shell {

AndroidExternalImageGL::~AndroidExternalImageGL() {}

AndroidExternalImageGL::AndroidExternalImageGL() {
  ftl::AutoResetWaitableEvent latch;
  blink::Threads::IO()->PostTask([this, &latch]() {
    GrGLuint texID;
    glGenTextures(1, &texID);

    glBindTexture(GL_TEXTURE_EXTERNAL_OES, texID);
    texture_id_ = texID;
    latch.Signal();
  });
  latch.Wait();
}

void AndroidExternalImageGL::mark_new_frame_available() {
  ASSERT_IS_PLATFORM_THREAD;
  blink::Threads::IO()->PostTask([this]() {
    new_frame_ready_ = true;
  });
}

sk_sp<SkImage> AndroidExternalImageGL::MakeSkImage(int width, int height, GrContext *grContext) {
  ASSERT_IS_GPU_THREAD;
  ftl::AutoResetWaitableEvent latch;
  blink::Threads::IO()->PostTask([this, &latch]() {
    if (new_frame_ready_) {
      JNIEnv* env = fml::jni::AttachCurrentThread();
      FlutterViewUpdateTexImage(env, image_id());
      first_frame_seen_ = true;
      new_frame_ready_ = false;
    }
    latch.Signal();
  });
  latch.Wait();
  if (!first_frame_seen_) {
    return nullptr;
  }
  SkImageInfo info = SkImageInfo::MakeN32(128, 64, SkAlphaType::kPremul_SkAlphaType);
  GrGLTextureInfo textureInfo = {0x8D65, texture_id_};

  GrBackendTexture backendTexture(width, height, kRGBA_8888_GrPixelConfig, textureInfo);
  sk_sp<SkImage> sk_image = SkImage::MakeFromTexture(
     grContext,
     backendTexture,
     kBottomLeft_GrSurfaceOrigin,
     SkAlphaType::kPremul_SkAlphaType,
     info.refColorSpace()
  );
  return sk_image;
}

} // namespace shell

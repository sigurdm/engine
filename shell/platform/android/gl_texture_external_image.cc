#include <jni.h>
#include "flutter/shell/platform/android/gl_texture_external_image.h"
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

GlTextureExternalImage::~GlTextureExternalImage() {}

GlTextureExternalImage::GlTextureExternalImage() {
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

sk_sp<SkImage> GlTextureExternalImage::MakeSkImage(int width, int height, GrContext *grContext) {
  SkImageInfo info = SkImageInfo::MakeN32(128, 64, SkAlphaType::kPremul_SkAlphaType);
  GrGLTextureInfo textureInfo = {0x8D65, (uint)texture_id_};

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

void GlTextureExternalImage::UpdateImage() {
  ftl::AutoResetWaitableEvent latch;
  blink::Threads::IO()->PostTask([this, &latch]() {
    ASSERT_IS_IO_THREAD;
    if (new_frame_ready()) {
      JNIEnv* env = fml::jni::AttachCurrentThread();
      FlutterViewUpdateTexImage(env, image_id());
      set_new_frame_ready(false);
      set_first_frame_seen();
    }
    latch.Signal();
  });
  latch.Wait();
}

}

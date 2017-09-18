// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/flow/layers/texture_layer.h"
#include "third_party/skia/include/gpu/GrTypes.h"
#include "third_party/skia/include/gpu/GrBackendSurface.h"
#include "third_party/skia/include/gpu/GrContext.h"
#include "third_party/skia/include/gpu/GrTexture.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/core/SkMatrix.h"
#include "flutter/common/threads.h"
#include "lib/ftl/synchronization/waitable_event.h"

namespace flow {

TextureLayer::TextureLayer() = default;

TextureLayer::~TextureLayer() = default;

void TextureLayer::Preroll(PrerollContext* context, const SkMatrix& matrix) {
  set_paint_bounds(SkRect::MakeXYWH(offset_.x(), offset_.y(), size_.width(), size_.height()));
  set_needs_system_composite(true);
}

void printPixels(sk_sp<SkImage> image) {
  uint pixel = 99;
  SkImageInfo dstInfo = SkImageInfo::MakeN32(1, 1, SkAlphaType::kPremul_SkAlphaType);
  image->readPixels(dstInfo, &pixel, 4, 0, 0);
  FTL_DLOG(INFO) << "First pixel: " << pixel;
}

jmethodID g_update_tex_image_method = nullptr;
void FlutterViewUpdateTexImage(jlong texName) {
  JNIEnv* env = fml::jni::AttachCurrentThread();
  fml::jni::ScopedJavaLocalRef<jobject> view = flow::flutter_view.get(env);
  env->CallVoidMethod(view.obj(), g_update_tex_image_method, texName);
  FTL_CHECK(env->ExceptionCheck() == JNI_FALSE);
}

jlong texName = 0;
bool surfaceUpdated = false;
fml::jni::JavaObjectWeakGlobalRef flutter_view;

void TextureLayer::Paint(PaintContext& context) {

  if (texName == 0) {
    FTL_DLOG(INFO) << "No texName!";
    return;
  }

  SkScalar x = paint_bounds().x();
  SkScalar y = paint_bounds().y();

  static bool firstFrameSeen = false;

  ftl::AutoResetWaitableEvent latch;
  blink::Threads::IO()->PostTask([&latch]() {
    ASSERT_IS_IO_THREAD;
    if (surfaceUpdated) {
      surfaceUpdated = false;
      FlutterViewUpdateTexImage(texName);
      firstFrameSeen = true;
    }

    latch.Signal();
  });
  latch.Wait();

  if (!firstFrameSeen) {
    FTL_DLOG(INFO) << "No frame yet";
    return;
  }
  SkImageInfo info = SkImageInfo::MakeN32(128, 64, SkAlphaType::kPremul_SkAlphaType);
  GrGLTextureInfo textureInfo = {0x8D65, (uint)texName};

  GrBackendTexture backendTexture(paint_bounds().width(), paint_bounds().height(), kRGBA_8888_GrPixelConfig, textureInfo);
  sk_sp<SkImage> image2 = SkImage::MakeFromTexture(
     context.canvas.getGrContext(),
     backendTexture,
     kBottomLeft_GrSurfaceOrigin,
     SkAlphaType::kPremul_SkAlphaType,
     info.refColorSpace()
  );
  FTL_DLOG(INFO) << "image from texture: " << image2;
  context.canvas.save();
  context.canvas.scale(1.0, -1.0);
  context.canvas.translate(0.0, -paint_bounds().height());
  context.canvas.drawImage(image2, x, -y);
  context.canvas.restore();
}

}  // namespace flow

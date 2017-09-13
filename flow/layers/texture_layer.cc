// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/flow/layers/texture_layer.h"
#include "third_party/skia/include/gpu/GrTypes.h"
#include "third_party/skia/include/gpu/GrBackendSurface.h"
#include "third_party/skia/include/gpu/GrContext.h"
#include "third_party/skia/include/gpu/GrTexture.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "flutter/lib/ui/painting/resource_context.h"
#include "flutter/common/threads.h"
#include "lib/ftl/synchronization/waitable_event.h"

namespace flow {

TextureLayer::TextureLayer() = default;

TextureLayer::~TextureLayer() = default;

void TextureLayer::Preroll(PrerollContext* context, const SkMatrix& matrix) {
  set_paint_bounds(SkRect::MakeXYWH(offset_.x(), offset_.y(), size_.width(), size_.height()));
}

void printPixels(sk_sp<SkImage> image) {
  uint pixel = 99;
  SkImageInfo dstInfo = SkImageInfo::MakeN32(1, 1, SkAlphaType::kPremul_SkAlphaType);
  image->readPixels(dstInfo, &pixel, 4, 0, 0);
  FTL_DLOG(INFO) << "First pixel: " << pixel;
}

void TextureLayer::Paint(PaintContext& context) {
  FTL_DLOG(INFO) << "Painting texture layer!";

  SkScalar x = paint_bounds().x();
  SkScalar y = paint_bounds().y();

  SkImageInfo info = SkImageInfo::MakeN32(128, 64, SkAlphaType::kPremul_SkAlphaType);
  ftl::AutoResetWaitableEvent latch;
  sk_sp<SkSurface> surface;
  sk_sp<SkImage> image;
  GrGLTextureInfo* textureInfo;
  blink::Threads::IO()->PostTask([&info, &latch, &surface, &image, &textureInfo]() {
    ASSERT_IS_IO_THREAD;
    FTL_DLOG(INFO) << "IO thread task running...";
    std::string string = "blah_texture";

    surface = SkSurface::MakeRenderTarget(blink::ResourceContext::Get(), SkBudgeted::kYes, info);
    SkCanvas* canvas = surface->getCanvas();
    canvas->clear(0x88888888);
    SkPaint paint;
    paint.setTextSize(14);
    paint.setLinearText(false);
    paint.setColor(SK_ColorRED);
    canvas->drawText(string.c_str(), string.size(), 0, 0 + 27, paint);
    canvas->flush();
    image = surface->makeImageSnapshot();
    printPixels(image);
     // TODO(mravn): Keeps image and its texture alive until (and after!) canvas drawing has flushed.
    image->ref();
    GrBackendObject backendObject = image->getTextureHandle(true);
    textureInfo = reinterpret_cast<GrGLTextureInfo*>(backendObject);

    FTL_DLOG(INFO) << "Signalling IO thread task completed with texture ID " << textureInfo->fID;
    latch.Signal();
  });
  FTL_DLOG(INFO) << "Waiting for IO thread task...";
  latch.Wait();
  FTL_DLOG(INFO) << "IO thread task completed with texture ID " << textureInfo->fID;

  if (textureInfo) {
    GrBackendTexture backendTexture(128, 64, kRGBA_8888_GrPixelConfig, *textureInfo);
    sk_sp<SkImage> image2 = SkImage::MakeFromTexture(
       context.canvas.getGrContext(),
       backendTexture,
       kBottomLeft_GrSurfaceOrigin,
       SkAlphaType::kPremul_SkAlphaType,
       info.refColorSpace()
    );
    FTL_DLOG(INFO) << "image from texture: " << image2;
    context.canvas.drawImage(image2, x, y);
  }
}

}  // namespace flow

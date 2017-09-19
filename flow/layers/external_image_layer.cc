// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/flow/layers/external_image_layer.h"
#include "flutter/flow/external_image.h"
#include "third_party/skia/include/gpu/GrTypes.h"
#include "third_party/skia/include/gpu/GrBackendSurface.h"
#include "third_party/skia/include/gpu/GrContext.h"
#include "third_party/skia/include/gpu/GrTexture.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/core/SkMatrix.h"
#include "flutter/common/threads.h"
#include "lib/ftl/synchronization/waitable_event.h"

namespace flow {

ExternalImageLayer::ExternalImageLayer() = default;

ExternalImageLayer::~ExternalImageLayer() = default;

void ExternalImageLayer::Preroll(PrerollContext* context, const SkMatrix& matrix) {
  set_paint_bounds(SkRect::MakeXYWH(offset_.x(), offset_.y(), size_.width(), size_.height()));
  set_needs_system_composite(true);
}

void printPixels(sk_sp<SkImage> image) {
  uint pixel = 99;
  SkImageInfo dstInfo = SkImageInfo::MakeN32(1, 1, SkAlphaType::kPremul_SkAlphaType);
  image->readPixels(dstInfo, &pixel, 4, 0, 0);
  FTL_DLOG(INFO) << "First pixel: " << pixel;
}

void ExternalImageLayer::Paint(PaintContext& context) {

  ExternalImage* image = ExternalImage::getExternalImage(image_id_);

  if (image == nullptr) {
    FTL_DLOG(INFO) << "No external image!";
    return;
  }

  image->UpdateImage();

  SkScalar x = paint_bounds().x();
  SkScalar y = paint_bounds().y();

  if (!image->first_frame_seen()) {
    FTL_DLOG(INFO) << "No frame yet";
    return;
  }

  sk_sp<SkImage> sk_image = image->MakeSkImage(paint_bounds().width(), paint_bounds().height(), context.canvas.getGrContext());
  context.canvas.save();
  context.canvas.scale(1.0, -1.0);
  context.canvas.translate(0.0, -paint_bounds().height());
  context.canvas.drawImage(sk_image, x, -y);
  context.canvas.restore();
}

}  // namespace flow

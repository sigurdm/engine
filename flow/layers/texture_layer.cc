// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/flow/layers/texture_layer.h"
#include "third_party/skia/include/gpu/GrTypes.h"

namespace flow {

TextureLayer::TextureLayer() = default;

TextureLayer::~TextureLayer() = default;

void TextureLayer::Paint(PaintContext& context) {
  GrBackendTexture backend = context.compositor.BindTexture(context.canvas.getGrContext(), texture_id_);
  if (backend.width() == 0 || backend.height() == 0) {
    FTL_DLOG(INFO) << "texture provider provided an empty texture";
    return;
  }
  sk_sp<SkImage> image = SkImage::MakeFromTexture(
    context.canvas.getGrContext(),
    backend,
    kTopLeft_GrSurfaceOrigin,
    kOpaque_SkAlphaType,
    nullptr
  );
  FTL_DLOG(INFO) << "texture converted to image: " << (image ? "none" : "some");
  if (image) {
    context.canvas.drawImage(image, rect_.left(), rect_.top());
  }
}
}  // namespace flow

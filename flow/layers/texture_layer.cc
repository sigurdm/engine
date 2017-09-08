// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/flow/layers/texture_layer.h"
#include "third_party/skia/include/gpu/GrTypes.h"

namespace flow {

TextureLayer::TextureLayer() = default;

TextureLayer::~TextureLayer() = default;

void TextureLayer::Preroll(PrerollContext* context, const SkMatrix& matrix) {
  set_paint_bounds(SkRect::MakeXYWH(offset_.x(), offset_.y(), size_.width(), size_.height()));
}

void TextureLayer::Paint(PaintContext& context) {
  //GrBackendTexture backend = context.compositor.BindTexture(context.canvas.getGrContext(), texture_id_);
  FTL_DLOG(INFO) << "Painting texture layer!";

  std::string string = "blah_texture";

  SkPaint paint;
  paint.setTextSize(14);
  paint.setLinearText(false);
  paint.setColor(SK_ColorRED);
  SkScalar x = paint_bounds().x();
  SkScalar y = paint_bounds().y();
  context.canvas.drawText(string.c_str(), string.size(), x, y + 7, paint);

}

}  // namespace flow
